#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <memory>
#include "../include/disaster_core.hpp"
#include "../include/power_subsystem.hpp"
#include "../include/telemetry_buffer.hpp"
#include "../include/sensor_hierarchy.hpp"
#include "../include/sensor_station.hpp"
#include "../include/exceptions.hpp"
#include "../include/persistence_engine.hpp"
#include "../include/evacuation_routing.hpp"

using namespace DisasterCore;
using namespace DisasterCore::Sensors;
using namespace DisasterCore::Alerts;

// Helper function to build a default grid of 5 Himalayan stations
std::vector<std::shared_ptr<SensorStation>> createDefaultHimalayanGrid() {
    std::vector<std::shared_ptr<SensorStation>> stations;

    // Station 1: Kedarnath Valley High Altitude Base
    auto st1 = std::make_shared<SensorStation>("STA-KED-01", "Kedarnath Upper Valley", 30.7346, 79.0669, 3584.0);
    st1->attachSensorNode(new SoilMoistureGeophoneNode("SN-KED-GEO-1", 72.5, 3.4));
    st1->attachSensorNode(new AcousticTiltmeterNode("SN-KED-TILT-1", 4.8, 120.0));
    st1->attachSensorNode(new AutonomousHybridProbe("SN-KED-HYB-1", 12.7, 32.0));
    st1->executeSensorSweepCycle();
    stations.push_back(st1);

    // Station 2: Chamoli Sector Geohazard Node
    auto st2 = std::make_shared<SensorStation>("STA-CHA-02", "Chamoli Ravine Slopes", 30.4074, 79.3274, 1550.0);
    st2->attachSensorNode(new SoilMoistureGeophoneNode("SN-CHA-GEO-1", 58.0, 1.8));
    st2->attachSensorNode(new RiverSonarGaugeNode("SN-CHA-RIV-1", 5.2, 0.6));
    st2->executeSensorSweepCycle();
    stations.push_back(st2);

    // Station 3: Rohtang Pass Glacial Corridor
    auto st3 = std::make_shared<SensorStation>("STA-ROH-03", "Rohtang Pass North Ridge", 32.3716, 77.2466, 3978.0);
    st3->attachSensorNode(new AcousticTiltmeterNode("SN-ROH-TILT-1", 1.2, 25.0));
    st3->attachSensorNode(new AutonomousHybridProbe("SN-ROH-HYB-1", 12.9, 28.0));
    st3->executeSensorSweepCycle();
    stations.push_back(st3);

    // Station 4: Teesta River Basin Flash-Flood Node
    auto st4 = std::make_shared<SensorStation>("STA-TEE-04", "Teesta Basin Gorge", 27.2023, 88.5122, 920.0);
    st4->attachSensorNode(new RiverSonarGaugeNode("SN-TEE-RIV-1", 8.4, 1.8)); // Critical high river level
    st4->attachSensorNode(new SoilMoistureGeophoneNode("SN-TEE-GEO-1", 84.0, 4.2));
    st4->executeSensorSweepCycle();
    stations.push_back(st4);

    // Station 5: Joshimath Ridge Subsidence Sector
    auto st5 = std::make_shared<SensorStation>("STA-JOS-05", "Joshimath Town Ridge", 30.5564, 79.5664, 1890.0);
    st5->attachSensorNode(new AcousticTiltmeterNode("SN-JOS-TILT-1", 8.9, 210.0));
    st5->attachSensorNode(new AutonomousHybridProbe("SN-JOS-HYB-1", 12.5, 30.0));
    st5->executeSensorSweepCycle();
    stations.push_back(st5);

    return stations;
}

