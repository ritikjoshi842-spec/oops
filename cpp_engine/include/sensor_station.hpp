#ifndef ALF_SENSOR_STATION_HPP
#define ALF_SENSOR_STATION_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "disaster_core.hpp"
#include "telemetry_buffer.hpp"
#include "sensor_hierarchy.hpp"
#include "exceptions.hpp"

namespace DisasterCore {
namespace Alerts {

// Unit II: EarlyWarningSiren entity
class EarlyWarningSiren {
private:
    std::string m_sirenId;
    int m_decibelLevel;        // e.g. 135 dB
    bool m_strobeActive;
    bool m_acousticSounding;
    std::string m_sirenType;   // e.g. "ROTATING_ELECTROMECHANICAL"

public:
    EarlyWarningSiren(const std::string& id = "SIREN-01", int db = 135, const std::string& type = "HIMALAYAN_OMNI_135DB")
        : m_sirenId(id), m_decibelLevel(db), m_strobeActive(false), 
          m_acousticSounding(false), m_sirenType(type) {}

    void triggerAlarm(AlertLevel level) {
        if (level == AlertLevel::RED_EVACUATE) {
            m_strobeActive = true;
            m_acousticSounding = true;
        } else if (level == AlertLevel::ORANGE_WARNING) {
            m_strobeActive = true;
            m_acousticSounding = false;
        } else {
            m_strobeActive = false;
            m_acousticSounding = false;
        }
    }

    void silence() {
        m_strobeActive = false;
        m_acousticSounding = false;
    }

    const std::string& getId() const noexcept { return m_sirenId; }
    int getDecibels() const noexcept { return m_decibelLevel; }
    bool isStrobeActive() const noexcept { return m_strobeActive; }
    bool isSounding() const noexcept { return m_acousticSounding; }
    const std::string& getType() const noexcept { return m_sirenType; }
};

} // namespace Alerts

namespace Sensors {

// Forward declaration of friend auditor class
class TelemetryAuditor;

// Unit II: RegionalZoneCluster for aggregating multiple sensor stations/clusters using binary operator +
class RegionalZoneCluster {
private:
    std::string m_zoneName;
    std::vector<std::string> m_stationIds;
    double m_cumulativeHazardScore;
    int m_totalSensorNodes;

public:
    RegionalZoneCluster(const std::string& zone = "DEFAULT_ZONE")
        : m_zoneName(zone), m_cumulativeHazardScore(0.0), m_totalSensorNodes(0) {}

    RegionalZoneCluster(const std::string& zone, const std::string& stationId, double hazard, int sensorCount)
        : m_zoneName(zone), m_cumulativeHazardScore(hazard), m_totalSensorNodes(sensorCount) {
        m_stationIds.push_back(stationId);
    }

    // Unit II: Overload binary + to aggregate multiple sensor clusters into regional zone vectors
    RegionalZoneCluster operator+(const RegionalZoneCluster& rhs) const {
        RegionalZoneCluster merged(m_zoneName + "+" + rhs.m_zoneName);
        merged.m_stationIds = m_stationIds;
        merged.m_stationIds.insert(merged.m_stationIds.end(), rhs.m_stationIds.begin(), rhs.m_stationIds.end());
        merged.m_cumulativeHazardScore = m_cumulativeHazardScore + rhs.m_cumulativeHazardScore;
        merged.m_totalSensorNodes = m_totalSensorNodes + rhs.m_totalSensorNodes;
        return merged;
    }

    const std::string& getZoneName() const noexcept { return m_zoneName; }
    const std::vector<std::string>& getStationIds() const noexcept { return m_stationIds; }
    double getCumulativeHazard() const noexcept { return m_cumulativeHazardScore; }
    int getTotalSensorNodes() const noexcept { return m_totalSensorNodes; }
    double getAverageHazard() const noexcept {
        return m_stationIds.empty() ? 0.0 : (m_cumulativeHazardScore / m_stationIds.size());
    }
};

// =========================================================================
// Unit II: SensorStation Entity with strict data abstraction, static members,
// dynamic buffers, operator overloads, and friend auditor access
// =========================================================================
class SensorStation {
    // Unit I: Friend class TelemetryAuditor to inspect raw private metrics without getters
    friend class TelemetryAuditor;

    // Unit I: Friend function to inspect private calibration internals
    friend void auditRawHardwareInternals(const SensorStation& station, std::ostream& os);

private:
    std::string m_stationId;
    std::string m_locationName;
    GeoCoordinate m_coordinates;
    double m_computedHazardScore;
    Alerts::AlertLevel m_stationAlertLevel;

    // Unit II Dynamic Memory Buffer
    TelemetryBuffer m_telemetryBuffer;

    // Unit II Siren entity
    Alerts::EarlyWarningSiren m_siren;

    // Unit III Base pointer vector for runtime dynamic dispatch (vtable / _vptr)
    std::vector<BaseSensorNode*> m_sensorNodes;

