// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

namespace qttutorial::avionics {

// A vertical-speed "ladder": a needle deflecting from a centre zero mark toward
// non-linearly spaced climb/descend graduations, matching a real VSI's compressed
// scale at high rates.
class VerticalSpeedIndicator : public QWidget {
    Q_OBJECT
public:
    explicit VerticalSpeedIndicator(QWidget* parent = nullptr);

    void setVerticalSpeed(double feetPerMinute);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_feetPerMinute = 0.0;
};

} // namespace qttutorial::avionics