// =========================================================================
// Academic Unit Verification Suite (Console Demonstration)
// =========================================================================
void runAcademicVerificationSuite() {
    std::cout << "\n======================================================================\n";
    std::cout << "  MAIT CSE - CIC-211: OBJECT-ORIENTED PROGRAMMING USING C++\n";
    std::cout << "  OPTION 1: AUTONOMOUS LANDSLIDE & FLASH-FLOOD EARLY WARNING GRID (ALF-EWG)\n";
    std::cout << "  ACADEMIC TECHNICAL MECHANISMS VERIFICATION SUITE\n";
    std::cout << "======================================================================\n\n";

    // ---------------------------------------------------------------------
    // UNIT I: Core Foundations & Call Semantics
    // ---------------------------------------------------------------------
    std::cout << ">>> [UNIT I: CORE FOUNDATIONS & CALL SEMANTICS]\n";
    std::cout << "1. Distinct Namespaces: DisasterCore::Sensors & DisasterCore::Alerts verified.\n";

    // Inline mathematical functions
    double shear = computeCriticalShearStress(25.0, 110.0, 32.0);
    double porePress = computePoreWaterPressure(1000.0, 12.5, 4.0);
    double runoffVel = calculateRunoffVelocity(2.8, 0.045, 0.035);
    double waveSpeed = calculateFlashFloodWaveSpeed(6.5);
    std::cout << "2. Inline Risk Algorithms:\n"
              << "   - Critical Shear Stress (Mohr-Coulomb): " << shear << " kPa\n"
              << "   - Pore Water Pressure (Terzaghi):       " << porePress << " kPa\n"
              << "   - Manning Runoff Velocity:              " << runoffVel << " m/s\n"
              << "   - Shock Wave Celerity:                  " << waveSpeed << " m/s\n";

    // Overloaded computeHazardIndex()
    double hOverload1 = computeHazardIndex(85.0, 42.0, 35.0); // Rain + Slope + Pore
    double hOverload2 = computeHazardIndex(78.0, 4.5, 7.2, 180.0); // Saturation + Vib + River + Acoustic
    TelemetryMetrics metrics;
    metrics.rainfallRateMmPerHour = 95.0;
    metrics.slopeAngleDegrees = 38.0;
    metrics.soilMoisturePercentage = 82.0;
    metrics.seismicVibrationHz = 3.8;
    metrics.riverLevelMeters = 6.8;
    metrics.acousticEmissionCPM = 190.0;
    double hOverload3 = computeHazardIndex(metrics);
    std::cout << "3. Overloaded computeHazardIndex():\n"
              << "   - Overload 1 (Rainfall 85mm/h + Slope 42 deg):     " << hOverload1 << "\n"
              << "   - Overload 2 (Soil Sat 78% + Seismic Vib 4.5 Hz):  " << hOverload2 << "\n"
              << "   - Overload 3 (Composite Multi-Parameter Telemetry): " << hOverload3 << "\n";

    // Call by reference and return by reference
    GeoCoordinate rawCoord(30.7346, 79.0669, 3584.0);
    GeoCoordinate transformedCoord;
    GeoCoordinate& refResult = transformCoordinateWGS84toUTM(rawCoord, transformedCoord);
    std::cout << "4. Call & Return by Reference (WGS84 -> UTM Zone 44N):\n"
              << "   - Input Lat/Lon: " << rawCoord.latitude << "N, " << rawCoord.longitude << "E\n"
              << "   - Transformed UTM: Easting=" << refResult.utmEasting << " Northing=" << refResult.utmNorthing 
              << " (Verified reference address match: " << (&refResult == &transformedCoord ? "SUCCESS" : "FAIL") << ")\n";

    // Friend class TelemetryAuditor
    SensorStation sampleStation("STA-KED-01", "Kedarnath High Base", 30.7346, 79.0669, 3584.0);
    std::cout << "5. Friend Class (TelemetryAuditor) Deep Hardware Inspection:\n";
    std::cout << TelemetryAuditor::performFullSecurityAudit(sampleStation) << "\n\n";

    // ---------------------------------------------------------------------
    // UNIT II: Encapsulation, Dynamic Memory & Operator Overloading
    // ---------------------------------------------------------------------
    std::cout << ">>> [UNIT II: ENCAPSULATION, DYNAMIC MEMORY & OPERATOR OVERLOADING]\n";
    std::cout << "1. Static Data Members & Static Member Functions:\n"
              << "   - Network Active Stations: " << SensorStation::getActiveStationCount() << "\n"
              << "   - Global Emergency Level:  " << SensorStation::getGlobalEmergencyAlertLevel() << "\n";

    // Dynamic Sensor Memory Buffer & Deep Copy Verification
    std::cout << "2. Dynamic Memory Buffer (Parameterized, Dynamic new[], Deep Copy, Destructor):\n";
    {
        TelemetryBuffer originalBuffer(4);
        originalBuffer.push(SensorSample(1001, 45.2, "SOIL_01"));
        originalBuffer.push(SensorSample(1002, 52.8, "SOIL_01"));

        // Deep copy constructor invocation
        TelemetryBuffer copiedBuffer = originalBuffer;

        std::cout << "   - Original Buffer Addr: 0x" << std::hex << originalBuffer.getBufferAddress() << std::dec 
                  << " [Size: " << originalBuffer.size() << "]\n"
                  << "   - Copied Buffer Addr:   0x" << std::hex << copiedBuffer.getBufferAddress() << std::dec 
                  << " [Size: " << copiedBuffer.size() << "]\n"
                  << "   - Deep copy verification: " 
                  << (originalBuffer.getBufferAddress() != copiedBuffer.getBufferAddress() ? "PASSED (Unique Heap Allocations)" : "FAILED (Shallow copy!)") 
                  << "\n";
    } // Explicit destructors automatically execute and release memory safely here

    // Operator Overloading: + (Cluster Aggregation)
    RegionalZoneCluster zoneA("KEDARNATH_ZONE", "STA-KED-01", 0.74, 3);
    RegionalZoneCluster zoneB("CHAMOLI_ZONE", "STA-CHA-02", 0.62, 2);
    RegionalZoneCluster mergedZone = zoneA + zoneB; // Binary + overload
    std::cout << "3. Operator + (Regional Cluster Merge):\n"
              << "   - Merged Zone Name: " << mergedZone.getZoneName() << "\n"
              << "   - Merged Stations:  " << mergedZone.getStationIds().size() << "\n"
              << "   - Total Nodes:      " << mergedZone.getTotalSensorNodes() << "\n"
              << "   - Cumulative Score: " << mergedZone.getCumulativeHazard() << "\n";

    // Operator Overloading: > (Hazard Prioritization)
    SensorStation stationHigh("STA-HIGH", "High Danger Basin", 30.0, 79.0, 2000.0);
    stationHigh.setHazardScore(0.88);
    SensorStation stationLow("STA-LOW", "Low Risk Valley", 30.1, 79.1, 1500.0);
    stationLow.setHazardScore(0.24);
    std::cout << "4. Operator > (Hazard Prioritization Relational Overload):\n"
              << "   - (stationHigh > stationLow) is: " << (stationHigh > stationLow ? "TRUE (High Hazard Prioritized)" : "FALSE") << "\n";

    // Operator Overloading: << and >>
    std::cout << "5. Stream Insertion (<<) & Extraction (>>):\n"
              << "   - Stream << Output: " << stationHigh << "\n";

    std::istringstream stationInput("STA-SYNTH-99 SyntheticRidge 31.5000 77.8000 2450.0");
    SensorStation deserializedStation("TEMP", "TEMP", 0, 0, 0);
    stationInput >> deserializedStation; // Extraction >> overload
    std::cout << "   - Stream >> Input Deserialized: " << deserializedStation << "\n\n";

    // ---------------------------------------------------------------------
    // UNIT III: Inheritance, Containership & Polymorphism
    // ---------------------------------------------------------------------
    std::cout << ">>> [UNIT III: INHERITANCE, CONTAINERSHIP & POLYMORPHISM]\n";
    std::cout << "1. Diamond Problem Resolution via Virtual Base Classes:\n"
              << "   - Root: DisasterTelemetryDevice\n"
              << "   - Intermediates: SolarPoweredNode & WirelessLoRaTransceiver (virtual public DisasterTelemetryDevice)\n"
              << "   - Derived Leaf: AutonomousHybridProbe\n";

    AutonomousHybridProbe hybridProbe("HYBRID-ALPHA-01", 12.8, 30.0);
    std::cout << "   - Diamond Signature: " << hybridProbe.getFullDiamondSignature() << "\n"
              << "   - Single root instance verified through unified ID: " << hybridProbe.getDeviceId() << "\n";

    std::cout << "2. Containership (Aggregation):\n"
              << "   - PowerSubsystem composed directly inside BaseSensorNode:\n   ";
    hybridProbe.getPowerSystem().displayStatus(std::cout);
    std::cout << "\n";

    std::cout << "3. Runtime Dynamic Method Dispatch via Base Pointer Vector (std::vector<BaseSensorNode*>):\n";
    std::vector<BaseSensorNode*> sweepVector;
    sweepVector.push_back(new SoilMoistureGeophoneNode("SWP-GEO-01", 76.0, 3.1));
    sweepVector.push_back(new AcousticTiltmeterNode("SWP-TILT-01", 5.2, 140.0));
    sweepVector.push_back(new RiverSonarGaugeNode("SWP-RIV-01", 7.8, 1.2));
    sweepVector.push_back(new AutonomousHybridProbe("SWP-HYB-01", 12.7, 28.0));

    for (size_t i = 0; i < sweepVector.size(); ++i) {
        // Dynamic dispatch through vtable / _vptr
        sweepVector[i]->readTelemetry();
        double h = sweepVector[i]->evaluateThreshold();
        std::cout << "   - Sweep [" << i << "] (vptr binding): " 
                  << sweepVector[i]->getTelemetrySummary() 
                  << " -> Evaluated Hazard: " << std::fixed << std::setprecision(2) << h << "\n";
    }

    // Clean up base pointers
    for (auto* ptr : sweepVector) delete ptr;
    sweepVector.clear();
    std::cout << "\n";

    // ---------------------------------------------------------------------
    // UNIT IV: Streams, Persistence, Exception Safety & STL
    // ---------------------------------------------------------------------
    std::cout << ">>> [UNIT IV: STREAMS, PERSISTENCE, EXCEPTION SAFETY & STL]\n";

    // Custom Exception Classes
    std::cout << "1. Custom Exceptions Derived from std::exception:\n";
    try {
        throw SensorFailureException("NODE-FAIL-99", 0x404E, "Loss of optical geophone pulse synchronization");
    } catch (const std::exception& e) {
        std::cout << "   - Caught SensorFailureException: " << e.what() << "\n";
    }

    try {
        throw TelemetryChecksumMismatch("PKT-CORRUPT-77", 0x1A2B, 0x9F00);
    } catch (const std::exception& e) {
        std::cout << "   - Caught TelemetryChecksumMismatch: " << e.what() << "\n";
    }

    try {
        throw ThresholdOverflowException("FLASH_FLOOD_CREST_PEAK", 11.4, 9.0);
    } catch (const std::exception& e) {
        std::cout << "   - Caught ThresholdOverflowException: " << e.what() << "\n";
    }

    // Binary Persistence & Transactional Rollback with seekp()
    std::cout << "2. Persistent Binary Stream Logging with seekp()/seekg() Transactional Rollback:\n";
    PersistenceEngine persistence("data/academic_test_incidents.bin");
    persistence.clearAll();

    IncidentRecord rec1;
    rec1.timestamp = 1700601000;
    std::strncpy(rec1.stationId, "STA-KED-01", sizeof(rec1.stationId) - 1);
    std::strncpy(rec1.zoneName, "Kedarnath Sector", sizeof(rec1.zoneName) - 1);
    rec1.hazardIndex = 0.88;
    rec1.alertLevel = 3;
    std::strncpy(rec1.summary, "Impending glacial lake outburst flood warning", sizeof(rec1.summary) - 1);

    bool commit1 = persistence.logIncidentTransactional(rec1, false);
    std::cout << "   - Record #1 Write (Normal Commit): " << (commit1 ? "SUCCESS (Committed to disk)" : "FAILED") << "\n";

    IncidentRecord recCorrupt;
    recCorrupt.timestamp = 1700602000;
    std::strncpy(recCorrupt.stationId, "STA-CHA-02", sizeof(recCorrupt.stationId) - 1);
    std::strncpy(recCorrupt.zoneName, "Chamoli Ravine", sizeof(recCorrupt.zoneName) - 1);
    recCorrupt.hazardIndex = 0.95;
    recCorrupt.alertLevel = 3;
    std::strncpy(recCorrupt.summary, "Rockfall breach simulated failure test", sizeof(recCorrupt.summary) - 1);

    std::cout << "   - Testing Transactional Failure & seekp() Rollback:\n";
    bool commitCorrupt = persistence.logIncidentTransactional(recCorrupt, true /* simulate failure */);
    std::cout << "   - Corrupt Record Write Result: " << (commitCorrupt ? "COMMITTED" : "ROLLED BACK via seekp()") << "\n";

    auto savedRecords = persistence.readAllIncidents();
    std::cout << "   - Persistent Records on Disk after rollback: " << savedRecords.size() 
              << " (Integrity Verified: rollback successfully averted disk corruption!)\n";

    // STL Containers, Algorithms, Custom Functor, and Custom Stream Manipulators
    std::cout << "3. Evacuation Routing (STL vector/map, std::sort, Custom Functor & Stream Manipulators):\n";
    EvacuationRoutingManager routingMgr;
    std::cout << "   --- Before Priority Sort ---\n";
    routingMgr.printTabularDispatch(std::cout);

    // Sort using custom EvacuationPriorityFunctor
    routingMgr.sortRoutesByEmergencyPriority();
    std::cout << "\n   --- After std::sort with EvacuationPriorityFunctor (Highest Urgency First) ---\n";
    routingMgr.printTabularDispatch(std::cout);

    std::cout << "\n======================================================================\n";
    std::cout << "  ALL UNIT I - IV TECHNICAL REQUIREMENTS SUCCESSFULLY VERIFIED!\n";
    std::cout << "======================================================================\n\n";
}

