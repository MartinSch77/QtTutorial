// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

namespace qttutorial::space {

// A hand-painted radial gauge (no Qt Charts/Graphs, GPL/commercial-only):
// a 270-degree arc from minValue to maxValue with a coloured needle and a
// digital readout. Reused for battery state-of-charge and each thermal zone.
class RadialGauge : public QWidget {
    Q_OBJECT
public:
    RadialGauge(QString title, QString unit, double minValue, double maxValue, QWidget* parent = nullptr);

    void setValue(double value);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_title;
    QString m_unit;
    double m_minValue;
    double m_maxValue;
    double m_value;
};

} // namespace qttutorial::space
