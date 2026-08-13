// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

#include <vector>

namespace qttutorial::avionics {

// A hand-painted trend line (no Qt Charts, which is GPL/commercial-only): plots
// a series of doubles against a nominal band so the operator can see a parameter
// drifting before it is flagged.
class TrendWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrendWidget(QWidget* parent = nullptr);

    void setSeries(std::vector<double> values, double nominalLow, double nominalHigh);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<double> m_values;
    double m_nominalLow = 0.0;
    double m_nominalHigh = 1.0;
};

} // namespace qttutorial::avionics
