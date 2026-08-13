// SPDX-License-Identifier: MIT
#pragma once

#include "HaulFleetSimulator.h"

#include <QAbstractTableModel>
#include <QTimer>

#include <vector>

namespace qttutorial::mining::pit {

// A QAbstractTableModel fed by HaulFleetSimulator on a QTimer, standing in for
// a pit-operations telemetry-ingestion service pushing updates into a control
// room.
class PitFleetModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { IdColumn, StateColumn, PayloadColumn, SpeedColumn, FuelColumn, LocationColumn, ColumnCount };

    explicit PitFleetModel(int truckCount = 6, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] TruckSample sampleAt(int row) const;
    [[nodiscard]] const std::vector<TruckSample>& samples() const { return m_samples; }

signals:
    void samplesUpdated();

private:
    void tick();

    int m_truckCount;
    double m_elapsedSeconds = 0.0;
    std::vector<TruckSample> m_samples;
    QTimer m_timer;
};

} // namespace qttutorial::mining::pit
