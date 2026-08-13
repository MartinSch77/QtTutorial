// SPDX-License-Identifier: MIT
#pragma once

#include "FieldOperationSimulator.h"

#include <QAbstractTableModel>
#include <QTimer>

#include <vector>

namespace qttutorial::agriculture::ops {

// A QAbstractTableModel fed by FieldOperationSimulator on a QTimer, standing in
// for a farm-management back office aggregating many fields/vehicles at once.
class FieldOperationsModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { VehicleColumn, FieldColumn, CoverageColumn, StatusColumn, ColumnCount };

    explicit FieldOperationsModel(int fieldCount = 6, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] FieldSample sampleAt(int row) const;

signals:
    void samplesUpdated();

private:
    void tick();

    int m_fieldCount;
    double m_elapsedSeconds = 0.0;
    std::vector<FieldSample> m_samples;
    QTimer m_timer;
};

} // namespace qttutorial::agriculture::ops
