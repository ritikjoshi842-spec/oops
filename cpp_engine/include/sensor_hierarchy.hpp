#ifndef ALF_SENSOR_HIERARCHY_HPP
#define ALF_SENSOR_HIERARCHY_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "power_subsystem.hpp"
#include "disaster_core.hpp"
#include "exceptions.hpp"

namespace DisasterCore {
namespace Sensors {

// Unit III: Diamond Problem Resolution via Virtual Base Classes:
// DisasterTelemetryDevice (Virtual Root) -> SolarPoweredNode & WirelessLoRaTransceiver -> AutonomousHybridProbe

// 1. Root Virtual Base Class
class DisasterTelemetryDevice {
protected:
    std::string m_deviceId;
    std::string m_firmwareRevision;
    uint64_t m_installationTimestamp;

public:
    DisasterTelemetryDevice(const std::string& id = "DEV-ROOT-000", 
                            const std::string& fw = "v2.4.1", 
                            uint64_t ts = 1700000000)
        : m_deviceId(id), m_firmwareRevision(fw), m_installationTimestamp(ts) {
    }

    virtual ~DisasterTelemetryDevice() {}

    virtual std::string getDeviceMetadata() const {
        std::ostringstream oss;
        oss << "RootDevice[ID:" << m_deviceId << ", FW:" << m_firmwareRevision << "]";
        return oss.str();
    }

    const std::string& getDeviceId() const noexcept { return m_deviceId; }
    const std::string& getFirmware() const noexcept { return m_firmwareRevision; }
};

// 2. Intermediate Derivation 1: SolarPoweredNode (virtually inherits DisasterTelemetryDevice)
class SolarPoweredNode : virtual public DisasterTelemetryDevice {
protected:
    double m_photovoltaicEfficiency;
    double m_dailySunHours;

public:
    SolarPoweredNode(const std::string& id = "DEV-SOLAR", 
                     const std::string& fw = "v2.4.1", 
                     uint64_t ts = 1700000000,
                     double efficiency = 0.22,
                     double sunHours = 6.5)
        : DisasterTelemetryDevice(id, fw, ts),
          m_photovoltaicEfficiency(efficiency),
          m_dailySunHours(sunHours) {
    }

    virtual ~SolarPoweredNode() {}

    double calculateDailySolarEnergyWattHours(double panelAreaM2 = 0.5) const {
        // Solar insolation ~ 1000 W/m^2 * sunHours * efficiency * area
        return 1000.0 * m_dailySunHours * m_photovoltaicEfficiency * panelAreaM2;
    }

    virtual std::string getDeviceMetadata() const override {
        std::ostringstream oss;
        oss << DisasterTelemetryDevice::getDeviceMetadata() 
            << " | Solar[Eff:" << std::fixed << std::setprecision(1) 
            << (m_photovoltaicEfficiency * 100.0) << "%]";
        return oss.str();
    }
};

// 3. Intermediate Derivation 2: WirelessLoRaTransceiver (virtually inherits DisasterTelemetryDevice)
class WirelessLoRaTransceiver : virtual public DisasterTelemetryDevice {
protected:
    double m_carrierFrequencyMHz;
    int m_transmissionPowerDbm;
    double m_currentRSSI;

public:
    WirelessLoRaTransceiver(const std::string& id = "DEV-LORA", 
                            const std::string& fw = "v2.4.1", 
                            uint64_t ts = 1700000000,
                            double freqMHz = 868.0,
                            int txPower = 14)
        : DisasterTelemetryDevice(id, fw, ts),
          m_carrierFrequencyMHz(freqMHz),
          m_transmissionPowerDbm(txPower),
          m_currentRSSI(-78.5) {
    }

    virtual ~WirelessLoRaTransceiver() {}

    bool transmitTelemetryPacket(const std::string& packetHex) {
        // Transceiver transmission simulation
        return (packetHex.length() > 0 && m_currentRSSI > -120.0);
    }

    virtual std::string getDeviceMetadata() const override {
        std::ostringstream oss;
        oss << DisasterTelemetryDevice::getDeviceMetadata() 
            << " | LoRa[" << m_carrierFrequencyMHz << "MHz, Tx:" 
            << m_transmissionPowerDbm << "dBm, RSSI:" << m_currentRSSI << "dBm]";
        return oss.str();
    }
};

// =========================================================================
// Unit III: Abstract Base Class BaseSensorNode
// With containership (aggregation) composing an independent PowerSubsystem object
// =========================================================================
class BaseSensorNode {
protected:
    std::string m_nodeId;
    std::string m_sensorType;
    bool m_isOnline;
    double m_lastReadingValue;
    uint64_t m_lastSweepTimestamp;

    // Unit III Containership: Composing independent PowerSubsystem inside BaseSensorNode
    PowerSubsystem m_powerSystem;

public:
    BaseSensorNode(const std::string& nodeId, const std::string& sensorType, 
                   double initialVoltage = 12.6, double solarWatts = 25.0)
        : m_nodeId(nodeId),
          m_sensorType(sensorType),
          m_isOnline(true),
          m_lastReadingValue(0.0),
          m_lastSweepTimestamp(0),
          m_powerSystem(initialVoltage, solarWatts) {
    }

