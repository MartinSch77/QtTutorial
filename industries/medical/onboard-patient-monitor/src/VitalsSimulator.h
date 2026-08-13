// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::medical {

// Deterministic, time-based vitals model: baseline heart rate has plausible
// respiratory sinus arrhythmia (a slow +/- wobble tied to breathing).
//
// Two independent, non-overlapping scenarios are layered on top so the alarm
// system and the UI have something clinically believable to react to,
// without relying on random noise and while staying perfectly reproducible
// in tests:
//
//  - A "desaturation event" (episodeSeverityAt), once per 90 second cycle:
//    heart rate ramps up, SpO2 drops, and respiration rate rises too — low
//    SpO2 plausibly correlates with faster, more laboured breathing.
//  - A "fever episode" (feverSeverityAt), on a longer, offset 150 second
//    cycle: body temperature rises, and heart rate and respiration rate rise
//    with it — a mild tachycardia/tachypnoea that plausibly accompanies a
//    fever, distinct from the sharper desaturation event.
class VitalsSimulator {
public:
    static constexpr double kEpisodePeriodSeconds = 90.0;
    static constexpr double kFeverPeriodSeconds = 150.0;

    [[nodiscard]] static double episodeSeverityAt(double timeSeconds);
    [[nodiscard]] static double feverSeverityAt(double timeSeconds);
    [[nodiscard]] static double heartRateAt(double timeSeconds);
    [[nodiscard]] static double spo2At(double timeSeconds);
    [[nodiscard]] static double systolicAt(double timeSeconds);
    [[nodiscard]] static double diastolicAt(double timeSeconds);
    [[nodiscard]] static double respirationRateAt(double timeSeconds);
    [[nodiscard]] static double temperatureAt(double timeSeconds);
};

} // namespace qttutorial::medical
