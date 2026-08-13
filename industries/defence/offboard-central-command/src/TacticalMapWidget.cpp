// SPDX-License-Identifier: MIT
#include "TacticalMapWidget.h"

#include <QPainter>
#include <QPen>
#include <QPolygonF>

#include <algorithm>

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

    for (const Asset& asset : m_assets) {
        const double px = cx + asset.xKm * scale;
        const double py = cy + asset.yKm * scale;
        const QColor color = colorForHealth(asset.health);

        painter.setPen(QPen(color, 2));
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

        painter.setPen(QPen(QColor(0xf2, 0xf4, 0xf8), 1));
        painter.drawText(QPointF(px + 10, py - 8), asset.id);
    }
}

} // namespace qttutorial::defence