    virtual ~BaseSensorNode() {}

    // Pure virtual functions (must be implemented by derived concrete classes)
    virtual void readTelemetry() = 0;
    virtual double evaluateThreshold() = 0;

    // Common concrete methods
    const std::string& getNodeId() const noexcept { return m_nodeId; }
    const std::string& getSensorType() const noexcept { return m_sensorType; }
    bool isOnline() const noexcept { return m_isOnline; }
    double getLastReading() const noexcept { return m_lastReadingValue; }
    uint64_t getLastSweepTimestamp() const noexcept { return m_lastSweepTimestamp; }

    // Containership access to composed PowerSubsystem
    const PowerSubsystem& getPowerSystem() const noexcept { return m_powerSystem; }
    PowerSubsystem& getPowerSystem() noexcept { return m_powerSystem; }

    virtual std::string getTelemetrySummary() const {
        std::ostringstream oss;
        oss << "Node[" << m_nodeId << " | " << m_sensorType << "] Value: " 
            << std::fixed << std::setprecision(2) << m_lastReadingValue 
            << " | Power: " << m_powerSystem.getStateString() 
            << " (" << m_powerSystem.getVoltage() << "V)";
        return oss.str();
    }
};

// =========================================================================
// 4. Derived Leaf: AutonomousHybridProbe
// Inherits from SolarPoweredNode, WirelessLoRaTransceiver, and BaseSensorNode
// Perfectly resolves Diamond Problem via single DisasterTelemetryDevice root
// =========================================================================
class AutonomousHybridProbe : public SolarPoweredNode, 
                              public WirelessLoRaTransceiver, 
                              public BaseSensorNode {
private:
    double m_geophoneResonanceHz;
    double m_tiltAngleDeg;

public:
    AutonomousHybridProbe(const std::string& id = "HYBRID-PROBE-01", 
                          double initialVoltage = 12.8, 
                          double solarWatts = 30.0)
        // Explicitly initialize virtual base class DisasterTelemetryDevice
        : DisasterTelemetryDevice(id, "v3.1.0-HYBRID", 1700500000),
          SolarPoweredNode(id, "v3.1.0-HYBRID", 1700500000, 0.24, 7.0),
          WirelessLoRaTransceiver(id, "v3.1.0-HYBRID", 1700500000, 868.3, 20),
          BaseSensorNode(id, "AUTONOMOUS_HYBRID_PROBE", initialVoltage, solarWatts),
          m_geophoneResonanceHz(4.2),
          m_tiltAngleDeg(0.0) {
    }

    virtual ~AutonomousHybridProbe() override {}

    // Virtual base final overrider resolving diamond ambiguity for getDeviceMetadata()
    virtual std::string getDeviceMetadata() const override {
        std::ostringstream oss;
        oss << "AutonomousHybridProbe[" << m_deviceId << " FW:" << m_firmwareRevision << "]"
            << " | Solar[Eff:" << (m_photovoltaicEfficiency * 100.0) << "%]"
            << " | LoRa[" << m_carrierFrequencyMHz << "MHz, Tx:" << m_transmissionPowerDbm << "dBm]";
        return oss.str();
    }

    // Pure virtual implementations
    virtual void readTelemetry() override {
        // Simulate reading hybrid telemetry
        m_lastReadingValue = m_geophoneResonanceHz * 1.5;
        m_lastSweepTimestamp = 1700550000;
        m_powerSystem.updateTelemetry(m_powerSystem.getVoltage() - 0.02, 
                                     m_powerSystem.getSolarWatts(), 
                                     m_powerSystem.getTemperature());
    }

    virtual double evaluateThreshold() override {
        // High vibration frequency combined with tilt deviation
        double risk = (m_geophoneResonanceHz / 12.0) + (m_tiltAngleDeg / 30.0);
        return std::min(risk, 1.0);
    }

    void setSensorMetrics(double vibHz, double tiltDeg) {
        m_geophoneResonanceHz = vibHz;
        m_tiltAngleDeg = tiltDeg;
        m_lastReadingValue = vibHz;
    }

    double getGeophoneResonance() const noexcept { return m_geophoneResonanceHz; }
    double getTiltAngle() const noexcept { return m_tiltAngleDeg; }

    // Combined identity from diamond inheritance
    std::string getFullDiamondSignature() const {
        std::ostringstream oss;
        oss << "HybridProbe Diamond Signature: "
            << SolarPoweredNode::getDeviceMetadata() << " & "
            << "LoRa RSSI=" << m_currentRSSI << "dBm";
        return oss.str();
    }
};

// Specialized Concrete Node 1: Soil Moisture & Seismic Geophone Node
class SoilMoistureGeophoneNode : public BaseSensorNode {
private:
    double m_soilMoisturePct;
    double m_vibrationHz;

public:
    SoilMoistureGeophoneNode(const std::string& id, double moisture = 45.0, double vibHz = 1.2)
        : BaseSensorNode(id, "SOIL_SEISMIC_GEOPHONE", 12.4, 20.0),
          m_soilMoisturePct(moisture),
          m_vibrationHz(vibHz) {
        m_lastReadingValue = moisture;
    }

