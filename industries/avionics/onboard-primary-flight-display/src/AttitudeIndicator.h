// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

namespace qttutorial::avionics {

// The canonical hard Qt-painting exercise: an artificial horizon. The sky/ground
// "horizon plane" is rotated by roll and translated by pitch using real
// trigonometry (QPainter transforms), with a fixed aircraft symbol overlaid so the
// motion of the plane, not the symbol, reads as attitude change.
class AttitudeIndicator : public QWidget {
    Q_OBJECT
public:
    explicit AttitudeIndicator(QWidget* parent = nullptr);

    void setAttitude(double pitchDeg, double rollDeg);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_pitchDeg = 0.0;
    double m_rollDeg = 0.0;
};

} // namespace qttutorial::avionics
