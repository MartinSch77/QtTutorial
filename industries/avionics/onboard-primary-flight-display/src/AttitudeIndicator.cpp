// SPDX-License-Identifier: MIT
#include "AttitudeIndicator.h"

#include <QPainter>
#include <QPainterPath>

#include <cmath>

namespace qttutorial::avionics {

namespace {
constexpr double kPixelsPerDegree = 8.0;
}

AttitudeIndicator::AttitudeIndicator(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(260, 260);
}

void AttitudeIndicator::setAttitude(double pitchDeg, double rollDeg)
{
    m_pitchDeg = pitchDeg;
    m_rollDeg = rollDeg;
    update();
}

void AttitudeIndicator::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF bounds = rect();
    const QPointF center = bounds.center();
    const double radius = std::min(bounds.width(), bounds.height()) / 2.0 - 4.0;

    painter.save();
    painter.setClipRegion(QRegion(bounds.toRect(), QRegion::Ellipse));

    painter.translate(center);
    painter.rotate(-m_rollDeg);
    painter.translate(0.0, m_pitchDeg * kPixelsPerDegree);

    const double span = radius * 6.0;
    const QRectF skyRect(-span, -span, span * 2.0, span);
    const QRectF groundRect(-span, 0.0, span * 2.0, span);
    painter.fillRect(skyRect, QColor(0x2f, 0x6f, 0xb5));
    painter.fillRect(groundRect, QColor(0x6b, 0x4a, 0x27));

    QPen horizonPen(Qt::white, 2.0);
    painter.setPen(horizonPen);
    painter.drawLine(QPointF(-span, 0.0), QPointF(span, 0.0));

    painter.setPen(QPen(Qt::white, 1.5));
    painter.setFont(QFont(font().family(), 8));
    for (int mark = -90; mark <= 90; mark += 10) {
        if (mark == 0) {
            continue;
        }
        const double y = -mark * kPixelsPerDegree;
        const double halfWidth = (mark % 30 == 0) ? 40.0 : 20.0;
        painter.drawLine(QPointF(-halfWidth, y), QPointF(halfWidth, y));
        if (mark % 30 == 0) {
            painter.drawText(QPointF(halfWidth + 4.0, y + 4.0), QString::number(mark));
        }
    }

    painter.restore();

    painter.save();
    painter.translate(center);
    QPainterPath rollScale;
    rollScale.addEllipse(QPointF(0.0, 0.0), radius, radius);
    painter.setPen(QPen(Qt::white, 1.5));
    for (int mark = -60; mark <= 60; mark += 10) {
        painter.save();
        painter.rotate(mark);
        const double tickLength = (mark % 30 == 0) ? 12.0 : 6.0;
        painter.drawLine(QPointF(0.0, -radius), QPointF(0.0, -radius + tickLength));
        painter.restore();
    }

    painter.setPen(QPen(Qt::yellow, 2.0));
    painter.save();
    painter.rotate(-m_rollDeg);
    QPolygonF pointer;
    pointer << QPointF(0.0, -radius + 2.0) << QPointF(-6.0, -radius + 16.0) << QPointF(6.0, -radius + 16.0);
    painter.setBrush(Qt::yellow);
    painter.drawPolygon(pointer);
    painter.restore();

    QPolygonF aircraftSymbol;
    painter.setPen(QPen(Qt::yellow, 3.0));
    painter.drawLine(QPointF(-30.0, 0.0), QPointF(-8.0, 0.0));
    painter.drawLine(QPointF(8.0, 0.0), QPointF(30.0, 0.0));
    painter.drawLine(QPointF(0.0, -6.0), QPointF(0.0, 0.0));
    painter.setBrush(Qt::yellow);
    painter.drawEllipse(QPointF(0.0, 0.0), 3.0, 3.0);

    painter.restore();

    painter.setPen(QPen(Qt::black, 3.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(bounds.adjusted(1.0, 1.0, -1.0, -1.0));
}

} // namespace qttutorial::avionics
