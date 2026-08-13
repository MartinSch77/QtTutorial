// SPDX-License-Identifier: MIT
#pragma once

#include "FieldOperationSimulator.h"

#include <QWidget>

#include <vector>

namespace qttutorial::agriculture::ops {

// A farm-wide "which fields are done / in progress / not started" overview
// strip, painted with QPainter (no charting library) as a row of tiles, one
// per field, colour-coded by FieldSample::overallStatus. This is the
// control-room counterpart to the per-field FieldMapWidget: instead of one
// field's position, it shows the whole operation at a glance, the way a farm
// manager would want to see it before drilling into any single field.
class FieldStatusOverviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit FieldStatusOverviewWidget(QWidget* parent = nullptr);

    void setSamples(const std::vector<FieldSample>& samples);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<FieldSample> m_samples;
};

} // namespace qttutorial::agriculture::ops
