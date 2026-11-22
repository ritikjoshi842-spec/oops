#ifndef ALF_TELEMETRY_BUFFER_HPP
#define ALF_TELEMETRY_BUFFER_HPP

#include <cstddef>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <utility>
#include "disaster_core.hpp"
#include "exceptions.hpp"

namespace DisasterCore {
namespace Sensors {

struct SensorSample {
    uint64_t timestamp;
    double sensorValue;
    uint32_t checksum;
    char sensorTag[16];

    SensorSample(uint64_t ts = 0, double val = 0.0, const char* tag = "GENERIC")
        : timestamp(ts), sensorValue(val), checksum(0) {
        std::strncpy(sensorTag, tag, sizeof(sensorTag) - 1);
        sensorTag[sizeof(sensorTag) - 1] = '\0';
        computeChecksum();
    }

    void computeChecksum() {
        uint64_t valBits = 0;
        std::memcpy(&valBits, &sensorValue, sizeof(double));
        uint32_t sum1 = 0xffff & (timestamp ^ valBits);
        uint32_t sum2 = 0xffff & (timestamp >> 16);
        checksum = (sum2 << 16) | sum1;
    }

    bool verifyChecksum() const {
        uint64_t valBits = 0;
        std::memcpy(&valBits, &sensorValue, sizeof(double));
        uint32_t sum1 = 0xffff & (timestamp ^ valBits);
        uint32_t sum2 = 0xffff & (timestamp >> 16);
        uint32_t expected = (sum2 << 16) | sum1;
        return checksum == expected;
    }
};

// =========================================================================
// Unit II: Dynamic Sensor Memory Buffer with Parameterized, Dynamic, 
// Deep Copy Constructors, and Explicit Destructors
// =========================================================================
class TelemetryBuffer {
private:
    SensorSample* m_dataBuffer;   // Dynamic memory buffer pointer
    size_t m_capacity;           // Maximum slots allocated
    size_t m_size;               // Current valid entries
    size_t m_allocationCount;    // Debug audit tracker

public:
    // 1. Parameterized / Dynamic Constructor
    explicit TelemetryBuffer(size_t initialCapacity = 32)
        : m_capacity(initialCapacity > 0 ? initialCapacity : 16),
          m_size(0),
          m_allocationCount(1) {
        // Dynamic constructor allocating heap memory
        m_dataBuffer = new SensorSample[m_capacity];
    }

    // 2. Explicit Destructor: eliminates memory leaks
    ~TelemetryBuffer() {
        if (m_dataBuffer != nullptr) {
            delete[] m_dataBuffer;
            m_dataBuffer = nullptr;
        }
        m_size = 0;
        m_capacity = 0;
    }

    // 3. Deep Copy Constructor: eliminates shallow copy dangling pointer bugs
    TelemetryBuffer(const TelemetryBuffer& other)
        : m_capacity(other.m_capacity),
          m_size(other.m_size),
          m_allocationCount(other.m_allocationCount + 1) {
        m_dataBuffer = new SensorSample[m_capacity];
        for (size_t i = 0; i < m_size; ++i) {
            m_dataBuffer[i] = other.m_dataBuffer[i]; // Deep value copy
        }
    }

    // 4. Copy Assignment Operator (Rule of Three / Five)
    TelemetryBuffer& operator=(const TelemetryBuffer& other) {
        if (this != &other) {
            // Allocate new buffer first for strong exception safety
            SensorSample* newBuffer = new SensorSample[other.m_capacity];
            for (size_t i = 0; i < other.m_size; ++i) {
                newBuffer[i] = other.m_dataBuffer[i];
            }

            // Release old resources
            delete[] m_dataBuffer;

            m_dataBuffer = newBuffer;
            m_capacity = other.m_capacity;
            m_size = other.m_size;
            m_allocationCount++;
        }
        return *this;
    }

    // 5. Move Constructor (C++11/14 compliant efficiency)
    TelemetryBuffer(TelemetryBuffer&& other) noexcept
        : m_dataBuffer(other.m_dataBuffer),
          m_capacity(other.m_capacity),
          m_size(other.m_size),
          m_allocationCount(other.m_allocationCount) {
        other.m_dataBuffer = nullptr;
        other.m_capacity = 0;
        other.m_size = 0;
    }

    // 6. Move Assignment Operator
    TelemetryBuffer& operator=(TelemetryBuffer&& other) noexcept {
        if (this != &other) {
            delete[] m_dataBuffer;
            m_dataBuffer = other.m_dataBuffer;
            m_capacity = other.m_capacity;
            m_size = other.m_size;
            m_allocationCount = other.m_allocationCount;

            other.m_dataBuffer = nullptr;
            other.m_capacity = 0;
            other.m_size = 0;
        }
        return *this;
    }

    // Append sample with dynamic capacity growth
    void push(const SensorSample& sample) {
        if (!sample.verifyChecksum()) {
            throw DisasterCore::Alerts::TelemetryChecksumMismatch(
                sample.sensorTag, 0xAA55, sample.checksum
            );
        }

        if (m_size >= m_capacity) {
            resize(m_capacity * 2);
        }
        m_dataBuffer[m_size++] = sample;
    }

    void resize(size_t newCapacity) {
        if (newCapacity <= m_capacity) return;
        SensorSample* newBuffer = new SensorSample[newCapacity];
        for (size_t i = 0; i < m_size; ++i) {
            newBuffer[i] = m_dataBuffer[i];
        }
        delete[] m_dataBuffer;
        m_dataBuffer = newBuffer;
        m_capacity = newCapacity;
        m_allocationCount++;
    }

    // Const-correct element access
    const SensorSample& operator[](size_t index) const {
        return m_dataBuffer[index];
    }

    SensorSample& operator[](size_t index) {
        return m_dataBuffer[index];
    }

    size_t size() const noexcept { return m_size; }
    size_t capacity() const noexcept { return m_capacity; }
    bool isEmpty() const noexcept { return m_size == 0; }
    void clear() noexcept { m_size = 0; }

    const SensorSample* getRawBuffer() const noexcept { return m_dataBuffer; }
    uintptr_t getBufferAddress() const noexcept { return reinterpret_cast<uintptr_t>(m_dataBuffer); }
};

} // namespace Sensors
} // namespace DisasterCore

#endif // ALF_TELEMETRY_BUFFER_HPP
