// SPDX-License-Identifier: MIT
#include "IconPainter.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>

namespace qttutorial::medical::telehealth {

void IconPainter::paintHeartbeat(QPainter& painter, const QRectF& rect, const QColor& color)
{
    painter.save();
    painter.setPen(QPen(color, rect.width() * 0.12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    const double w = rect.width();
    const double h = rect.height();
    const double x = rect.left();
    const double y = rect.top();

    QPainterPath path;
    path.moveTo(x + w * 0.02, y + h * 0.55);
    path.lineTo(x + w * 0.28, y + h * 0.55);
    path.lineTo(x + w * 0.40, y + h * 0.15);
    path.lineTo(x + w * 0.52, y + h * 0.85);
    path.lineTo(x + w * 0.62, y + h * 0.55);
    path.lineTo(x + w * 0.80, y + h * 0.55);
    path.lineTo(x + w * 0.86, y + h * 0.42);
    path.lineTo(x + w * 0.98, y + h * 0.42);
    painter.drawPath(path);
    painter.restore();
}

void IconPainter::paintDroplet(QPainter& painter, const QRectF& rect, const QColor& color)
{
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    const double w = rect.width();
    const double h = rect.height();
    const double x = rect.left();
    const double y = rect.top();

    QPainterPath path;
    path.moveTo(x + w * 0.5, y + h * 0.06);
    path.cubicTo(x + w * 0.85, y + h * 0.45, x + w * 0.82, y + h * 0.72, x + w * 0.5, y + h * 0.94);
    path.cubicTo(x + w * 0.18, y + h * 0.72, x + w * 0.15, y + h * 0.45, x + w * 0.5, y + h * 0.06);
    path.closeSubpath();
    painter.drawPath(path);
    painter.restore();
}

void IconPainter::paintCuff(QPainter& painter, const QRectF& rect, const QColor& color)
{
    painter.save();
    const double w = rect.width();
    const double h = rect.height();
    const double x = rect.left();
    const double y = rect.top();

    painter.setPen(QPen(color, w * 0.06));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(QRectF(x + w * 0.08, y + h * 0.28, w * 0.5, h * 0.44), w * 0.08, h * 0.08);
    painter.drawLine(QPointF(x + w * 0.33, y + h * 0.28), QPointF(x + w * 0.33, y + h * 0.72));
    painter.drawLine(QPointF(x + w * 0.58, y + h * 0.5), QPointF(x + w * 0.76, y + h * 0.5));

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(QRectF(x + w * 0.68, y + h * 0.34, w * 0.24, h * 0.32));
    painter.restore();
}

void IconPainter::paintBell(QPainter& painter, const QRectF& rect, const QColor& color)
{
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    const double w = rect.width();
    const double h = rect.height();
    const double x = rect.left();
    const double y = rect.top();

    QPainterPath body;
    body.moveTo(x + w * 0.18, y + h * 0.68);
    body.arcTo(QRectF(x + w * 0.18, y + h * 0.10, w * 0.64, h * 0.64), 180.0, 180.0);
    body.lineTo(x + w * 0.82, y + h * 0.68);
    body.closeSubpath();
    painter.drawPath(body);

    painter.drawRect(QRectF(x + w * 0.12, y + h * 0.68, w * 0.76, h * 0.06));
    painter.drawEllipse(QRectF(x + w * 0.38, y + h * 0.78, w * 0.24, h * 0.16));
    painter.restore();
}

} // namespace qttutorial::medical::telehealth
