// SPDX-License-Identifier: MIT
#pragma once

#include "PlantOverviewModel.h"

#include <QRectF>
#include <QWidget>

class QPainter;

namespace qttutorial::plant_scada {

// A plant-wide, status-at-a-glance overview: one small hand-drawn P&ID-style
// mimic (tank, valve, conveyor, motor glyphs) per plant line/cell, coloured
// by run/stop state and worst alarm severity, mirroring the "everything on
// one screen" overview a real SCADA client opens on. Hand-drawn with
// QPainter, the same approach TrendWidget already uses (no Qt Charts / Qt
// Graphs / external icon assets), driven by a PlantOverviewModel.
class PlantOverviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit PlantOverviewWidget(QWidget* parent = nullptr);

    void setModel(PlantOverviewModel* model);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawLineMimic(QPainter& painter, const QRectF& cell, const LineStatus& status) const;

    PlantOverviewModel* m_model = nullptr;
};

} // namespace qttutorial::plant_scada
