// SPDX-License-Identifier: MIT
#include "IconPainter.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QRectF>

#include <algorithm>

namespace qttutorial::space::icons {

void drawSatellite(QPainter& painter, const QRectF& rect, const QColor& color)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(rect.center());
    const double s = std::min(rect.width(), rect.height());

    QPen pen(color, s * 0.06);
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    // Body.
    const QRectF body(-s * 0.14, -s * 0.14, s * 0.28, s * 0.28);
    painter.drawRect(body);

    // Solar panels, one either side of the body, each split into two cells.
    for (const double side : {-1.0, 1.0}) {
        const QRectF panel(side * s * 0.14 + (side > 0 ? 0 : -s * 0.32), -s * 0.10, s * 0.32, s * 0.20);
        painter.drawRect(panel);
        painter.drawLine(QPointF(panel.center().x(), panel.top()), QPointF(panel.center().x(), panel.bottom()));
    }

    // Antenna: a stalk plus a small dish disc.
    painter.drawLine(QPointF(0.0, -s * 0.14), QPointF(0.0, -s * 0.34));
    painter.drawEllipse(QPointF(0.0, -s * 0.38), s * 0.05, s * 0.05);

    painter.restore();
}

void drawOrbitArc(QPainter& painter, const QRectF& rect, const QColor& color)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    const double s = std::min(rect.width(), rect.height());

    // Central body being orbited.
    painter.setPen(Qt::NoPen);
    painter.setBrush(color.darker(150));
    painter.drawEllipse(rect.center(), s * 0.16, s * 0.16);

    // Dashed orbital ellipse.
    QPen orbitPen(color, s * 0.035, Qt::DashLine);
    painter.setPen(orbitPen);
    painter.setBrush(Qt::NoBrush);
    const QRectF orbitRect(rect.center().x() - s * 0.42, rect.center().y() - s * 0.24, s * 0.84, s * 0.48);
    painter.drawEllipse(orbitRect);

    // A small satellite marker riding on the arc.
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    const QPointF markerPos(orbitRect.right(), orbitRect.center().y());
    painter.drawEllipse(markerPos, s * 0.06, s * 0.06);

    painter.restore();
}

void drawGroundStation(QPainter& painter, const QRectF& rect, const QColor& color, bool active)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(rect.center().x(), rect.bottom());
    const double s = std::min(rect.width(), rect.height());

    QPen pen(color, s * 0.05);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    // Ground line and mount stand.
    painter.drawLine(QPointF(-s * 0.30, 0.0), QPointF(s * 0.30, 0.0));
    painter.drawLine(QPointF(0.0, 0.0), QPointF(0.0, -s * 0.20));

    // Parabolic dish, drawn as an arc opening upward, with a feed post.
    QPainterPath dish;
    dish.moveTo(-s * 0.28, -s * 0.20);
    dish.quadTo(0.0, -s * 0.55, s * 0.28, -s * 0.20);
    painter.drawPath(dish);
    painter.drawLine(QPointF(0.0, -s * 0.20), QPointF(0.0, -s * 0.40));
    painter.drawEllipse(QPointF(0.0, -s * 0.42), s * 0.035, s * 0.035);

    if (active) {
        // Signal arcs radiating from the feed when a pass is in view.
        QPen signalPen(color.lighter(140), s * 0.03);
        painter.setPen(signalPen);
        for (int i = 1; i <= 2; ++i) {
            QRectF signalRect(-s * 0.10 * i, -s * 0.55 - s * 0.10 * i, s * 0.20 * i, s * 0.20 * i);
            painter.drawArc(signalRect, 30 * 16, 120 * 16);
        }
    }

    painter.restore();
}

void drawBatteryCharge(QPainter& painter, const QRectF& rect, const QColor& color, bool charging)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    const double s = std::min(rect.width(), rect.height());
    const QPointF c = rect.center();

    QPen pen(color, s * 0.05);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    const QRectF body(c.x() - s * 0.32, c.y() - s * 0.18, s * 0.58, s * 0.36);
    painter.drawRect(body);
    const QRectF nub(body.right(), c.y() - s * 0.06, s * 0.06, s * 0.12);
    painter.setBrush(color);
    painter.drawRect(nub);

    if (charging) {
        // A lightning-bolt glyph inside the battery body.
        QPainterPath bolt;
        bolt.moveTo(body.center().x() + s * 0.06, body.top() + s * 0.02);
        bolt.lineTo(body.center().x() - s * 0.10, body.center().y());
        bolt.lineTo(body.center().x() + s * 0.02, body.center().y());
        bolt.lineTo(body.center().x() - s * 0.06, body.bottom() - s * 0.02);
        bolt.lineTo(body.center().x() + s * 0.14, body.center().y() - s * 0.02);
        bolt.lineTo(body.center().x() + s * 0.02, body.center().y() - s * 0.02);
        bolt.closeSubpath();
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawPath(bolt);
    } else {
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.drawRect(QRectF(body.left() + s * 0.04, body.top() + s * 0.04, body.width() * 0.35, body.height() - s * 0.08));
    }

    painter.restore();
}

void drawWarningTriangle(QPainter& painter, const QRectF& rect, const QColor& color)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    const double s = std::min(rect.width(), rect.height());
    const QPointF c = rect.center();

    QPainterPath triangle;
    triangle.moveTo(c.x(), c.y() - s * 0.38);
    triangle.lineTo(c.x() - s * 0.38, c.y() + s * 0.30);
    triangle.lineTo(c.x() + s * 0.38, c.y() + s * 0.30);
    triangle.closeSubpath();

    QPen pen(color, s * 0.06);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(triangle);

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRect(QRectF(c.x() - s * 0.035, c.y() - s * 0.18, s * 0.07, s * 0.28));
    painter.drawEllipse(QPointF(c.x(), c.y() + s * 0.20), s * 0.045, s * 0.045);

    painter.restore();
}

} // namespace qttutorial::space::icons
