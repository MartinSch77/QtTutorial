// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

namespace qttutorial::agriculture::ops {

// Hand-painted 2D field-map view, drawn with QPainter rather than a charting
// library (Qt Charts is not MIT licensed): a plain rectangular field boundary
// with a marker that moves along the perimeter as the selected field's
// simulated vehicle progresses through its current pass.
class FieldMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit FieldMapWidget(QWidget* parent = nullptr);

    void setProgress(double coveragePercent, bool hasData);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_coveragePercent = 0.0;
    bool m_hasData = false;
};

} // namespace qttutorial::agriculture::ops
