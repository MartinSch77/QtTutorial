// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

#include <vector>

namespace qttutorial::space {

// A hand-painted trend line (no Qt Charts/Graphs) of a selected satellite's
// battery percentage over its most recent simulated pass, shaded to show
// eclipse periods.
class BatteryTrendWidget : public QWidget {
    Q_OBJECT
public:
    explicit BatteryTrendWidget(QWidget* parent = nullptr);

    void setSeries(std::vector<double> batteryPercent, std::vector<bool> inEclipse);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<double> m_batteryPercent;
    std::vector<bool> m_inEclipse;
};

} // namespace qttutorial::space
