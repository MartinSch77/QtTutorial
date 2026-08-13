// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::agriculture {

struct FieldPassState {
    int passNumber = 1;
    double coveragePercent = 0.0;
    double crossTrackErrorCm = 0.0;
    double workingDepthCm = 0.0;
    bool implementEngaged = false;
    double engineLoadPercent = 0.0;
    double fuelLevelPercent = 100.0;
};

// Drives a repeating field pass with no randomness so results are reproducible in
// tests. Coverage progresses deterministically with distance travelled along the
// current pass; cross-track error is a smooth, bounded function of total distance
// travelled (representing ongoing steering-assist correction dynamics, not
// independent per-tick noise); the implement disengages in short turn zones at each
// row end, and engine load/fuel burn are both derived from that same engaged state.
class FieldPassSimulator {
public:
    static constexpr double kPassLengthMeters = 400.0;
    static constexpr double kWorkingSpeedKph = 9.0;
    static constexpr double kTurnSpeedKph = 6.0;
    static constexpr double kTurnZoneFraction = 0.04;
    static constexpr double kWorkingDepthCm = 15.0;
    static constexpr double kFuelPercentPerKmWorking = 0.6;
    static constexpr double kFuelPercentPerKmIdle = 0.15;

    FieldPassSimulator() = default;

    FieldPassState advance(double dtSeconds);

    [[nodiscard]] const FieldPassState& state() const { return m_state; }

    [[nodiscard]] static bool isImplementEngagedAt(double progressFraction);
    [[nodiscard]] static double workingDepthAt(bool engaged);
    [[nodiscard]] static double crossTrackErrorAt(double totalDistanceMeters);
    [[nodiscard]] static double engineLoadAt(bool engaged, double crossTrackErrorCm);
    [[nodiscard]] static double speedKphAt(bool engaged);

private:
    double m_distanceInPassMeters = 0.0;
    double m_totalDistanceMeters = 0.0;
    double m_fuelLevelPercent = 100.0;
    FieldPassState m_state;
};

} // namespace qttutorial::agriculture
