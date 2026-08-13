// SPDX-License-Identifier: MIT
#pragma once

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>

namespace qttutorial::agriculture::ops::icons {

// A small set of procedurally-drawn farm-equipment glyphs shared by this
// app's widgets, painted as simple geometric vector paths with QPainter (no
// image/SVG/icon-font assets), matching the same "hand painted, no external
// assets" spirit as FieldMapWidget's field boundary.

inline void paintTractorGlyph(QPainter& painter, const QRectF& r, const QColor& color)
{
    painter.save();
    painter.setPen(QPen(color, r.height() * 0.06));
    painter.setBrush(Qt::NoBrush);

    QPainterPath body;
    body.moveTo(r.left() + r.width() * 0.10, r.top() + r.height() * 0.62);
    body.lineTo(r.left() + r.width() * 0.10, r.top() + r.height() * 0.42);
    body.lineTo(r.left() + r.width() * 0.34, r.top() + r.height() * 0.42);
    body.lineTo(r.left() + r.width() * 0.44, r.top() + r.height() * 0.20);
    body.lineTo(r.left() + r.width() * 0.62, r.top() + r.height() * 0.20);
    body.lineTo(r.left() + r.width() * 0.62, r.top() + r.height() * 0.42);
    body.lineTo(r.left() + r.width() * 0.86, r.top() + r.height() * 0.42);
    body.lineTo(r.left() + r.width() * 0.86, r.top() + r.height() * 0.62);
    body.closeSubpath();
    painter.drawPath(body);

    painter.drawEllipse(QPointF(r.left() + r.width() * 0.28, r.top() + r.height() * 0.72), r.height() * 0.20,
                         r.height() * 0.20);
    painter.drawEllipse(QPointF(r.left() + r.width() * 0.74, r.top() + r.height() * 0.76), r.height() * 0.12,
                         r.height() * 0.12);
    painter.restore();
}

inline void paintFieldGlyph(QPainter& painter, const QRectF& r, const QColor& color)
{
    painter.save();
    painter.setPen(QPen(color, r.height() * 0.05));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(r.adjusted(r.width() * 0.10, r.height() * 0.15, -r.width() * 0.10, -r.height() * 0.15));

    const int rows = 4;
    const QRectF inner = r.adjusted(r.width() * 0.16, r.height() * 0.15, -r.width() * 0.16, -r.height() * 0.15);
    for (int i = 1; i <= rows; ++i) {
        const double y = inner.top() + inner.height() * (static_cast<double>(i) / (rows + 1));
        painter.drawLine(QPointF(inner.left(), y), QPointF(inner.right(), y));
    }
    painter.restore();
}

inline void paintFuelGlyph(QPainter& painter, const QRectF& r, const QColor& color)
{
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);

    const double cx = r.center().x();
    QPainterPath droplet;
    droplet.moveTo(cx, r.top() + r.height() * 0.08);
    droplet.cubicTo(r.left() + r.width() * 0.90, r.top() + r.height() * 0.55, r.left() + r.width() * 0.78,
                     r.top() + r.height() * 0.92, cx, r.top() + r.height() * 0.92);
    droplet.cubicTo(r.left() + r.width() * 0.22, r.top() + r.height() * 0.92, r.left() + r.width() * 0.10,
                     r.top() + r.height() * 0.55, cx, r.top() + r.height() * 0.08);
    droplet.closeSubpath();
    painter.drawPath(droplet);
    painter.restore();
}

} // namespace qttutorial::agriculture::ops::icons