    // Private raw hardware calibration registers (inspected exclusively by friend TelemetryAuditor)
    uint32_t m_privateCalibrationRegister;
    double m_rawAdcVoltageBias;
    bool m_tamperSensorTripped;

    // Static data members
    static int s_activeStationCount;
    static int s_globalEmergencyAlertLevel; // 0=GREEN, 1=YELLOW, 2=ORANGE, 3=RED

public:
    // Parameterized Constructor
    SensorStation(const std::string& id, const std::string& loc, double lat, double lon, double elev)
        : m_stationId(id),
          m_locationName(loc),
          m_coordinates(lat, lon, elev),
          m_computedHazardScore(0.0),
          m_stationAlertLevel(Alerts::AlertLevel::GREEN_NOMINAL),
          m_telemetryBuffer(64),
          m_siren("SIREN-" + id, 135),
          m_privateCalibrationRegister(0xDEADBEEF),
          m_rawAdcVoltageBias(0.0142),
          m_tamperSensorTripped(false) {
        
        // Transform coordinates using call-by-reference and return-by-reference
        GeoCoordinate transformed;
        transformCoordinateWGS84toUTM(m_coordinates, transformed);
        m_coordinates = transformed;

        // Increment network-wide active station counter
        s_activeStationCount++;
    }

    // Destructor: properly clean up station instances and dynamic pointers
    ~SensorStation() {
        if (s_activeStationCount > 0) {
            s_activeStationCount--;
        }
        // Note: m_sensorNodes pointers are deleted if owned by this station
        for (auto* node : m_sensorNodes) {
            delete node;
        }
        m_sensorNodes.clear();
    }

    // Disable copy constructor or handle deep copy cleanly to prevent duplicate static counting
    SensorStation(const SensorStation& other)
        : m_stationId(other.m_stationId + "_COPY"),
          m_locationName(other.m_locationName),
          m_coordinates(other.m_coordinates),
          m_computedHazardScore(other.m_computedHazardScore),
          m_stationAlertLevel(other.m_stationAlertLevel),
          m_telemetryBuffer(other.m_telemetryBuffer),
          m_siren(other.m_siren),
          m_privateCalibrationRegister(other.m_privateCalibrationRegister),
          m_rawAdcVoltageBias(other.m_rawAdcVoltageBias),
          m_tamperSensorTripped(other.m_tamperSensorTripped) {
        s_activeStationCount++;
    }

    SensorStation& operator=(const SensorStation& other) {
        if (this != &other) {
            m_stationId = other.m_stationId;
            m_locationName = other.m_locationName;
            m_coordinates = other.m_coordinates;
            m_computedHazardScore = other.m_computedHazardScore;
            m_stationAlertLevel = other.m_stationAlertLevel;
            m_telemetryBuffer = other.m_telemetryBuffer;
            m_siren = other.m_siren;
            m_privateCalibrationRegister = other.m_privateCalibrationRegister;
            m_rawAdcVoltageBias = other.m_rawAdcVoltageBias;
            m_tamperSensorTripped = other.m_tamperSensorTripped;
        }
        return *this;
    }

    // Attach sensor node to vector of base pointers
    void attachSensorNode(BaseSensorNode* node) {
        if (node != nullptr) {
            m_sensorNodes.push_back(node);
        }
    }

    // =====================================================================
    // Unit III: Dynamic Dispatch Sweep Cycle (demonstrating vtable / _vptr)
    // =====================================================================
    void executeSensorSweepCycle() {
        double maxHazard = 0.0;
        uint64_t nowTs = 1700600000;

        for (BaseSensorNode* nodePtr : m_sensorNodes) {
            // Polymorphic call resolved dynamically at runtime through vtable
            nodePtr->readTelemetry();
            double nodeHazard = nodePtr->evaluateThreshold();
            if (nodeHazard > maxHazard) {
                maxHazard = nodeHazard;
            }

            // Buffer sample in dynamic buffer
            SensorSample sample(nowTs, nodePtr->getLastReading(), nodePtr->getSensorType().c_str());
            m_telemetryBuffer.push(sample);
        }

        m_computedHazardScore = maxHazard;
        m_stationAlertLevel = Alerts::evaluateHazardToAlert(m_computedHazardScore);
        m_siren.triggerAlarm(m_stationAlertLevel);

        // Update global emergency alert level if this station exceeds current
        if (static_cast<int>(m_stationAlertLevel) > s_globalEmergencyAlertLevel) {
            s_globalEmergencyAlertLevel = static_cast<int>(m_stationAlertLevel);
        }
    }

    // Static member functions
    static int getActiveStationCount() noexcept {
        return s_activeStationCount;
    }

    static int getGlobalEmergencyAlertLevel() noexcept {
        return s_globalEmergencyAlertLevel;
    }

    static void setGlobalEmergencyAlertLevel(int level) noexcept {
        s_globalEmergencyAlertLevel = level;
    }

    // =====================================================================
    // Unit II Operator Overloading
    // =====================================================================

    // Relational operator > to prioritize higher hazard values
    bool operator>(const SensorStation& rhs) const noexcept {
        return this->m_computedHazardScore > rhs.m_computedHazardScore;
    }

