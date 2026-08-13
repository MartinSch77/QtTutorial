// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::qml_basics {

enum class IndicatorStatus { Nominal, Warning, Critical };

struct IndicatorReading {
    QString name;
    double value = 0.0;
    IndicatorStatus status = IndicatorStatus::Nominal;
};

// Pure, deterministic data source for the "system status" demo: no QObject, no
// randomness, so it is trivially unit-testable and reusable outside of QML.
class IndicatorGenerator {
public:
    [[nodiscard]] static IndicatorStatus statusFor(double value, double warningThreshold,
                                                     double criticalThreshold);

    [[nodiscard]] static std::vector<QString> indicatorNames();

    [[nodiscard]] static std::vector<IndicatorReading> generate(int tick);
};

} // namespace qttutorial::qml_basics
