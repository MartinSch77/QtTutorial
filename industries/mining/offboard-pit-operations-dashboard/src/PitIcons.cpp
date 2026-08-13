// SPDX-License-Identifier: MIT
#include "PitIcons.h"

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QRectF>

#include <algorithm>

namespace qttutorial::mining::pit::icons {

QIcon warningTriangleIcon(const QColor& color, int pixelSize)
{
    QPixmap pixmap(pixelSize, pixelSize);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF r(0, 0, pixelSize, pixelSize);
    QPainterPath triangle;
    triangle.moveTo(r.width() * 0.5, r.height() * 0.06);
    triangle.lineTo(r.width() * 0.95, r.height() * 0.92);
    triangle.lineTo(r.width() * 0.05, r.height() * 0.92);
    triangle.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawPath(triangle);

    painter.setBrush(QColor(0x12, 0x15, 0x1b));
    painter.drawRect(QRectF(r.width() * 0.46, r.height() * 0.34, r.width() * 0.08, r.height() * 0.28));
    painter.drawEllipse(QRectF(r.width() * 0.455, r.height() * 0.68, r.width() * 0.09, r.height() * 0.09));

    painter.end();
    return QIcon(pixmap);
}

void paintTruckGlyph(QPainter& painter, const QRectF& rect, const QColor& color)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    const double w = rect.width();
    const double h = rect.height();
    const double x0 = rect.left();
    const double y0 = rect.top();

    QPainterPath body;
    body.moveTo(x0 + w * 0.10, y0 + h * 0.70);
    body.lineTo(x0 + w * 0.20, y0 + h * 0.30);
    body.lineTo(x0 + w * 0.80, y0 + h * 0.30);
    body.lineTo(x0 + w * 0.90, y0 + h * 0.70);
    body.closeSubpath();

    painter.setPen(QPen(color.darker(140), 1.0));
    painter.setBrush(color);
    painter.drawPath(body);

    painter.setBrush(QColor(0x12, 0x15, 0x1b));
    painter.drawEllipse(QPointF(x0 + w * 0.28, y0 + h * 0.78), w * 0.10, w * 0.10);
    painter.drawEllipse(QPointF(x0 + w * 0.72, y0 + h * 0.78), w * 0.10, w * 0.10);

    painter.restore();
}

void paintPitBenches(QPainter& painter, const QRectF& rect, const QColor& benchColor)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::NoBrush);

    constexpr int kBenchCount = 6;
    const double maxRadius = 0.5 * std::min(rect.width(), rect.height());
    const QPointF center = rect.center();

    for (int i = 0; i < kBenchCount; ++i) {
        const double t = static_cast<double>(i) / kBenchCount;
        const double radius = maxRadius * (1.0 - t * 0.85);
        QColor c = benchColor;
        c.setAlphaF(0.25 + 0.5 * t);
        painter.setPen(QPen(c, std::max(1.0, maxRadius * 0.05)));
        painter.drawEllipse(center, radius, radius);
    }

    painter.restore();
}

} // namespace qttutorial::mining::pit::icons
