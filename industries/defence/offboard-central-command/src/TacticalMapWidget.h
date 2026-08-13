// SPDX-License-Identifier: MIT
#pragma once

#include "Asset.h"

#include <QWidget>

#include <vector>

namespace qttutorial::defence {

// A plain top-down 2D tactical map (QPainter-drawn, no real map tiles or
// imagery): a range-ring grid with a colored icon and id label per asset,
// colored by health. This is a passive plot of last-known positions - there
// is no engagement geometry, no weapons range ring, and no target cueing.
class TacticalMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit TacticalMapWidget(QWidget* parent = nullptr);

    void setAssets(std::vector<Asset> assets);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<Asset> m_assets;
    double m_mapRangeKm = 100.0;
};

} // namespace qttutorial::defence
