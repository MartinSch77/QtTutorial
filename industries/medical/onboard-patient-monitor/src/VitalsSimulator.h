// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::medical {

// Deterministic, time-based vitals model: baseline heart rate has plausible
// respiratory sinus arrhythmia (a slow +/- wobble tied to breathing), and once
// per 90 second cycle a short "deterioration episode" ramps heart rate up and
// SpO2 down and back again. This gives AlarmStateMachine something believable
// to react to without relying on random noise, and keeps everything
// reproducible in tests.
class VitalsSimulator {
public:
    static constexpr double kEpisodePeriodSeconds = 90.0;

    [[nodiscard]] static double episodeSeverityAt(double timeSeconds);
    [[nodiscard]] static double heartRateAt(double timeSeconds);
    [[nodiscard]] static double spo2At(double timeSeconds);
    [[nodiscard]] static double systolicAt(double timeSeconds);
    [[nodiscard]] static double diastolicAt(double timeSeconds);
};

} // namespace qttutorial::medical
