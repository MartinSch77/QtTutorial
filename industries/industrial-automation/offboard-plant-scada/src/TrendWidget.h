// SPDX-License-Identifier: MIT
#pragma once

#include "Historian.h"

#include <QWidget>

namespace qttutorial::plant_scada {

// A hand-drawn QPainter trend chart (no Qt Charts / Qt Graphs dependency,
// both commercial-only): a simple axis plus polyline over the samples the
// historian returns for the selected tag and time window.
class TrendWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrendWidget(QWidget* parent = nullptr);

    void setSamples(const std::vector<Sample>& samples, double lowLimit, double highLimit);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<Sample> m_samples;
    double m_lowLimit = 0.0;
    double m_highLimit = 1.0;
};

} // namespace qttutorial::plant_scada
