#ifndef ALF_PERSISTENCE_ENGINE_HPP
#define ALF_PERSISTENCE_ENGINE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include "exceptions.hpp"
#include "disaster_core.hpp"

namespace DisasterCore {
namespace Alerts {

#pragma pack(push, 1)
// Binary Incident Record struct for serialization to disk
struct IncidentRecord {
    uint32_t magicHeader;      // 0x414C4631 -> "ALF1"
    uint32_t recordId;
    uint64_t timestamp;
    char stationId[32];
    char zoneName[32];
    double hazardIndex;
    int alertLevel;
    char summary[128];
    uint32_t crcChecksum;

    IncidentRecord()
        : magicHeader(0x414C4631),
          recordId(0),
          timestamp(0),
          hazardIndex(0.0),
          alertLevel(0),
          crcChecksum(0) {
        std::memset(stationId, 0, sizeof(stationId));
        std::memset(zoneName, 0, sizeof(zoneName));
        std::memset(summary, 0, sizeof(summary));
    }

    void calculateChecksum() {
        uint32_t sum = magicHeader ^ recordId ^ static_cast<uint32_t>(timestamp);
        sum ^= static_cast<uint32_t>(hazardIndex * 1000.0);
        sum ^= static_cast<uint32_t>(alertLevel);
        for (size_t i = 0; i < sizeof(stationId); ++i) sum = (sum * 31) + stationId[i];
        crcChecksum = sum;
    }

    bool isValid() const {
        if (magicHeader != 0x414C4631) return false;
        uint32_t sum = magicHeader ^ recordId ^ static_cast<uint32_t>(timestamp);
        sum ^= static_cast<uint32_t>(hazardIndex * 1000.0);
        sum ^= static_cast<uint32_t>(alertLevel);
        for (size_t i = 0; i < sizeof(stationId); ++i) sum = (sum * 31) + stationId[i];
        return crcChecksum == sum;
    }
};
#pragma pack(pop)

// =========================================================================
// Unit IV: Persistent file stream logging using binary std::fstream
// Implementing transactional rollbacks using seekp() and seekg()
// =========================================================================
class PersistenceEngine {
private:
    std::string m_filepath;
    uint32_t m_nextRecordId;

public:
    explicit PersistenceEngine(const std::string& path = "data/incidents.bin")
        : m_filepath(path), m_nextRecordId(1001) {
        // Initialize file if not existing
        std::ifstream check(m_filepath, std::ios::binary);
        if (!check.is_open()) {
            std::ofstream init(m_filepath, std::ios::binary | std::ios::trunc);
            init.close();
        } else {
            check.close();
            // Determine next record ID
            auto existing = readAllIncidents();
            if (!existing.empty()) {
                m_nextRecordId = existing.back().recordId + 1;
            }
        }
    }

    // Atomically write an incident record with transactional rollback on failure
    bool logIncidentTransactional(IncidentRecord& record, bool simulateFailure = false) {
        // Ensure file exists first so in | out | binary succeeds on Windows
        {
            std::ofstream touch(m_filepath, std::ios::binary | std::ios::app);
        }

        // Open binary stream for read and write
        std::fstream file(m_filepath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            throw SensorFailureException("PERSISTENCE_SYS", 0x5001, "Cannot open binary log stream: " + m_filepath);
        }

        // Seek to end of file to prepare append
        file.seekp(0, std::ios::end);

        // TRANSACTION START: Record checkpoint position using tellp()
        std::streampos checkpointPos = file.tellp();

        record.recordId = m_nextRecordId++;
        record.calculateChecksum();

        // Write the binary packet
        file.write(reinterpret_cast<const char*>(&record), sizeof(IncidentRecord));
        file.flush();

        // Check for simulated failure or stream write corruption
        if (simulateFailure || file.bad() || file.fail()) {
            // TRANSACTION ROLLBACK: Seek write pointer back to checkpoint
            file.clear(); // Clear error state flags
            file.seekp(checkpointPos); // Rollback seekp() to checkpoint!

            // Overwrite corrupt bytes with 0 or truncate
            char zeroBuffer[sizeof(IncidentRecord)];
            std::memset(zeroBuffer, 0, sizeof(zeroBuffer));
            file.write(zeroBuffer, sizeof(IncidentRecord));
            file.seekp(checkpointPos); // Reposition back to checkpoint
            file.flush();
            file.close();

            // Rewind record ID
            m_nextRecordId--;

            std::cerr << "[PersistenceEngine] Transaction ABORTED! seekp() rolled back to byte offset " 
                      << static_cast<long long>(checkpointPos) << std::endl;
            return false;
        }

        file.close();
        return true;
    }

    // Read all records using binary std::fstream seekg()
    std::vector<IncidentRecord> readAllIncidents() {
        std::vector<IncidentRecord> records;
        std::ifstream file(m_filepath, std::ios::binary);
        if (!file.is_open()) {
            return records;
        }

        file.seekg(0, std::ios::beg);
        IncidentRecord rec;
        while (file.read(reinterpret_cast<char*>(&rec), sizeof(IncidentRecord))) {
            if (rec.isValid()) {
                records.push_back(rec);
            }
        }

        file.close();
        return records;
    }

    // Read specific record at index offset using seekg()
    bool readIncidentAt(size_t index, IncidentRecord& outRecord) {
        std::ifstream file(m_filepath, std::ios::binary);
        if (!file.is_open()) return false;

        std::streampos targetPos = static_cast<std::streampos>(index * sizeof(IncidentRecord));
        file.seekg(targetPos, std::ios::beg); // Direct random access via seekg()

        if (file.read(reinterpret_cast<char*>(&outRecord), sizeof(IncidentRecord))) {
            file.close();
            return outRecord.isValid();
        }

        file.close();
        return false;
    }

    size_t getIncidentCount() {
        return readAllIncidents().size();
    }

    void clearAll() {
        std::ofstream file(m_filepath, std::ios::binary | std::ios::trunc);
        file.close();
        m_nextRecordId = 1001;
    }

    const std::string& getFilePath() const noexcept { return m_filepath; }
};

} // namespace Alerts
} // namespace DisasterCore

#endif // ALF_PERSISTENCE_ENGINE_HPP
