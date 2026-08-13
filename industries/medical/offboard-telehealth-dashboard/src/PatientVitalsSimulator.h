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
    double respirationRate = 16.0;
    double temperature = 37.0;
};

// Deterministic, phase-shifted per-patient vitals feed standing in for a
// telehealth ingestion service aggregating several remote monitors. Each
// patient has a plausible baseline with respiratory-driven wobble, plus two
// independent, clinically-motivated scenarios so vitals correlate the way a
// real patient's would rather than drifting independently:
//
//  - Patient index 0 periodically runs a short **desaturation event**: heart
//    rate up, SpO2 down, and respiration rate up too (low oxygen plausibly
//    correlates with faster, more laboured breathing) - so the alert list
//    has something real to surface.
//  - Patient index 1 periodically runs a **fever episode**, on a longer,
//    offset cycle: temperature up, with a milder correlated rise in heart
//    rate and respiration rate (a low-grade tachycardia/tachypnoea
//    accompanying a fever).
class PatientVitalsSimulator {
public:
    [[nodiscard]] static PatientVitals sampleAt(int patientIndex, double elapsedSeconds);
    [[nodiscard]] static AlarmSeverity classify(const PatientVitals& vitals);
};

} // namespace qttutorial::medical::telehealth
