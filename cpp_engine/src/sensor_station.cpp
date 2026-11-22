#include "../include/sensor_station.hpp"

namespace DisasterCore {
namespace Sensors {

// Definition and initialization of static data members
int SensorStation::s_activeStationCount = 0;
int SensorStation::s_globalEmergencyAlertLevel = 0;

} // namespace Sensors
} // namespace DisasterCore
