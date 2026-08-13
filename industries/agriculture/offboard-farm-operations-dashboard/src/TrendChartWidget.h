// SPDX-License-Identifier: MIT
#pragma once

#include <QString>
#include <QWidget>

#include <vector>

namespace qttutorial::agriculture::ops {

// A small dependency-free line-chart widget, painted with QPainter (this app
// already avoids Qt Charts, which is not MIT licensed, in favour of QPainter
// for FieldMapWidget; this reuses that same approach rather than introducing
// a new charting dependency). Used to show recent fuel-level/engine-load
// history for the selected field, pulled from OperationHistoryStore.
class TrendChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit TrendChartWidget(QWidget* parent = nullptr);

    void setSeries(const std::vector<double>& values, double minValue, double maxValue, const QString& unit);
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<double> m_values;
    double m_minValue = 0.0;
    double m_maxValue = 100.0;
    QString m_unit;
    bool m_hasData = false;
};

} // namespace qttutorial::agriculture::ops