    bool operator<(const SensorStation& rhs) const noexcept {
        return this->m_computedHazardScore < rhs.m_computedHazardScore;
    }

    // Stream insertion (<<) operator for formatted output to console and streams
    friend std::ostream& operator<<(std::ostream& os, const SensorStation& station) {
        os << "Station[" << station.m_stationId << " | " << station.m_locationName 
           << "] Coord:(" << std::fixed << std::setprecision(4) 
           << station.m_coordinates.latitude << "N, " 
           << station.m_coordinates.longitude << "E, " 
           << station.m_coordinates.elevationMeters << "m) "
           << "Hazard:" << std::fixed << std::setprecision(2) << station.m_computedHazardScore 
           << " Alert:" << Alerts::getAlertLevelString(station.m_stationAlertLevel)
           << " Siren:" << (station.m_siren.isSounding() ? "SOUNDING" : "QUIET")
           << " Nodes:" << station.m_sensorNodes.size()
           << " BufferSamples:" << station.m_telemetryBuffer.size();
        return os;
    }

    // Stream extraction (>>) operator for deserializing station configuration
    friend std::istream& operator>>(std::istream& is, SensorStation& station) {
        std::string id, loc;
        double lat, lon, elev;
        if (is >> id >> loc >> lat >> lon >> elev) {
            station.m_stationId = id;
            station.m_locationName = loc;
            station.m_coordinates = GeoCoordinate(lat, lon, elev);
            GeoCoordinate transformed;
            transformCoordinateWGS84toUTM(station.m_coordinates, transformed);
            station.m_coordinates = transformed;
        }
        return is;
    }

    // Const-correct getters
    const std::string& getId() const noexcept { return m_stationId; }
    const std::string& getLocation() const noexcept { return m_locationName; }
    const GeoCoordinate& getCoordinates() const noexcept { return m_coordinates; }
    double getHazardScore() const noexcept { return m_computedHazardScore; }
    Alerts::AlertLevel getAlertLevel() const noexcept { return m_stationAlertLevel; }
    const Alerts::EarlyWarningSiren& getSiren() const noexcept { return m_siren; }
    Alerts::EarlyWarningSiren& getSiren() noexcept { return m_siren; }
    const TelemetryBuffer& getBuffer() const noexcept { return m_telemetryBuffer; }
    const std::vector<BaseSensorNode*>& getSensorNodes() const noexcept { return m_sensorNodes; }

    void setHazardScore(double score) {
        m_computedHazardScore = score;
        m_stationAlertLevel = Alerts::evaluateHazardToAlert(score);
        m_siren.triggerAlarm(m_stationAlertLevel);
        if (static_cast<int>(m_stationAlertLevel) > s_globalEmergencyAlertLevel) {
            s_globalEmergencyAlertLevel = static_cast<int>(m_stationAlertLevel);
        }
    }
};

// =========================================================================
// Unit I: Friend Class TelemetryAuditor
// Inspects raw private sensor metrics without exposing public getters
// =========================================================================
class TelemetryAuditor {
public:
    static std::string performFullSecurityAudit(const SensorStation& station) {
        std::ostringstream oss;
        oss << "=== TELEMETRY AUDITOR DEEP MEMORY PROBE ===\n"
            << "Target Station ID: " << station.m_stationId << "\n"
            << "Raw Private Calibration Reg: 0x" << std::hex << station.m_privateCalibrationRegister << "\n"
            << "Raw ADC Voltage Bias: " << std::dec << station.m_rawAdcVoltageBias << " V\n"
            << "Hardware Tamper Tripped: " << (station.m_tamperSensorTripped ? "YES (BREACH)" : "NO (SECURE)") << "\n"
            << "Internal TelemetryBuffer Heap Address: 0x" << std::hex << station.m_telemetryBuffer.getBufferAddress() << "\n"
            << "Internal Buffer Capacity: " << std::dec << station.m_telemetryBuffer.capacity() << " slots\n"
            << "Internal Buffer Size: " << station.m_telemetryBuffer.size() << " samples\n"
            << "Active Station Count (Static): " << SensorStation::s_activeStationCount << "\n"
            << "Global Emergency Level (Static): " << SensorStation::s_globalEmergencyAlertLevel << "\n"
            << "===========================================";
        return oss.str();
    }

    static uint32_t getCalibrationRegister(const SensorStation& station) {
        return station.m_privateCalibrationRegister;
    }

    static double getRawBias(const SensorStation& station) {
        return station.m_rawAdcVoltageBias;
    }
};

// Unit I Friend Function implementation
inline void auditRawHardwareInternals(const SensorStation& station, std::ostream& os) {
    os << "[Friend Audit] Station:" << station.m_stationId 
       << " Reg:0x" << std::hex << station.m_privateCalibrationRegister 
       << " Bias:" << std::dec << station.m_rawAdcVoltageBias << "V";
}

} // namespace Sensors
} // namespace DisasterCore

#endif // ALF_SENSOR_STATION_HPP
