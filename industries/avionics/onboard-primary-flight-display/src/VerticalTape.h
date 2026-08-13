// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

namespace qttutorial::avionics {

// A scrolling vertical scale, the shared painting logic behind both the airspeed
// and altitude "tapes" on a PFD: a fixed centre readout box with the scale sliding
// past it as the value changes. Configurable spacing lets one implementation serve
// both instruments instead of duplicating near-identical paint code.
class VerticalTape : public QWidget {
    Q_OBJECT
public:
    explicit VerticalTape(double majorStep, double pixelsPerUnit, QWidget* parent = nullptr);

    void setValue(double value);
    [[nodiscard]] double value() const { return m_value; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_value = 0.0;
    double m_majorStep;
    double m_pixelsPerUnit;
};

} // namespace qttutorial::avionics
