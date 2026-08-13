// SPDX-License-Identifier: MIT
#include "VerticalSpeedIndicator.h"

#include <QPainter>

#include <algorithm>
#include <cmath>

namespace qttutorial::avionics {

namespace {
constexpr double kMaxFeetPerMinute = 2000.0;

double toFraction(double fpm)
{
    const double clamped = std::clamp(fpm, -kMaxFeetPerMinute, kMaxFeetPerMinute);
    const double sign = clamped < 0.0 ? -1.0 : 1.0;
    return sign * std::sqrt(std::abs(clamped) / kMaxFeetPerMinute);
}
}

VerticalSpeedIndicator::VerticalSpeedIndicator(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(70, 260);
}

void VerticalSpeedIndicator::setVerticalSpeed(double feetPerMinute)
{
    m_feetPerMinute = feetPerMinute;
    update();
}

void VerticalSpeedIndicator::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x10, 0x10, 0x10));

    const double centerY = height() / 2.0;
    const double halfHeight = height() / 2.0 - 10.0;

    painter.setPen(QPen(Qt::white, 1.0));
    painter.setFont(QFont(font().family(), 7));
    for (int fpm : {-2000, -1000, -500, 0, 500, 1000, 2000}) {
        const double y = centerY - toFraction(fpm) * halfHeight;
        painter.drawLine(QPointF(width() - 14.0, y), QPointF(width() - 2.0, y));
        painter.drawText(QRectF(0.0, y - 8.0, width() - 16.0, 16.0), Qt::AlignRight | Qt::AlignVCenter,
                          QString::number(fpm / 1000.0, 'f', fpm % 1000 == 0 ? 0 : 1));
    }

    const double needleY = centerY - toFraction(m_feetPerMinute) * halfHeight;
    painter.setPen(QPen(Qt::green, 2.0));
    painter.drawLine(QPointF(width() - 2.0, centerY), QPointF(width() - 2.0, needleY));
    QPolygonF arrow;
    arrow << QPointF(width() - 24.0, needleY) << QPointF(width() - 2.0, needleY - 5.0)
          << QPointF(width() - 2.0, needleY + 5.0);
    painter.setBrush(Qt::green);
    painter.drawPolygon(arrow);
}

} // namespace qttutorial::avionics
