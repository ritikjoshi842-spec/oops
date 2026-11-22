#ifndef ALF_DISASTER_CORE_HPP
#define ALF_DISASTER_CORE_HPP

#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>

namespace DisasterCore {

// Geocoordinate structure for call-by-reference and return-by-reference transformations
struct GeoCoordinate {
    double latitude;
    double longitude;
    double elevationMeters;
    double utmEasting;
    double utmNorthing;
    int utmZone;

    GeoCoordinate(double lat = 0.0, double lon = 0.0, double elev = 0.0)
        : latitude(lat), longitude(lon), elevationMeters(elev),
          utmEasting(0.0), utmNorthing(0.0), utmZone(44) {}
};

struct TelemetryMetrics {
    double soilMoisturePercentage;
    double seismicVibrationHz;
    double acousticEmissionCPM;    // Counts per minute from tiltmeter
    double riverLevelMeters;
    double rainfallRateMmPerHour;
    double slopeAngleDegrees;
    double calculatedHazardIndex;
    int sampleCount;

    TelemetryMetrics()
        : soilMoisturePercentage(0.0),
          seismicVibrationHz(0.0),
          acousticEmissionCPM(0.0),
          riverLevelMeters(0.0),
          rainfallRateMmPerHour(0.0),
          slopeAngleDegrees(0.0),
          calculatedHazardIndex(0.0),
          sampleCount(0) {}
};

namespace Sensors {

// =========================================================================
// Unit I: Mathematical risk-indexing algorithms using INLINE functions
// =========================================================================

// Mohr-Coulomb failure criterion for mountainous slope stability
inline double computeCriticalShearStress(double soilCohesionKPa, double normalStressKPa, double frictionAngleDeg) {
    const double pi = 3.14159265358979323846;
    double frictionAngleRad = frictionAngleDeg * (pi / 180.0);
    // tau_f = c' + sigma' * tan(phi')
    return soilCohesionKPa + (normalStressKPa * std::tan(frictionAngleRad));
}

// Terzaghi pore water pressure calculation
inline double computePoreWaterPressure(double waterDensityKgM3, double depthMeters, double waterTableElevation) {
    const double g = 9.80665;
    double head = (depthMeters > waterTableElevation) ? (depthMeters - waterTableElevation) : 0.0;
    return (waterDensityKgM3 * g * head) / 1000.0; // in kPa
}

// Manning's equation for flash-flood river runoff flow velocity
inline double calculateRunoffVelocity(double hydraulicRadiusM, double channelSlopeRad, double roughnessCoeff) {
    if (roughnessCoeff <= 0.0) return 0.0;
    return (1.0 / roughnessCoeff) * std::pow(hydraulicRadiusM, 2.0 / 3.0) * std::sqrt(channelSlopeRad);
}

// Celerity of flash flood shock wave
inline double calculateFlashFloodWaveSpeed(double waterDepthM) {
    const double g = 9.80665;
    return std::sqrt(g * waterDepthM);
}

// =========================================================================
// Unit I: Overloaded operational functions computeHazardIndex()
// =========================================================================

// Overload A: Geotechnical landslide hazard index based on rainfall rate, slope angle, and pore pressure
inline double computeHazardIndex(double rainfallRate_mmph, double slopeAngle_deg, double soilPorePressure_kPa) {
    // Normalized risk score: 0.0 (safe) to 1.0 (catastrophic)
    double rainFactor = std::min(rainfallRate_mmph / 120.0, 1.0) * 0.45;
    double slopeFactor = std::sin(slopeAngle_deg * (3.14159265 / 180.0)) * 0.35;
    double pressureFactor = std::min(soilPorePressure_kPa / 50.0, 1.0) * 0.20;
    double total = rainFactor + slopeFactor + pressureFactor;
    return std::min(std::max(total, 0.0), 1.0);
}

// Overload B: Hydrological and seismic hazard index based on soil saturation, vibration frequency, river level, acoustic emission
inline double computeHazardIndex(double soilSaturation_pct, double vibrationFreq_Hz, double riverLevel_m, double acousticEmission_cpm) {
    double satFactor = (soilSaturation_pct / 100.0) * 0.30;
    // Seismic peak resonant frequency for slope shear failure is typically 2.0 - 8.0 Hz
    double vibFactor = (vibrationFreq_Hz > 0.5 ? std::min(vibrationFreq_Hz / 15.0, 1.0) : 0.0) * 0.25;
    double riverFactor = std::min(riverLevel_m / 10.0, 1.0) * 0.30;
    double acousticFactor = std::min(acousticEmission_cpm / 500.0, 1.0) * 0.15;
    double total = satFactor + vibFactor + riverFactor + acousticFactor;
    return std::min(std::max(total, 0.0), 1.0);
}

// Overload C: Composite telemetry overload utilizing TelemetryMetrics
inline double computeHazardIndex(const TelemetryMetrics& telemetry) {
    double hGeotech = computeHazardIndex(telemetry.rainfallRateMmPerHour, telemetry.slopeAngleDegrees, 15.0);
    double hHydro = computeHazardIndex(telemetry.soilMoisturePercentage, telemetry.seismicVibrationHz, 
                                       telemetry.riverLevelMeters, telemetry.acousticEmissionCPM);
    // Synergistic non-linear amplification when both geotechnical and hydrological indices are elevated
    double combined = (0.5 * hGeotech) + (0.5 * hHydro);
    if (hGeotech > 0.6 && hHydro > 0.6) {
        combined *= 1.25; // Non-linear trigger cascade
    }
    return std::min(combined, 1.0);
}

// =========================================================================
// Unit I: Call by reference and Return by reference
// =========================================================================

// High-frequency geocoordinate transformation: takes raw coords by const ref and returns output by ref
inline GeoCoordinate& transformCoordinateWGS84toUTM(const GeoCoordinate& raw, GeoCoordinate& outTransformed) {
    // Himalayan UTM Zone 44N projection approximation
    const double a = 6378137.0; // WGS84 semi-major axis
    const double pi = 3.14159265358979323846;
    double latRad = raw.latitude * (pi / 180.0);
    double lonRad = raw.longitude * (pi / 180.0);
    double centralMeridian = (44 * 6 - 183) * (pi / 180.0); // Central meridian for zone 44

    double k0 = 0.9996;
    double deltaLon = lonRad - centralMeridian;

    outTransformed.latitude = raw.latitude;
    outTransformed.longitude = raw.longitude;
    outTransformed.elevationMeters = raw.elevationMeters;
    outTransformed.utmZone = 44;
    // Approximated conformal mapping for high-frequency embedded telemetry
    outTransformed.utmEasting = 500000.0 + k0 * a * deltaLon * std::cos(latRad);
    outTransformed.utmNorthing = k0 * a * latRad;

    return outTransformed; // Return by reference
}

// High-frequency telemetry state accumulation: modifies and returns reference to accumulator
inline TelemetryMetrics& accumulateTelemetryState(const TelemetryMetrics& sample, TelemetryMetrics& accumulator) {
    accumulator.sampleCount++;
    double n = static_cast<double>(accumulator.sampleCount);
    // Running moving average update
    accumulator.soilMoisturePercentage += (sample.soilMoisturePercentage - accumulator.soilMoisturePercentage) / n;
    accumulator.seismicVibrationHz += (sample.seismicVibrationHz - accumulator.seismicVibrationHz) / n;
    accumulator.acousticEmissionCPM += (sample.acousticEmissionCPM - accumulator.acousticEmissionCPM) / n;
    accumulator.riverLevelMeters += (sample.riverLevelMeters - accumulator.riverLevelMeters) / n;
    accumulator.rainfallRateMmPerHour += (sample.rainfallRateMmPerHour - accumulator.rainfallRateMmPerHour) / n;
    accumulator.slopeAngleDegrees = sample.slopeAngleDegrees; // Constant terrain slope
    accumulator.calculatedHazardIndex = computeHazardIndex(accumulator);

    return accumulator; // Return by reference
}

} // namespace Sensors

namespace Alerts {

enum class AlertLevel {
    GREEN_NOMINAL = 0,
    YELLOW_ADVISORY = 1,
    ORANGE_WARNING = 2,
    RED_EVACUATE = 3
};

inline const char* getAlertLevelString(AlertLevel level) {
    switch (level) {
        case AlertLevel::GREEN_NOMINAL: return "GREEN_NOMINAL";
        case AlertLevel::YELLOW_ADVISORY: return "YELLOW_ADVISORY";
        case AlertLevel::ORANGE_WARNING: return "ORANGE_WARNING";
        case AlertLevel::RED_EVACUATE: return "RED_EVACUATE";
        default: return "UNKNOWN";
    }
}

inline AlertLevel evaluateHazardToAlert(double hazardIndex) {
    if (hazardIndex >= 0.75) return AlertLevel::RED_EVACUATE;
    if (hazardIndex >= 0.50) return AlertLevel::ORANGE_WARNING;
    if (hazardIndex >= 0.25) return AlertLevel::YELLOW_ADVISORY;
    return AlertLevel::GREEN_NOMINAL;
}

} // namespace Alerts
} // namespace DisasterCore

#endif // ALF_DISASTER_CORE_HPP
