// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::agriculture {

// Which implement is mounted, purely for display (icon/label) purposes; the
// physical engagement/depth/load model is identical regardless of kind.
enum class ImplementKind { Planter, Plow, Sprayer };

struct FieldPassState {
    int passNumber = 1;
    double coveragePercent = 0.0;
    double crossTrackErrorCm = 0.0;
    double workingDepthCm = 0.0;
    bool implementEngaged = false;
    double engineLoadPercent = 0.0;
    double fuelLevelPercent = 100.0;
    double yieldRateTonsPerHour = 0.0;
    int rowIndex = 0;
    bool movingForward = true;
    ImplementKind implementKind = ImplementKind::Planter;
};

// Drives a repeating field pass with no randomness so results are reproducible in
// tests. Coverage progresses deterministically with distance travelled along the
// current pass; cross-track error is a smooth, bounded function of total distance
// travelled (representing ongoing steering-assist correction dynamics, not
// independent per-tick noise); the implement disengages in short turn zones at each
// row end, and engine load, fuel burn rate and yield rate are all derived from that
// same engaged/speed state so the readouts stay physically consistent with one
// another rather than varying independently.
class FieldPassSimulator {
public:
    static constexpr double kPassLengthMeters = 400.0;
    static constexpr double kWorkingSpeedKph = 9.0;
    static constexpr double kTurnSpeedKph = 6.0;
    static constexpr double kTurnZoneFraction = 0.04;
    static constexpr double kWorkingDepthCm = 15.0;
    // Fuel burn rate is a continuous function of engine load rather than a fixed
    // working/idle constant, so a harder-working engine (e.g. correcting a larger
    // cross-track error) burns measurably more fuel per km, not just "engaged vs not".
    static constexpr double kFuelBaselinePercentPerKm = 0.05;
    static constexpr double kFuelPercentPerKmPerLoadPoint = 0.008;
    // Yield rate only exists while the implement is engaged, and scales with ground
    // speed the way a real planter/combine's material throughput would.
    static constexpr double kYieldTonsPerHourAtWorkingSpeed = 38.0;
    // Number of parallel rows in the simulated field before the boustrophedon
    // pattern wraps back to row 0, used to lay the field-coverage map out in 2D.
    static constexpr int kFieldRowCount = 10;

    explicit FieldPassSimulator(ImplementKind implementKind = ImplementKind::Planter);

    FieldPassState advance(double dtSeconds);

    [[nodiscard]] const FieldPassState& state() const { return m_state; }

    [[nodiscard]] static bool isImplementEngagedAt(double progressFraction);
    [[nodiscard]] static double workingDepthAt(bool engaged);
    [[nodiscard]] static double crossTrackErrorAt(double totalDistanceMeters);
    [[nodiscard]] static double engineLoadAt(bool engaged, double crossTrackErrorCm);
    [[nodiscard]] static double speedKphAt(bool engaged);
    [[nodiscard]] static double fuelPercentPerKmAt(double engineLoadPercent);
    [[nodiscard]] static double yieldRateAt(bool engaged, double speedKph);
    [[nodiscard]] static int rowIndexForPass(int passNumber);
    [[nodiscard]] static bool isMovingForward(int rowIndex);

private:
    double m_distanceInPassMeters = 0.0;
    double m_totalDistanceMeters = 0.0;
    double m_fuelLevelPercent = 100.0;
    FieldPassState m_state;
};

} // namespace qttutorial::agriculture
