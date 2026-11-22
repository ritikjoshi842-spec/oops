# Autonomous Landslide & Flash-Flood Early Warning Grid (ALF-EWG)
**Paper Code: CIC-211 | Object-Oriented Programming Using C++**  
*Maharaja Agrasen Institute of Technology (MAIT) — Department of Computer Science & Engineering*  
**Option 1 Submission**

---

## 🏔️ Domain & System Overview
In vulnerable mountainous terrains across the Himalayan belt (e.g., Kedarnath, Chamoli, Rohtang Pass, Teesta River Basin, and Joshimath), severe landslides and flash floods cause massive disruptions to human life and critical infrastructure. 

**ALF-EWG** is a high-performance disaster management grid combining:
1. A **pure standard C++ (C++14 compliant)** core engine implementing all core OOP principles, dynamic memory management, diamond inheritance resolution, binary stream transactional persistence, and custom STL algorithms.
2. A **React Command Center UI** visualizing live telemetry, automated siren alerts, memory auditor probes, and evacuation routing.
3. An **Express API Bridge** seamlessly connecting the web dashboard to the compiled native C++ executable (`alf_ewg_engine.exe`).

---

## 📚 Technical Mechanisms Across the CIC-211 Syllabus

### Unit I: Core Foundations & Call Semantics
- **Distinct Namespaces**:
  - `DisasterCore::Sensors`: Telemetry definitions, coordinate transforms, sensor nodes.
  - `DisasterCore::Alerts`: Emergency siren controllers, incident persistence, alert dispatching.
