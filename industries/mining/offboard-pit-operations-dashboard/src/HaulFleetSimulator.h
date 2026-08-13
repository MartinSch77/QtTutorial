// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::mining::pit {

struct TruckSample {
    QString id;
    QString location;
    QString stateLabel;
    int stateIndex = 0;
    double payloadTonnes = 0.0;
    double cumulativeTonnesHauled = 0.0;
    double speedKph = 0.0;
    double fuelLtrPerHour = 0.0;
    // Normalised (0..1) top-down pit-plan position, for rendering on a simple
    // pit-map view. Not a real survey coordinate system - just enough to
    // place a truck glyph somewhere plausible for its current haul phase.
    double positionX = 0.5;
    double positionY = 0.5;
    bool overloaded = false;
};

// Stands in for a pit-wide telemetry-ingestion service: given a truck index and
// an elapsed time, produces a deterministic, physically plausible sample. Each
// truck is phase-shifted through the same four-stage haul cycle (load, haul,
// dump, return) used by the onboard console, so the fleet does not move in
// lockstep. Cumulative tonnes hauled is a pure function of how many full haul
// cycles a truck has completed, since a dump only completes once a full cycle
// has elapsed. Speed and fuel burn are derived from the same payload/phase
// signal as the onboard console (heavier payload and higher speed both drive
// up fuel burn), so the fleet view stays domain-consistent with the onboard
// telemetry model rather than introducing independent noise.
class HaulFleetSimulator {
public:
    static constexpr double kLoadingSeconds = 30.0;
    static constexpr double kHaulingSeconds = 90.0;
    static constexpr double kDumpingSeconds = 15.0;
    static constexpr double kReturningSeconds = 60.0;
    static constexpr double kCyclePeriodSeconds =
        kLoadingSeconds + kHaulingSeconds + kDumpingSeconds + kReturningSeconds;
    static constexpr double kRatedCapacityTonnes = 300.0;
    static constexpr double kPeakLoadingOverfillTonnes = 315.0;

    [[nodiscard]] static TruckSample sampleAt(int truckIndex, double elapsedSeconds);

    [[nodiscard]] static double speedKphAt(double phaseSeconds);
    [[nodiscard]] static double fuelLtrPerHourAt(double payloadTonnes, double speedKph);
    [[nodiscard]] static bool isOverloaded(double payloadTonnes);

private:
    [[nodiscard]] static double phaseOffsetFor(int truckIndex);
    [[nodiscard]] static int stateIndexAt(double phaseSeconds);
    [[nodiscard]] static QString stateLabelFor(int stateIndex);
    [[nodiscard]] static double payloadAt(double phaseSeconds);
    [[nodiscard]] static QString locationFor(int truckIndex, int stateIndex);
    static void positionFor(int truckIndex, int stateIndex, double phaseSeconds, double& outX, double& outY);
};

} // namespace qttutorial::mining::pit
