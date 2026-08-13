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

} // namespace

PatientVitals PatientVitalsSimulator::sampleAt(int patientIndex, double elapsedSeconds)
{
    const double offset = static_cast<double>(patientIndex) * 17.0;
    const double severity = episodeSeverity(patientIndex, elapsedSeconds);

    PatientVitals vitals;
    vitals.id = QStringLiteral("PT-%1").arg(patientIndex + 1, 2, 10, QLatin1Char('0'));
    vitals.name = QString::fromLatin1(kNames[static_cast<std::size_t>(patientIndex) % kNames.size()]);
    vitals.heartRate = 70.0 + 5.0 * std::sin(kTwoPi * (elapsedSeconds + offset) / 12.0) + severity * 50.0;
    vitals.spo2 = 97.0 + 1.0 * std::sin(kTwoPi * (elapsedSeconds + offset) / 50.0) - severity * 9.0;
    vitals.systolic = 118.0 + 6.0 * std::sin(kTwoPi * (elapsedSeconds + offset) / 70.0) + severity * 10.0;
    vitals.diastolic = vitals.systolic * 0.64;
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
