// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

#include <vector>

namespace qttutorial::two_wheelers::fleet {

// Hand-painted trend line for a single vehicle's recent speed history, drawn
// with QPainter rather than a charting library (Qt Charts is not MIT licensed).
class TrendWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrendWidget(QWidget* parent = nullptr);

    void setValues(const std::vector<double>& values, double maxValue);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<double> m_values;
    double m_maxValue = 1.0;
};

} // namespace qttutorial::two_wheelers::fleet
