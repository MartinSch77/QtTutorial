// SPDX-License-Identifier: MIT
#include "TacticalMapWidget.h"

#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPolygonF>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::defence {

namespace {
QColor colorForHealth(const QString& health)
{
    if (health == QStringLiteral("Critical")) {
        return QColor(0xc0, 0x39, 0x2b);
    }
    if (health == QStringLiteral("Caution")) {
        return QColor(0xe0, 0xa3, 0x00);
    }
    return QColor(0x3d, 0xdc, 0x6f);
}
}

TacticalMapWidget::TacticalMapWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(320, 320);
}

void TacticalMapWidget::setAssets(std::vector<Asset> assets)
{
    m_assets = std::move(assets);
    update();
}

void TacticalMapWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(0x0a, 0x0f, 0x14));

    const double cx = width() / 2.0;
    const double cy = height() / 2.0;
    const double radius = std::min(width(), height()) / 2.0 - 12.0;
    const double scale = radius / m_mapRangeKm;

    painter.setPen(QPen(QColor(0x1f, 0x2a, 0x33), 1));
    for (int ring = 1; ring <= 4; ++ring) {
        const double r = radius * ring / 4.0;
        painter.drawEllipse(QPointF(cx, cy), r, r);
    }
    painter.drawLine(QPointF(cx - radius, cy), QPointF(cx + radius, cy));
    painter.drawLine(QPointF(cx, cy - radius), QPointF(cx, cy + radius));

    drawCompassRose(painter, QPointF(cx, cy), radius);

    for (const Asset& asset : m_assets) {
        const double px = cx + asset.xKm * scale;
        const double py = cy + asset.yKm * scale;
        const QColor color = colorForHealth(asset.health);

        painter.setOpacity(asset.trackStale ? 0.55 : 1.0);
        painter.setPen(QPen(color, 2, asset.trackStale ? Qt::DashLine : Qt::SolidLine));
        painter.setBrush(color);
        switch (asset.type) {
        case AssetType::Vehicle:
            painter.drawEllipse(QPointF(px, py), 6, 6);
            break;
        case AssetType::Drone: {
            QPolygonF triangle;
            triangle << QPointF(px, py - 7) << QPointF(px - 6, py + 5) << QPointF(px + 6, py + 5);
            painter.drawPolygon(triangle);
            break;
        }
        case AssetType::Vessel: {
            QPolygonF diamond;
            diamond << QPointF(px, py - 7) << QPointF(px + 7, py) << QPointF(px, py + 7) << QPointF(px - 7, py);
            painter.drawPolygon(diamond);
            break;
        }
        case AssetType::Installation:
            painter.drawRect(QRectF(px - 6, py - 6, 12, 12));
            break;
        }
        painter.setOpacity(1.0);

        painter.setPen(QPen(QColor(0xf2, 0xf4, 0xf8), 1));
        QString label = asset.id;
        if (asset.trackStale) {
            label += QStringLiteral(" (stale %1s)").arg(QString::number(asset.dataAgeSeconds, 'f', 0));
        }
        painter.drawText(QPointF(px + 10, py - 8), label);

        if (asset.trackStale) {
            drawWarningTriangle(painter, QPointF(px - 12, py - 14), 6.0);
        } else if (asset.commsQualityPercent < 60.0) {
            drawAntennaGlyph(painter, QPointF(px - 14, py + 14), 8.0, asset.commsQualityPercent);
        }
    }
}

void TacticalMapWidget::drawCompassRose(QPainter& painter, QPointF center, double radius) const
{
    painter.save();
    painter.setPen(QPen(QColor(0x33, 0x42, 0x4f), 1));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    const char* labels[] = {"N", "E", "S", "W"};
    for (int deg = 0; deg < 360; deg += 30) {
        const double rad = (deg - 90) * std::numbers::pi / 180.0;
        const bool isMajor = deg % 90 == 0;
        const double tickLength = isMajor ? 12.0 : 6.0;
        const QPointF outer(center.x() + std::cos(rad) * radius, center.y() + std::sin(rad) * radius);
        const QPointF inner(center.x() + std::cos(rad) * (radius - tickLength),
                             center.y() + std::sin(rad) * (radius - tickLength));
        painter.drawLine(outer, inner);
        if (isMajor) {
            const QPointF labelPos(center.x() + std::cos(rad) * (radius + 12),
                                    center.y() + std::sin(rad) * (radius + 12));
            painter.drawText(labelPos, QString::fromLatin1(labels[deg / 90]));
        }
    }
    painter.restore();
}

void TacticalMapWidget::drawWarningTriangle(QPainter& painter, QPointF center, double size) const
{
    painter.save();
    painter.setOpacity(1.0);
    QPainterPath path;
    path.moveTo(center.x(), center.y() - size);
    path.lineTo(center.x() - size, center.y() + size * 0.8);
    path.lineTo(center.x() + size, center.y() + size * 0.8);
    path.closeSubpath();
    painter.setPen(QPen(QColor(0x1a, 0x1a, 0x1a), 1));
    painter.setBrush(QColor(0xe0, 0xa3, 0x00));
    painter.drawPath(path);
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(static_cast<int>(size));
    painter.setFont(font);
    painter.setPen(QColor(0x1a, 0x1a, 0x1a));
    painter.drawText(QRectF(center.x() - size, center.y() - size, size * 2, size * 1.8), Qt::AlignCenter,
                      QStringLiteral("!"));
    painter.restore();
}

void TacticalMapWidget::drawAntennaGlyph(QPainter& painter, QPointF center, double size, double qualityPercent) const
{
    painter.save();
    painter.setOpacity(1.0);
    const QColor color(0xe0, 0xa3, 0x00);
    painter.setPen(QPen(color, 1.5));
    painter.drawLine(QPointF(center.x(), center.y() + size), QPointF(center.x(), center.y() - size * 0.2));
    painter.drawLine(QPointF(center.x(), center.y() - size * 0.2),
                      QPointF(center.x() - size * 0.35, center.y() - size * 0.6));
    painter.drawLine(QPointF(center.x(), center.y() - size * 0.2),
                      QPointF(center.x() + size * 0.35, center.y() - size * 0.6));

    const int litBars = std::clamp(static_cast<int>(std::lround(qualityPercent / 25.0)), 0, 4);
    for (int i = 0; i < 4; ++i) {
        const double barHeight = size * 0.25 * (i + 1);
        painter.setOpacity(i < litBars ? 1.0 : 0.25);
        painter.fillRect(QRectF(center.x() + size * 0.5 + i * size * 0.3, center.y() + size - barHeight,
                                 size * 0.22, barHeight),
                          color);
    }
    painter.restore();
}

} // namespace qttutorial::defence
