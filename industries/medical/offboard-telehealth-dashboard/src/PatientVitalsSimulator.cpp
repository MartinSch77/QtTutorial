// SPDX-License-Identifier: MIT
#include "PatientVitalsSimulator.h"

#include <array>
#include <cmath>
#include <numbers>

namespace qttutorial::medical::telehealth {

namespace {

constexpr double kTwoPi = 2.0 * std::numbers::pi;
constexpr double kEpisodePeriodSeconds = 90.0;

const std::array<const char*, 5> kNames = {
    "A. Rossi", "B. Nguyen", "C. Diaz", "D. Kim", "E. Okafor",
};

constexpr double kFeverPeriodSeconds = 150.0;

double episodeSeverity(int patientIndex, double elapsedSeconds)
{
    if (patientIndex != 0) {
        return 0.0;
    }
    const double phase = std::fmod(elapsedSeconds, kEpisodePeriodSeconds);
    if (phase < 40.0 || phase >= 75.0) {
        return 0.0;
    }
    if (phase < 50.0) {
        return (phase - 40.0) / 10.0;
    }
    if (phase < 65.0) {
        return 1.0;
    }
    return 1.0 - (phase - 65.0) / 10.0;
}

// A fever episode, distinct from the desaturation event above: only patient
// index 1 runs it, on a longer, offset cycle, so the two scenarios never
// coincide.
double feverSeverity(int patientIndex, double elapsedSeconds)
{
    if (patientIndex != 1) {
        return 0.0;
    }
    const double phase = std::fmod(elapsedSeconds, kFeverPeriodSeconds);
    if (phase < 100.0 || phase >= 140.0) {
        return 0.0;
    }
    if (phase < 110.0) {
        return (phase - 100.0) / 10.0;
    }
    if (phase < 125.0) {
        return 1.0;
    }
    return 1.0 - (phase - 125.0) / 15.0;
}

} // namespace

PatientVitals PatientVitalsSimulator::sampleAt(int patientIndex, double elapsedSeconds)
{
    const double offset = static_cast<double>(patientIndex) * 17.0;
    const double desatSeverity = episodeSeverity(patientIndex, elapsedSeconds);
    const double feverSev = feverSeverity(patientIndex, elapsedSeconds);

    PatientVitals vitals;
    vitals.id = QStringLiteral("PT-%1").arg(patientIndex + 1, 2, 10, QLatin1Char('0'));
    vitals.name = QString::fromLatin1(kNames[static_cast<std::size_t>(patientIndex) % kNames.size()]);
    vitals.heartRate = 70.0 + 5.0 * std::sin(kTwoPi * (elapsedSeconds + offset) / 12.0) + desatSeverity * 50.0
        + feverSev * 15.0;
    vitals.spo2 = 97.0 + 1.0 * std::sin(kTwoPi * (elapsedSeconds + offset) / 50.0) - desatSeverity * 9.0;
    vitals.systolic = 118.0 + 6.0 * std::sin(kTwoPi * (elapsedSeconds + offset) / 70.0) + desatSeverity * 10.0;
    vitals.diastolic = vitals.systolic * 0.64;
    // Desaturation plausibly correlates with faster, more laboured
    // breathing; fever contributes a smaller respiratory-rate rise too.
    vitals.respirationRate = 16.0 + 1.5 * std::sin(kTwoPi * (elapsedSeconds + offset) / 20.0) + desatSeverity * 10.0
        + feverSev * 4.0;
    // Fever correlates elevated temperature with the heart-rate/respiration
    // rise modelled above; a desaturation event alone does not raise it.
    vitals.temperature = 37.0 + 0.15 * std::sin(kTwoPi * (elapsedSeconds + offset) / 300.0) + feverSev * 1.7;
    return vitals;
}

AlarmSeverity PatientVitalsSimulator::classify(const PatientVitals& vitals)
{
    if (vitals.heartRate >= 125.0 || vitals.spo2 <= 88.0) {
        return AlarmSeverity::Critical;
    }
    if (vitals.heartRate >= 110.0 || vitals.spo2 <= 92.0) {
        return AlarmSeverity::Warning;
    }
    return AlarmSeverity::Normal;
}

} // namespace qttutorial::medical::telehealth
