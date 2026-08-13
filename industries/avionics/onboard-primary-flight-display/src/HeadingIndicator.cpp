// SPDX-License-Identifier: MIT
#include "HeadingIndicator.h"

#include <QPainter>

#include <cmath>
#include <numbers>

namespace qttutorial::avionics {

HeadingIndicator::HeadingIndicator(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(220, 120);
}

void HeadingIndicator::setHeading(double headingDeg)
{
    m_headingDeg = std::fmod(headingDeg + 360.0, 360.0);
    update();
}

void HeadingIndicator::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x10, 0x10, 0x10));

    const double centerX = width() / 2.0;
    const double radius = height() - 20.0;
    const QPointF pivot(centerX, height() + radius - 30.0);

    painter.setPen(QPen(Qt::white, 1.0));
    painter.setFont(QFont(font().family(), 8));

    for (int mark = 0; mark < 360; mark += 10) {
        const double relativeAngle = mark - m_headingDeg;
        const double drawAngle = relativeAngle - 90.0;
        const double x = pivot.x() + radius * std::cos(drawAngle * std::numbers::pi / 180.0);
        const double y = pivot.y() + radius * std::sin(drawAngle * std::numbers::pi / 180.0);
        if (y > height()) {
            continue;
        }
        painter.save();
        painter.translate(x, y);
        painter.rotate(relativeAngle);
        const double tickLength = (mark % 30 == 0) ? 10.0 : 5.0;
        painter.drawLine(QPointF(0.0, 0.0), QPointF(0.0, tickLength));
        if (mark % 30 == 0) {
            const QString label = mark == 0 ? QStringLiteral("N")
                : mark == 90 ? QStringLiteral("E")
                : mark == 180 ? QStringLiteral("S")
                : mark == 270 ? QStringLiteral("W")
                : QString::number(mark / 10);
            painter.drawText(QRectF(-12.0, tickLength + 1.0, 24.0, 12.0), Qt::AlignCenter, label);
        }
        painter.restore();
    }

    painter.setPen(QPen(Qt::yellow, 2.0));
    QPolygonF lubberLine;
    lubberLine << QPointF(centerX, 4.0) << QPointF(centerX - 6.0, 14.0) << QPointF(centerX + 6.0, 14.0);
    painter.setBrush(Qt::yellow);
    painter.drawPolygon(lubberLine);

    const QRectF readoutRect(centerX - 26.0, 16.0, 52.0, 20.0);
    painter.setBrush(QColor(0x10, 0x10, 0x10));
    painter.setPen(QPen(Qt::yellow, 1.5));
    painter.drawRect(readoutRect);
    painter.setPen(Qt::yellow);
    painter.drawText(readoutRect, Qt::AlignCenter, QString::number(std::lround(m_headingDeg)));
}

} // namespace qttutorial::avionics
