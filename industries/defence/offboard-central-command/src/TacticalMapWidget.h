// SPDX-License-Identifier: MIT
#pragma once

#include "Asset.h"

#include <QWidget>

#include <vector>

namespace qttutorial::defence {

// A plain top-down 2D tactical map (QPainter-drawn, no real map tiles or
// imagery): a range-ring grid with a compass-rose overlay, a colored icon
// and id label per asset (colored by health), a small comms/antenna glyph
// when an asset's data link is degraded, and a warning-triangle + dashed
// outline for a stale (not currently updating) track. This is a passive
// plot of last-known positions - there is no engagement geometry, no
// weapons range ring, and no target cueing. All icons are drawn
// procedurally from QPainter primitives (arcs/lines/polygons), never loaded
// from an icon font or external image asset.
class TacticalMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit TacticalMapWidget(QWidget* parent = nullptr);

    void setAssets(std::vector<Asset> assets);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawCompassRose(QPainter& painter, QPointF center, double radius) const;
    void drawWarningTriangle(QPainter& painter, QPointF center, double size) const;
    void drawAntennaGlyph(QPainter& painter, QPointF center, double size, double qualityPercent) const;

    std::vector<Asset> m_assets;
    double m_mapRangeKm = 100.0;
};

} // namespace qttutorial::defence