- **Mathematical Risk Algorithms (`inline` functions)**:
  - `computeCriticalShearStress()`: Mohr-Coulomb failure criterion for mountainous slope stability ($\tau_f = c' + \sigma' \tan\phi'$).
  - `computePoreWaterPressure()`: Terzaghi pore pressure calculation ($u = \gamma_w \cdot h_w$).
  - `calculateRunoffVelocity()`: Manning's equation for flash-flood river runoff flow velocity ($v = \frac{1}{n} R^{2/3} S^{1/2}$).
  - `calculateFlashFloodWaveSpeed()`: Shock wave celerity calculation ($c = \sqrt{g \cdot y}$).
- **Function Overloading (`computeHazardIndex()`)**:
  - `computeHazardIndex(rainfallRate, slopeAngle, soilPorePressure)` (Geotechnical parameters).
  - `computeHazardIndex(soilSaturation, vibrationFreq, riverLevel, acousticCount)` (Hydrological & seismic parameters).
  - `computeHazardIndex(const TelemetryMetrics&)` (Composite non-linear synergistic multi-sensor payload).
- **Call by Reference & Return by Reference**:
  - `transformCoordinateWGS84toUTM(const GeoCoordinate& raw, GeoCoordinate& outTransformed)`: High-frequency geodetic conformal mapping returning reference to transformed object.
  - `accumulateTelemetryState(const TelemetryMetrics& sample, TelemetryMetrics& accumulator)`: Modifies and returns reference to state accumulator.
- **Friend Class (`TelemetryAuditor`) & Friend Function**:
  - `TelemetryAuditor` inspects raw private calibration registers (`m_privateCalibrationRegister = 0xDEADBEEF`), raw ADC voltage bias, and buffer pointers without exposing public getters.
  - `friend void auditRawHardwareInternals(const SensorStation&, std::ostream&)`.

---

### Unit II: Encapsulation, Dynamic Memory & Operator Overloading
- **Data Abstraction & Const Correctness**:
  - `SensorStation`, `EarlyWarningSiren`, `TelemetryBuffer` modeled with strict encapsulation and const-correct member functions.
- **Static Members**:
  - `SensorStation::s_activeStationCount` & `SensorStation::getActiveStationCount()`
  - `SensorStation::s_globalEmergencyAlertLevel` & `SensorStation::getGlobalEmergencyAlertLevel()`
- **Dynamic Sensor Memory Buffer (`TelemetryBuffer`)**:
  - Parameterized & dynamic constructor allocating heap array: `m_dataBuffer = new SensorSample[m_capacity];`
  - Deep copy constructor performing element-by-element deep value duplication onto separate heap address.
  - Copy assignment operator (`operator=`) adhering to Rule of Five.
  - Move constructor and move assignment operator (`&&`).
  - Explicit destructor (`delete[] m_dataBuffer;`) eliminating memory leaks.
- **Operator Overloading**:
  - Binary `+`: Aggregates multiple sensor clusters into regional zone vectors (`RegionalZoneCluster = zoneA + zoneB`).
  - Relational `>`: Prioritizes higher hazard score stations (`stationA > stationB`).
  - Stream insertion (`<<`): Formats station state to console/file streams.
  - Stream extraction (`>>`): Deserializes space-separated telemetry configurations.

---

### Unit III: Inheritance, Containership & Polymorphism
- **Diamond Problem Resolution via Virtual Base Classes**:
  ```
                 DisasterTelemetryDevice (Virtual Root Base)
                         /                      \
                        / (virtual)              \ (virtual)
             SolarPoweredNode              WirelessLoRaTransceiver
                        \                        /
                         \                      /
                       AutonomousHybridProbe (Derived Leaf)
  ```
  - Both intermediate classes virtually inherit `DisasterTelemetryDevice`, ensuring only **a single shared root instance** exists inside `AutonomousHybridProbe`.
- **Containership (Aggregation)**:
  - `PowerSubsystem` object is composed inside `BaseSensorNode` (has-a) to clearly distinguish composition from classification (is-a).
- **Runtime Dynamic Method Dispatch**:
  - Abstract base class `BaseSensorNode` contains pure virtual functions:
    - `virtual void readTelemetry() = 0;`
    - `virtual double evaluateThreshold() = 0;`
  - Base pointer vectors (`std::vector<BaseSensorNode*>`) demonstrate dynamic runtime dispatch through the `vtable` and `_vptr` during cyclical sensor sweep cycles.

---

### Unit IV: Streams, Persistence, Exception Safety & STL
- **Custom Exception Hierarchy**:
  - `DisasterCore::Alerts::SensorFailureException`
  - `DisasterCore::Alerts::TelemetryChecksumMismatch`
  - `DisasterCore::Alerts::ThresholdOverflowException`
  - All derive from `std::exception` with detailed, context-rich `what()` diagnostics.
- **Binary Persistence with Transactional `seekp()`/`seekg()` Rollback**:
  - Serializes binary structs (`IncidentRecord`) to `data/incidents.bin` using `std::fstream`.
  - Transaction start: records checkpoint stream offset `std::streampos checkpoint = file.tellp();`
  - In the event of a simulated or real write failure/corruption, `file.seekp(checkpoint)` rolls back the stream write pointer, overwrites corrupted bytes, and preserves file integrity.
- **STL Containers & Standard Algorithms**:
  - Containers: `std::vector<EvacuationRoute>`, `std::map<std::string, std::vector<std::string>>`.
  - Algorithms: `std::sort` for emergency priority ranking, `std::for_each` for batch exposure updates.
  - **Custom Functor**: `struct EvacuationPriorityFunctor` sorts evacuation corridors by hazard risk, road clearance, transit capacity, and urgency.
  - **Custom Stream Manipulators**: `alertTableBorder`, `alertTableHeader`, `formatAlertRow` for formatted tabular console output.

---

## 🚀 Running the Project

### 1. Build and Run C++ Core Engine (CLI Mode)
From the project root:
```powershell
cd cpp_engine
# Recompile with C++14 compliance
C:\MinGW\bin\g++.exe -std=c++14 -O2 -Wall -I./include src/sensor_station.cpp src/main.cpp -o ../bin/alf_ewg_engine.exe

# Execute Academic Verification Suite
cd ..
.\bin\alf_ewg_engine.exe --demo
```

### 2. Run the Full Web Application (React UI + Express API Bridge)
1. Start the backend API:
```powershell
cd server
npm start
```
The server will start on `http://localhost:5000`, serving both the C++ API bridge and the production React dashboard.

2. Open your browser and navigate to:
```
http://localhost:5000
```

---

## 📁 Repository Directory Structure

```
alf-ewg-disaster-grid/
├── bin/
│   └── alf_ewg_engine.exe           # Compiled C++ Core Engine
├── data/
│   ├── incidents.bin                # Binary std::fstream incident log
│   └── academic_test_incidents.bin  # Verification test binary stream
├── cpp_engine/
│   ├── include/
│   │   ├── disaster_core.hpp        # Namespaces, inline math, overloads, references
│   │   ├── exceptions.hpp           # Custom std::exception derived classes
│   │   ├── power_subsystem.hpp      # Containership aggregation class
│   │   ├── telemetry_buffer.hpp     # Dynamic memory buffer (Rule of 5)
│   │   ├── sensor_hierarchy.hpp     # Diamond problem & abstract BaseSensorNode
│   │   ├── sensor_station.hpp       # Station entity, operators, friend auditor
│   │   ├── persistence_engine.hpp   # Binary stream logging & seekp() rollback
│   │   └── evacuation_routing.hpp   # STL containers, sort, Functor, manipulators
│   ├── src/
│   │   ├── sensor_station.cpp       # Static data members definitions
│   │   └── main.cpp                 # Academic test suite & JSON CLI dispatcher
│   └── build.bat                    # Windows batch build script
├── server/
│   ├── index.js                     # Express API bridge executing C++ executable
│   └── package.json
├── ui/                              # React 19 + Vite dashboard
│   ├── src/
│   │   ├── App.jsx                  # Interactive command center UI
│   │   ├── App.css                  # Dark mode styling & radar animations
│   │   └── main.jsx
│   ├── dist/                        # Production build served by Express
│   └── package.json
└── README.md
```