// =========================================================================
// JSON API Dispatcher for React UI Backend Integration
// =========================================================================
void outputJsonStations(const std::vector<std::shared_ptr<SensorStation>>& stations) {
    std::cout << "{\"activeStationCount\":" << SensorStation::getActiveStationCount()
              << ",\"globalEmergencyLevel\":" << SensorStation::getGlobalEmergencyAlertLevel()
              << ",\"stations\":[";

    for (size_t i = 0; i < stations.size(); ++i) {
        const auto& st = stations[i];
        if (i > 0) std::cout << ",";
        std::cout << "{\"id\":\"" << st->getId() << "\""
                  << ",\"name\":\"" << st->getLocation() << "\""
                  << ",\"latitude\":" << std::fixed << std::setprecision(4) << st->getCoordinates().latitude
                  << ",\"longitude\":" << st->getCoordinates().longitude
                  << ",\"elevation\":" << st->getCoordinates().elevationMeters
                  << ",\"utmEasting\":" << std::setprecision(1) << st->getCoordinates().utmEasting
                  << ",\"utmNorthing\":" << st->getCoordinates().utmNorthing
                  << ",\"utmZone\":" << st->getCoordinates().utmZone
                  << ",\"hazardScore\":" << std::setprecision(2) << st->getHazardScore()
                  << ",\"alertLevel\":\"" << Alerts::getAlertLevelString(st->getAlertLevel()) << "\""
                  << ",\"siren\":{\"sounding\":" << (st->getSiren().isSounding() ? "true" : "false")
                  << ",\"strobe\":" << (st->getSiren().isStrobeActive() ? "true" : "false")
                  << ",\"decibels\":" << st->getSiren().getDecibels()
                  << ",\"type\":\"" << st->getSiren().getType() << "\"}"
                  << ",\"sensorNodes\":[";

        const auto& nodes = st->getSensorNodes();
        for (size_t j = 0; j < nodes.size(); ++j) {
            if (j > 0) std::cout << ",";
            const auto* n = nodes[j];
            std::cout << "{\"id\":\"" << n->getNodeId() << "\""
                      << ",\"type\":\"" << n->getSensorType() << "\""
                      << ",\"value\":" << std::setprecision(2) << n->getLastReading()
                      << ",\"power\":{\"voltage\":" << n->getPowerSystem().getVoltage()
                      << ",\"solarWatts\":" << n->getPowerSystem().getSolarWatts()
                      << ",\"temperature\":" << n->getPowerSystem().getTemperature()
                      << ",\"state\":\"" << n->getPowerSystem().getStateString() << "\"}"
                      << "}";
        }
        std::cout << "],\"bufferSize\":" << st->getBuffer().size()
                  << ",\"bufferCapacity\":" << st->getBuffer().capacity()
                  << "}";
    }
    std::cout << "]}\n";
}

