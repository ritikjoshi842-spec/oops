#ifndef ALF_EXCEPTIONS_HPP
#define ALF_EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <sstream>

namespace DisasterCore {
namespace Alerts {

// Unit IV: Custom exception derived from std::exception for hardware / sensor telemetry faults
class SensorFailureException : public std::exception {
private:
    std::string m_message;
    std::string m_sensorId;
    int m_faultCode;

public:
    SensorFailureException(const std::string& sensorId, int faultCode, const std::string& detail)
        : m_sensorId(sensorId), m_faultCode(faultCode) {
        std::ostringstream oss;
        oss << "[SensorFailureException] Critical failure at Node [" << sensorId 
            << "] (Fault Code: 0x" << std::hex << faultCode << "): " << detail;
        m_message = oss.str();
    }

    virtual const char* what() const noexcept override {
        return m_message.c_str();
    }

    const std::string& getSensorId() const noexcept { return m_sensorId; }
    int getFaultCode() const noexcept { return m_faultCode; }
};

// Unit IV: Custom exception for telemetry corruption / CRC mismatch
class TelemetryChecksumMismatch : public std::exception {
private:
    std::string m_message;
    uint32_t m_expectedChecksum;
    uint32_t m_actualChecksum;

public:
    TelemetryChecksumMismatch(const std::string& packetId, uint32_t expected, uint32_t actual)
        : m_expectedChecksum(expected), m_actualChecksum(actual) {
        std::ostringstream oss;
        oss << "[TelemetryChecksumMismatch] Packet [" << packetId 
            << "] CRC validation failed. Expected: 0x" << std::hex << expected 
            << ", Received: 0x" << actual;
        m_message = oss.str();
    }

    virtual const char* what() const noexcept override {
        return m_message.c_str();
    }

    uint32_t getExpected() const noexcept { return m_expectedChecksum; }
    uint32_t getActual() const noexcept { return m_actualChecksum; }
};

// Unit IV: Custom exception for safety limits overflow
class ThresholdOverflowException : public std::exception {
private:
    std::string m_message;
    std::string m_metric;
    double m_measuredValue;
    double m_thresholdLimit;

public:
    ThresholdOverflowException(const std::string& metric, double val, double limit)
        : m_metric(metric), m_measuredValue(val), m_thresholdLimit(limit) {
        std::ostringstream oss;
        oss << "[ThresholdOverflowException] Metric [" << metric 
            << "] exceeded catastrophic safety threshold! Value=" << val 
            << ", Limit=" << limit;
        m_message = oss.str();
    }

    virtual const char* what() const noexcept override {
        return m_message.c_str();
    }

    double getMeasuredValue() const noexcept { return m_measuredValue; }
    double getThresholdLimit() const noexcept { return m_thresholdLimit; }
};

} // namespace Alerts
} // namespace DisasterCore

#endif // ALF_EXCEPTIONS_HPP
