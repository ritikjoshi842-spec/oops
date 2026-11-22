#ifndef ALF_POWER_SUBSYSTEM_HPP
#define ALF_POWER_SUBSYSTEM_HPP

#include <string>
#include <iostream>
#include <sstream>

namespace DisasterCore {
namespace Sensors {

enum class PowerState {
    OPTIMAL,
    BATTERY_DISCHARGING,
    SOLAR_CHARGING,
    CRITICAL_RESERVE,
    EMERGENCY_SHUTDOWN
};

// Unit III: Independent PowerSubsystem object used for containership (aggregation) inside BaseSensorNode
class PowerSubsystem {
private:
    double m_batteryVoltage;    // in Volts (nominal: 12.6V)
    double m_solarOutputWatts;   // in Watts
    double m_cellTemperature;    // in Celsius
    PowerState m_state;
    bool m_backupAuxiliaryEngaged;

public:
    PowerSubsystem(double voltage = 12.6, double solarWatts = 25.0, double temp = 18.5)
        : m_batteryVoltage(voltage),
          m_solarOutputWatts(solarWatts),
          m_cellTemperature(temp),
          m_state(PowerState::OPTIMAL),
          m_backupAuxiliaryEngaged(false) {
        evaluatePowerState();
    }

    void evaluatePowerState() {
        if (m_batteryVoltage < 10.8) {
            m_state = PowerState::CRITICAL_RESERVE;
            m_backupAuxiliaryEngaged = true;
        } else if (m_solarOutputWatts > 10.0) {
            m_state = PowerState::SOLAR_CHARGING;
            m_backupAuxiliaryEngaged = false;
        } else if (m_solarOutputWatts <= 2.0 && m_batteryVoltage >= 11.5) {
            m_state = PowerState::BATTERY_DISCHARGING;
            m_backupAuxiliaryEngaged = false;
        } else {
            m_state = PowerState::OPTIMAL;
        }
    }

    void updateTelemetry(double voltage, double solarWatts, double temp) {
        m_batteryVoltage = voltage;
        m_solarOutputWatts = solarWatts;
        m_cellTemperature = temp;
        evaluatePowerState();
    }

    double getVoltage() const noexcept { return m_batteryVoltage; }
    double getSolarWatts() const noexcept { return m_solarOutputWatts; }
    double getTemperature() const noexcept { return m_cellTemperature; }
    PowerState getState() const noexcept { return m_state; }
    bool isBackupEngaged() const noexcept { return m_backupAuxiliaryEngaged; }

    std::string getStateString() const {
        switch (m_state) {
            case PowerState::OPTIMAL: return "OPTIMAL";
            case PowerState::SOLAR_CHARGING: return "SOLAR_CHARGING";
            case PowerState::BATTERY_DISCHARGING: return "BATTERY_DISCHARGING";
            case PowerState::CRITICAL_RESERVE: return "CRITICAL_RESERVE";
            case PowerState::EMERGENCY_SHUTDOWN: return "EMERGENCY_SHUTDOWN";
            default: return "UNKNOWN";
        }
    }

    void displayStatus(std::ostream& os) const {
        os << "[PowerSubsystem: " << m_batteryVoltage << "V, " 
           << m_solarOutputWatts << "W Solar, Temp: " << m_cellTemperature << "C, State: " 
           << getStateString() << "]";
    }
};

} // namespace Sensors
} // namespace DisasterCore

#endif // ALF_POWER_SUBSYSTEM_HPP
