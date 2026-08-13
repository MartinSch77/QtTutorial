// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

#include <vector>

namespace qttutorial::mining::pit {

// Hand-painted trend line of a single truck's cumulative tonnes hauled, drawn
// with QPainter rather than a charting library (Qt Charts is not MIT licensed).
class ProductionTrendWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProductionTrendWidget(QWidget* parent = nullptr);

    void setValues(const std::vector<double>& values, double maxValue);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<double> m_values;
    double m_maxValue = 1.0;
};

} // namespace qttutorial::mining::pit
