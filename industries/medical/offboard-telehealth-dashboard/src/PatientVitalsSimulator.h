// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::medical::telehealth {

enum class AlarmSeverity { Normal, Warning, Critical };

struct PatientVitals {
    QString id;
    QString name;
    double heartRate = 72.0;
    double spo2 = 97.0;
    double systolic = 118.0;
    double diastolic = 75.0;
};

// Deterministic, phase-shifted per-patient vitals feed standing in for a
// telehealth ingestion service aggregating several remote monitors. Each
// patient has a plausible baseline with respiratory-driven wobble, and one
// patient (index 0) periodically runs a short deterioration episode so the
// alert list has something real to surface.
class PatientVitalsSimulator {
public:
    [[nodiscard]] static PatientVitals sampleAt(int patientIndex, double elapsedSeconds);
    [[nodiscard]] static AlarmSeverity classify(const PatientVitals& vitals);
};

} // namespace qttutorial::medical::telehealth
