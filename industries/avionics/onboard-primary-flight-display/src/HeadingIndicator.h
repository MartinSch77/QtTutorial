// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

namespace qttutorial::avionics {

// A rotating compass card: the dial rotates opposite the aircraft heading behind a
// fixed lubber line at the top, the standard PFD/HSI convention.
class HeadingIndicator : public QWidget {
    Q_OBJECT
public:
    explicit HeadingIndicator(QWidget* parent = nullptr);

    void setHeading(double headingDeg);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_headingDeg = 0.0;
};

} // namespace qttutorial::avionics