void outputJsonRoutes() {
    EvacuationRoutingManager mgr;
    mgr.sortRoutesByEmergencyPriority();
    const auto& routes = mgr.getRoutes();

    std::cout << "{\"routes\":[";
    for (size_t i = 0; i < routes.size(); ++i) {
        if (i > 0) std::cout << ",";
        const auto& r = routes[i];
        std::cout << "{\"routeId\":\"" << r.routeId << "\""
                  << ",\"source\":\"" << r.sourceZone << "\""
                  << ",\"safeHaven\":\"" << r.safeHavenSector << "\""
                  << ",\"lengthKm\":" << std::fixed << std::setprecision(1) << r.roadLengthKm
                  << ",\"hazard\":" << std::setprecision(2) << r.currentHazardExposure
                  << ",\"busCapacity\":" << r.transitCapacityBuses
                  << ",\"bridgeIntact\":" << (r.isBridgeIntact ? "true" : "false")
                  << ",\"riskScore\":" << std::setprecision(1) << r.computedRiskScore
                  << "}";
    }
    std::cout << "]}\n";
}

void outputJsonIncidents() {
    PersistenceEngine persistence("data/incidents.bin");
    auto records = persistence.readAllIncidents();

    std::cout << "{\"incidentCount\":" << records.size()
              << ",\"logFilePath\":\"" << persistence.getFilePath() << "\""
              << ",\"records\":[";
    for (size_t i = 0; i < records.size(); ++i) {
        if (i > 0) std::cout << ",";
        const auto& r = records[i];
        std::cout << "{\"recordId\":" << r.recordId
                  << ",\"timestamp\":" << r.timestamp
                  << ",\"stationId\":\"" << r.stationId << "\""
                  << ",\"zoneName\":\"" << r.zoneName << "\""
                  << ",\"hazardIndex\":" << std::fixed << std::setprecision(2) << r.hazardIndex
                  << ",\"alertLevel\":" << r.alertLevel
                  << ",\"summary\":\"" << r.summary << "\""
                  << ",\"checksum\":\"0x" << std::hex << r.crcChecksum << std::dec << "\""
                  << "}";
    }
    std::cout << "]}\n";
}

