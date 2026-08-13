// SPDX-License-Identifier: MIT
#include "FleetIcons.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QRectF>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::two_wheelers::fleet::icons {

namespace {
constexpr double kTwoPi = 2.0 * std::numbers::pi;
}

void paintMotorcycleIcon(QPainter& painter, const QRectF& bounds, const QColor& strokeColor)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(strokeColor, std::max(1.0, bounds.height() * 0.09)));
    painter.setBrush(Qt::NoBrush);

    const double w = bounds.width();
    const double h = bounds.height();
    const double x0 = bounds.left();
    const double y0 = bounds.top();
    const double wheelRadius = h * 0.28;
    const double rearX = x0 + w * 0.26;
    const double frontX = x0 + w * 0.78;
    const double wheelY = y0 + h * 0.72;

    painter.drawEllipse(QPointF(rearX, wheelY), wheelRadius, wheelRadius);
    painter.drawEllipse(QPointF(frontX, wheelY), wheelRadius, wheelRadius);

    QPainterPath body;
    body.moveTo(rearX, wheelY - wheelRadius * 0.2);
    body.lineTo(x0 + w * 0.42, y0 + h * 0.38);
    body.lineTo(x0 + w * 0.60, y0 + h * 0.36);
    body.lineTo(x0 + w * 0.68, y0 + h * 0.5);
    body.lineTo(frontX, wheelY - wheelRadius * 0.2);
    painter.drawPath(body);

    painter.restore();
}

void paintMaintenanceBadgeIcon(QPainter& painter, const QRectF& bounds, const QColor& color, bool due)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    const QPointF center = bounds.center();
    const double outerRadius = std::min(bounds.width(), bounds.height()) / 2.0 - 1.0;
    const double innerRadius = outerRadius * 0.78;
    constexpr int kToothCount = 8;

    QPainterPath cog;
    for (int i = 0; i < kToothCount * 2; ++i) {
        const double angle = kTwoPi * static_cast<double>(i) / static_cast<double>(kToothCount * 2);
        const double r = (i % 2 == 0) ? outerRadius : innerRadius;
        const QPointF point(center.x() + std::cos(angle) * r, center.y() + std::sin(angle) * r);
        if (i == 0) {
            cog.moveTo(point);
        } else {
            cog.lineTo(point);
        }
    }
    cog.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(due ? color : QColor(color.red(), color.green(), color.blue(), 60));
    painter.drawPath(cog);

    if (due) {
        painter.setPen(QPen(Qt::white, std::max(1.0, outerRadius * 0.16)));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(center, innerRadius * 0.5, innerRadius * 0.5);
    }

    painter.restore();
}

void paintBatteryGaugeIcon(QPainter& painter, const QRectF& bounds, double percent)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    const double fraction = std::clamp(percent / 100.0, 0.0, 1.0);
    const QPointF center(bounds.center().x(), bounds.top() + bounds.height() * 0.62);
    const double radius = std::min(bounds.width(), bounds.height()) * 0.42;
    constexpr double kStartAngleDeg = 200.0;
    constexpr double kSweepAngleDeg = 140.0;

    painter.setPen(QPen(QColor("#3a4150"), std::max(1.0, radius * 0.18)));
    painter.drawArc(QRectF(center.x() - radius, center.y() - radius, radius * 2, radius * 2),
                     static_cast<int>(kStartAngleDeg * 16), static_cast<int>(kSweepAngleDeg * 16));

    const QColor levelColor = fraction < 0.15 ? QColor("#ff5a5a") : QColor("#39c0ff");
    painter.setPen(QPen(levelColor, std::max(1.0, radius * 0.18)));
    painter.drawArc(QRectF(center.x() - radius, center.y() - radius, radius * 2, radius * 2),
                     static_cast<int>(kStartAngleDeg * 16), static_cast<int>(kSweepAngleDeg * fraction * 16));

    const double needleAngleRad = (kStartAngleDeg + kSweepAngleDeg * fraction) * std::numbers::pi / 180.0;
    painter.setPen(QPen(Qt::white, std::max(1.0, radius * 0.12)));
    painter.drawLine(center, QPointF(center.x() + std::cos(needleAngleRad) * radius * 0.85,
                                      center.y() + std::sin(needleAngleRad) * radius * 0.85));

    painter.restore();
}

} // namespace qttutorial::two_wheelers::fleet::icons