    virtual ~SoilMoistureGeophoneNode() override {}

    virtual void readTelemetry() override {
        m_lastReadingValue = m_soilMoisturePct;
        m_lastSweepTimestamp = 1700550100;
    }

    virtual double evaluateThreshold() override {
        // Soil saturation > 80% with seismic frequency > 3.0 Hz is dangerous
        double hazard = (m_soilMoisturePct / 100.0) * 0.6 + (m_vibrationHz / 10.0) * 0.4;
        if (hazard > 0.90) {
            throw DisasterCore::Alerts::ThresholdOverflowException(
                "SOIL_SATURATION_CRITICAL", m_soilMoisturePct, 85.0
            );
        }
        return std::min(hazard, 1.0);
    }

    void updateReadings(double moisture, double vib) {
        m_soilMoisturePct = moisture;
        m_vibrationHz = vib;
        m_lastReadingValue = moisture;
    }

    double getMoisture() const noexcept { return m_soilMoisturePct; }
    double getVibration() const noexcept { return m_vibrationHz; }
};

// Specialized Concrete Node 2: Acoustic Tiltmeter Node
class AcousticTiltmeterNode : public BaseSensorNode {
private:
    double m_tiltDegrees;
    double m_acousticCPM; // Micro-fracture acoustic emissions

public:
    AcousticTiltmeterNode(const std::string& id, double tilt = 2.1, double acousticCPM = 45.0)
        : BaseSensorNode(id, "ACOUSTIC_TILTMETER", 12.5, 22.0),
          m_tiltDegrees(tilt),
          m_acousticCPM(acousticCPM) {
        m_lastReadingValue = tilt;
    }

    virtual ~AcousticTiltmeterNode() override {}

    virtual void readTelemetry() override {
        m_lastReadingValue = m_tiltDegrees;
        m_lastSweepTimestamp = 1700550200;
    }

    virtual double evaluateThreshold() override {
        // Rapid slope tilt change > 5 deg or acoustic emissions > 250 cpm
        double hazard = (m_tiltDegrees / 20.0) * 0.5 + (m_acousticCPM / 400.0) * 0.5;
        if (m_tiltDegrees > 15.0) {
            throw DisasterCore::Alerts::ThresholdOverflowException(
                "SLOPE_TILT_COLLAPSE_ANGLE", m_tiltDegrees, 15.0
            );
        }
        return std::min(hazard, 1.0);
    }

    void updateReadings(double tilt, double cpm) {
        m_tiltDegrees = tilt;
        m_acousticCPM = cpm;
        m_lastReadingValue = tilt;
    }

    double getTilt() const noexcept { return m_tiltDegrees; }
    double getAcousticCPM() const noexcept { return m_acousticCPM; }
};

// Specialized Concrete Node 3: River-Level Sonar Gauge Node
class RiverSonarGaugeNode : public BaseSensorNode {
private:
    double m_waterLevelMeters;
    double m_rateOfRiseMetersPerHour;

public:
    RiverSonarGaugeNode(const std::string& id, double level = 3.2, double riseRate = 0.4)
        : BaseSensorNode(id, "RIVER_SONAR_GAUGE", 12.6, 28.0),
          m_waterLevelMeters(level),
          m_rateOfRiseMetersPerHour(riseRate) {
        m_lastReadingValue = level;
    }

    virtual ~RiverSonarGaugeNode() override {}

    virtual void readTelemetry() override {
        m_lastReadingValue = m_waterLevelMeters;
        m_lastSweepTimestamp = 1700550300;
    }

    virtual double evaluateThreshold() override {
        // River level > 8.0 meters is high flash flood risk
        double hazard = (m_waterLevelMeters / 10.0) * 0.7 + (m_rateOfRiseMetersPerHour / 3.0) * 0.3;
        if (m_waterLevelMeters > 9.5) {
            throw DisasterCore::Alerts::ThresholdOverflowException(
                "RIVER_FLOOD_CREST_OVERFLOW", m_waterLevelMeters, 9.5
            );
        }
        return std::min(hazard, 1.0);
    }

    void updateReadings(double level, double riseRate) {
        m_waterLevelMeters = level;
        m_rateOfRiseMetersPerHour = riseRate;
        m_lastReadingValue = level;
    }

    double getWaterLevel() const noexcept { return m_waterLevelMeters; }
    double getRateOfRise() const noexcept { return m_rateOfRiseMetersPerHour; }
};

} // namespace Sensors
} // namespace DisasterCore

#endif // ALF_SENSOR_HIERARCHY_HPP