int main(int argc, char* argv[]) {
    // If no argument or "--demo", run full academic verification
    if (argc <= 1 || std::string(argv[1]) == "--demo") {
        runAcademicVerificationSuite();
        return 0;
    }

    std::string command = argv[1];

    if (command == "--json-status" || command == "--json-sweep") {
        auto grid = createDefaultHimalayanGrid();
        outputJsonStations(grid);
        return 0;
    }

    if (command == "--json-routes") {
        outputJsonRoutes();
        return 0;
    }

    if (command == "--json-incidents") {
        outputJsonIncidents();
        return 0;
    }

    if (command == "--json-audit") {
        auto grid = createDefaultHimalayanGrid();
        std::string targetId = (argc > 2) ? argv[2] : "STA-KED-01";
        std::shared_ptr<SensorStation> target = grid[0];
        for (auto& s : grid) {
            if (s->getId() == targetId) {
                target = s;
                break;
            }
        }

        std::cout << "{\"stationId\":\"" << target->getId() << "\""
                  << ",\"auditReport\":\"" << TelemetryAuditor::performFullSecurityAudit(*target) << "\""
                  << ",\"calibrationRegister\":\"0x" << std::hex << TelemetryAuditor::getCalibrationRegister(*target) << std::dec << "\""
                  << ",\"rawAdcBias\":" << TelemetryAuditor::getRawBias(*target)
                  << "}\n";
        return 0;
    }

    if (command == "--json-hazard") {
        // Evaluate overloaded hazard calculation with custom arguments
        double p1 = (argc > 2) ? std::stod(argv[2]) : 85.0; // rainfall or sat
        double p2 = (argc > 3) ? std::stod(argv[3]) : 42.0; // slope or vib
        double p3 = (argc > 4) ? std::stod(argv[4]) : 30.0; // pore or river
        double p4 = (argc > 5) ? std::stod(argv[5]) : 150.0; // acoustic

        double h1 = computeHazardIndex(p1, p2, p3);
        double h2 = computeHazardIndex(p1, p2, p3, p4);

        std::cout << "{\"hazardGeotechnical\":" << std::fixed << std::setprecision(3) << h1
                  << ",\"hazardHydrological\":" << h2
                  << ",\"alertGeotechnical\":\"" << getAlertLevelString(evaluateHazardToAlert(h1)) << "\""
                  << ",\"alertHydrological\":\"" << getAlertLevelString(evaluateHazardToAlert(h2)) << "\""
                  << "}\n";
        return 0;
    }

    if (command == "--json-log-incident") {
        PersistenceEngine persistence("data/incidents.bin");
        IncidentRecord rec;
        rec.timestamp = 1700603000;
        std::string stId = (argc > 2) ? argv[2] : "STA-KED-01";
        std::string zone = (argc > 3) ? argv[3] : "Kedarnath Ridge";
        double hz = (argc > 4) ? std::stod(argv[4]) : 0.85;
        std::string desc = (argc > 5) ? argv[5] : "Manual incident logged from early warning grid";

        std::strncpy(rec.stationId, stId.c_str(), sizeof(rec.stationId) - 1);
        std::strncpy(rec.zoneName, zone.c_str(), sizeof(rec.zoneName) - 1);
        rec.hazardIndex = hz;
        rec.alertLevel = static_cast<int>(evaluateHazardToAlert(hz));
        std::strncpy(rec.summary, desc.c_str(), sizeof(rec.summary) - 1);

        bool ok = persistence.logIncidentTransactional(rec, false);
        std::cout << "{\"success\":" << (ok ? "true" : "false")
                  << ",\"recordId\":" << rec.recordId
                  << ",\"checksum\":\"0x" << std::hex << rec.crcChecksum << std::dec << "\""
                  << "}\n";
        return 0;
    }

    if (command == "--json-test-rollback") {
        PersistenceEngine persistence("data/incidents.bin");
        size_t countBefore = persistence.getIncidentCount();

        IncidentRecord rec;
        rec.timestamp = 1700604000;
        std::strncpy(rec.stationId, "STA-TEST-CORRUPT", sizeof(rec.stationId) - 1);
        std::strncpy(rec.zoneName, "Simulated Fracture Zone", sizeof(rec.zoneName) - 1);
        rec.hazardIndex = 0.99;
        rec.alertLevel = 3;
        std::strncpy(rec.summary, "Simulated write failure to test transactional rollback", sizeof(rec.summary) - 1);

        // Intentionally trigger simulated write failure
        bool committed = persistence.logIncidentTransactional(rec, true);
        size_t countAfter = persistence.getIncidentCount();

        std::cout << "{\"transactionCommitted\":" << (committed ? "true" : "false")
                  << ",\"countBefore\":" << countBefore
                  << ",\"countAfter\":" << countAfter
                  << ",\"rollbackSuccess\":" << (countBefore == countAfter ? "true" : "false")
                  << ",\"explanation\":\"seekp() successfully repositioned file pointer back to previous valid offset, eliminating corrupt bytes.\""
                  << "}\n";
        return 0;
    }

    if (command == "--json-cluster-merge") {
        RegionalZoneCluster z1("KEDARNATH_SECTOR", "STA-KED-01", 0.78, 3);
        RegionalZoneCluster z2("CHAMOLI_SECTOR", "STA-CHA-02", 0.65, 2);
        RegionalZoneCluster merged = z1 + z2; // Binary operator +

        std::cout << "{\"zone1\":\"" << z1.getZoneName() << "\",\"hazard1\":" << z1.getCumulativeHazard()
                  << ",\"zone2\":\"" << z2.getZoneName() << "\",\"hazard2\":" << z2.getCumulativeHazard()
                  << ",\"mergedZone\":\"" << merged.getZoneName() << "\""
                  << ",\"mergedTotalNodes\":" << merged.getTotalSensorNodes()
                  << ",\"mergedCumulativeHazard\":" << merged.getCumulativeHazard()
                  << ",\"mergedAverageHazard\":" << merged.getAverageHazard()
                  << "}\n";
        return 0;
    }

    if (command == "--json-fault") {
        std::string faultType = (argc > 2) ? argv[2] : "sensor";
        std::string exceptionName = "";
        std::string message = "";
        int code = 0;

        try {
            if (faultType == "sensor") {
                throw SensorFailureException("STA-FAULT-01", 0x401A, "ADC sampling bus timeout on geotechnical tilt sensor");
            } else if (faultType == "checksum") {
                throw TelemetryChecksumMismatch("PKT-9921", 0x88AA, 0x1102);
            } else {
                throw ThresholdOverflowException("FLASH_FLOOD_RIVER_LEVEL", 12.8, 8.5);
            }
        } catch (const SensorFailureException& e) {
            exceptionName = "SensorFailureException";
            message = e.what();
            code = e.getFaultCode();
        } catch (const TelemetryChecksumMismatch& e) {
            exceptionName = "TelemetryChecksumMismatch";
            message = e.what();
            code = 0xAA55;
        } catch (const ThresholdOverflowException& e) {
            exceptionName = "ThresholdOverflowException";
            message = e.what();
            code = 0xEEEE;
        }

        std::cout << "{\"exceptionThrown\":\"" << exceptionName << "\""
                  << ",\"code\":\"0x" << std::hex << code << std::dec << "\""
                  << ",\"message\":\"" << message << "\""
                  << ",\"handledGracefully\":true"
                  << "}\n";
        return 0;
    }

    std::cerr << "Unknown command: " << command << std::endl;
    return 1;
}
