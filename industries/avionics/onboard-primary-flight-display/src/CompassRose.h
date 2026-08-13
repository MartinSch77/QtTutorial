// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

namespace qttutorial::avionics {

// A round, rotating compass-rose dial: unlike the linear heading tape
// (HeadingIndicator), this is the full 360-degree card genre found on
// glass-cockpit navigation displays, with a fixed top-down aircraft silhouette
// (a plain geometric shape - fuselage, wings, tail, all drawn from primitive
// QPainterPath segments, no image asset) pinned at the centre so the *card*
// rotates opposite heading while the little aircraft always points "up",
// matching the standard compass-card convention. This is a style/genre
// reference only; it does not reproduce any specific vendor's exact rose
// artwork, colours or trademark.
class CompassRose : public QWidget {
    Q_OBJECT
public:
    explicit CompassRose(QWidget* parent = nullptr);

    void setHeading(double headingDeg);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_headingDeg = 0.0;
};

} // namespace qttutorial::avionics
