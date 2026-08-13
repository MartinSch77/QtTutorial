// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::medical {

enum class AlarmLevel { Normal, Warning, Critical };

// A small hysteresis state machine: the thresholds required to *enter* a more
// severe level are looser than the thresholds required to *leave* it, so a
// heart rate or SpO2 value that sits right on a boundary does not make the
// alarm banner flicker between levels tick to tick.
class AlarmStateMachine {
public:
    static constexpr double kWarningEnterHeartRate = 110.0;
    static constexpr double kWarningExitHeartRate = 100.0;
    static constexpr double kCriticalEnterHeartRate = 125.0;
    static constexpr double kCriticalExitHeartRate = 115.0;
    static constexpr double kWarningEnterSpo2 = 92.0;
    static constexpr double kWarningExitSpo2 = 95.0;
    static constexpr double kCriticalEnterSpo2 = 88.0;
    static constexpr double kCriticalExitSpo2 = 91.0;

    AlarmLevel update(double heartRateBpm, double spo2Percent);

    [[nodiscard]] AlarmLevel level() const { return m_level; }

private:
    AlarmLevel m_level = AlarmLevel::Normal;
};

} // namespace qttutorial::medical
