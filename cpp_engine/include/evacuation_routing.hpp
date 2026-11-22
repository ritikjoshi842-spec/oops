#ifndef ALF_EVACUATION_ROUTING_HPP
#define ALF_EVACUATION_ROUTING_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "disaster_core.hpp"

namespace DisasterCore {
namespace Alerts {

struct EvacuationRoute {
    std::string routeId;
    std::string sourceZone;
    std::string safeHavenSector;
    double roadLengthKm;
    double currentHazardExposure; // 0.0 to 1.0
    int transitCapacityBuses;
    bool isBridgeIntact;
    double computedRiskScore;     // Lower is safer; Functor sorts highest urgency first

    EvacuationRoute(const std::string& id = "RT-01",
                    const std::string& src = "KEDARNATH_UPPER",
                    const std::string& dest = "GUPTAKASHI_STAGING",
                    double km = 18.5,
                    double hazard = 0.65,
                    int capacity = 40,
                    bool bridgeOk = true)
        : routeId(id), sourceZone(src), safeHavenSector(dest),
          roadLengthKm(km), currentHazardExposure(hazard),
          transitCapacityBuses(capacity), isBridgeIntact(bridgeOk) {
        computeRiskScore();
    }

    void computeRiskScore() {
        // Priority formula: high hazard + compromised bridge + high transit capacity demands immediate action
        double bridgePenalty = isBridgeIntact ? 1.0 : 3.5;
        computedRiskScore = (currentHazardExposure * 100.0 * bridgePenalty) + (roadLengthKm / 5.0) - (transitCapacityBuses * 0.1);
    }
};

// =========================================================================
// Unit IV: Custom Functor for priority-based evacuation routing
// Used directly with std::sort() to prioritize critical routes first
// =========================================================================
struct EvacuationPriorityFunctor {
    bool operator()(const EvacuationRoute& a, const EvacuationRoute& b) const {
        // Strict weak ordering: Highest risk score has priority for emergency evacuation dispatch
        if (a.computedRiskScore != b.computedRiskScore) {
            return a.computedRiskScore > b.computedRiskScore;
        }
        // Tie-breaker: higher hazard exposure first
        return a.currentHazardExposure > b.currentHazardExposure;
    }
};

// =========================================================================
// Unit IV: Custom Stream Manipulators for tabular alert dispatch formatting
// =========================================================================
namespace CustomManipulators {

// Custom stream manipulator for table border lines
inline std::ostream& alertTableBorder(std::ostream& os) {
    os << "+-----------------+----------------------+----------------------+---------+--------+----------+-----------+\n";
    return os;
}

// Custom stream manipulator for table header
inline std::ostream& alertTableHeader(std::ostream& os) {
    os << alertTableBorder;
    os << "| Route ID        | Source Sector        | Safe Haven Dest      | Dist(km)| Hazard | Cap(Bus) | RiskScore |\n";
    os << alertTableBorder;
    return os;
}

// Helper struct for parameterized row manipulator
struct FormatRouteRow {
    const EvacuationRoute& route;
    FormatRouteRow(const EvacuationRoute& r) : route(r) {}
};

inline std::ostream& operator<<(std::ostream& os, const FormatRouteRow& f) {
    os << "| " << std::left << std::setw(15) << f.route.routeId
       << " | " << std::left << std::setw(20) << f.route.sourceZone
       << " | " << std::left << std::setw(20) << f.route.safeHavenSector
       << " | " << std::right << std::fixed << std::setprecision(1) << std::setw(7) << f.route.roadLengthKm
       << " | " << std::right << std::fixed << std::setprecision(2) << std::setw(6) << f.route.currentHazardExposure
       << " | " << std::right << std::setw(8) << f.route.transitCapacityBuses
       << " | " << std::right << std::fixed << std::setprecision(1) << std::setw(9) << f.route.computedRiskScore
       << " |\n";
    return os;
}

inline FormatRouteRow formatAlertRow(const EvacuationRoute& route) {
    return FormatRouteRow(route);
}

} // namespace CustomManipulators

// =========================================================================
// Unit IV: Routing Engine utilizing STL containers and standard algorithms
// =========================================================================
class EvacuationRoutingManager {
private:
    // STL containers
    std::vector<EvacuationRoute> m_routes;
    std::map<std::string, std::vector<std::string>> m_zoneRouteMap; // Zone -> Route IDs

public:
    EvacuationRoutingManager() {
        seedDefaultRoutes();
    }

    void seedDefaultRoutes() {
        m_routes.clear();
        m_zoneRouteMap.clear();

        addRoute(EvacuationRoute("RT-HIM-01", "Kedarnath Valley", "Guptakashi Staging", 24.5, 0.85, 50, true));
        addRoute(EvacuationRoute("RT-HIM-02", "Chamoli Sector", "Joshimath High Base", 16.0, 0.72, 35, true));
        addRoute(EvacuationRoute("RT-HIM-03", "Rohtang Pass", "Manali Safe Corridor", 31.2, 0.45, 60, true));
        addRoute(EvacuationRoute("RT-HIM-04", "Teesta River Basin", "Kalimpong Relief Camp", 42.0, 0.92, 45, false));
        addRoute(EvacuationRoute("RT-HIM-05", "Joshimath Ridge", "Pipalkoti Logistics Hub", 28.4, 0.68, 40, true));
    }

    void addRoute(const EvacuationRoute& route) {
        m_routes.push_back(route);
        m_zoneRouteMap[route.sourceZone].push_back(route.routeId);
    }

    // Unit IV: STL Algorithm std::sort using custom Functor
    void sortRoutesByEmergencyPriority() {
        std::sort(m_routes.begin(), m_routes.end(), EvacuationPriorityFunctor());
    }

    // Unit IV: STL Algorithm std::for_each with lambda
    void adjustExposureForZone(const std::string& zone, double deltaExposure) {
        std::for_each(m_routes.begin(), m_routes.end(), [&](EvacuationRoute& r) {
            if (r.sourceZone == zone) {
                r.currentHazardExposure = std::min(std::max(r.currentHazardExposure + deltaExposure, 0.0), 1.0);
                r.computeRiskScore();
            }
        });
    }

    // Print table using custom manipulators
    void printTabularDispatch(std::ostream& os) const {
        os << CustomManipulators::alertTableHeader;
        for (const auto& r : m_routes) {
            os << CustomManipulators::formatAlertRow(r);
        }
        os << CustomManipulators::alertTableBorder;
    }

    const std::vector<EvacuationRoute>& getRoutes() const noexcept { return m_routes; }
    const std::map<std::string, std::vector<std::string>>& getZoneRouteMap() const noexcept { return m_zoneRouteMap; }
};

} // namespace Alerts
} // namespace DisasterCore

#endif // ALF_EVACUATION_ROUTING_HPP
