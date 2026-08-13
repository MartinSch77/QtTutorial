// SPDX-License-Identifier: MIT
#include "CompassRose.h"

#include <QPainter>
#include <QPainterPath>

#include <cmath>
#include <numbers>

namespace qttutorial::avionics {

namespace {

// A simple top-down aircraft silhouette built entirely from primitive path
// segments (a fuselage line, a swept wing polygon, a small tail polygon) - a
// generic geometric shape, not any specific aircraft's outline or a traced
// image.
void drawAircraftSilhouette(QPainter& painter, double scale, const QColor& color)
{
    painter.setPen(QPen(color, 2.0 * scale, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(0.0, -16.0 * scale), QPointF(0.0, 14.0 * scale));

    QPolygonF wings;
    wings << QPointF(0.0, -2.0 * scale) << QPointF(22.0 * scale, 8.0 * scale) << QPointF(6.0 * scale, 8.0 * scale)
          << QPointF(0.0, 2.0 * scale) << QPointF(-6.0 * scale, 8.0 * scale) << QPointF(-22.0 * scale, 8.0 * scale);
    painter.setPen(QPen(color, 1.0 * scale));
    painter.setBrush(color);
    painter.drawPolygon(wings);

    QPolygonF tail;
    tail << QPointF(0.0, 8.0 * scale) << QPointF(8.0 * scale, 14.0 * scale) << QPointF(-8.0 * scale, 14.0 * scale);
    painter.drawPolygon(tail);
}

} // namespace

CompassRose::CompassRose(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(180, 180);
}

void CompassRose::setHeading(double headingDeg)
{
    m_headingDeg = std::fmod(headingDeg + 360.0, 360.0);
    update();
}

void CompassRose::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x10, 0x10, 0x10));

    const QPointF center = rect().center();
    const double radius = std::min(width(), height()) / 2.0 - 8.0;

    painter.save();
    painter.translate(center);

    painter.setPen(QPen(QColor(0x50, 0x50, 0x50), 1.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QPointF(0.0, 0.0), radius, radius);

    // The card rotates opposite the aircraft heading behind the fixed lubber
    // line/aircraft symbol - the standard compass-card convention.
    painter.save();
    painter.rotate(-m_headingDeg);
    painter.setFont(QFont(font().family(), 8));
    for (int mark = 0; mark < 360; mark += 30) {
        painter.save();
        painter.rotate(mark);
        painter.setPen(QPen(Qt::white, 1.5));
        painter.drawLine(QPointF(0.0, -radius), QPointF(0.0, -radius + 10.0));
        const QString label = mark == 0 ? QStringLiteral("N")
            : mark == 90 ? QStringLiteral("E")
            : mark == 180 ? QStringLiteral("S")
            : mark == 270 ? QStringLiteral("W")
            : QString::number(mark / 10);
        painter.translate(0.0, -radius + 22.0);
        painter.rotate(-mark);
        painter.setPen(Qt::white);
        painter.drawText(QRectF(-12.0, -8.0, 24.0, 16.0), Qt::AlignCenter, label);
        painter.restore();
    }
    for (int mark = 0; mark < 360; mark += 10) {
        if (mark % 30 == 0) {
            continue;
        }
        painter.save();
        painter.rotate(mark);
        painter.setPen(QPen(Qt::white, 1.0));
        painter.drawLine(QPointF(0.0, -radius), QPointF(0.0, -radius + 5.0));
        painter.restore();
    }
    painter.restore();

    // Fixed lubber line at the top of the case.
    painter.setPen(QPen(Qt::yellow, 2.0));
    QPolygonF lubberLine;
    lubberLine << QPointF(0.0, -radius - 6.0) << QPointF(-5.0, -radius + 4.0) << QPointF(5.0, -radius + 4.0);
    painter.setBrush(Qt::yellow);
    painter.drawPolygon(lubberLine);

    // Fixed aircraft silhouette, always pointing "up", pinned at the centre.
    drawAircraftSilhouette(painter, 0.7, Qt::yellow);

    painter.restore();

    const QRectF readoutRect(center.x() - 22.0, height() - 22.0, 44.0, 18.0);
    painter.setPen(QPen(Qt::yellow, 1.5));
    painter.setBrush(QColor(0x10, 0x10, 0x10));
    painter.drawRect(readoutRect);
    painter.setPen(Qt::yellow);
    painter.drawText(readoutRect, Qt::AlignCenter, QString::number(std::lround(m_headingDeg)));
}

} // namespace qttutorial::avionics
