// SPDX-License-Identifier: MIT
#include "FieldOperationsModel.h"

namespace qttutorial::agriculture::ops {

FieldOperationsModel::FieldOperationsModel(int fieldCount, QObject* parent)
    : QAbstractTableModel(parent)
    , m_fieldCount(fieldCount)
{
    m_samples.reserve(static_cast<std::size_t>(fieldCount));
    for (int i = 0; i < fieldCount; ++i) {
        m_samples.push_back(FieldOperationSimulator::sampleAt(i, 0.0));
    }

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &FieldOperationsModel::tick);
    m_timer.start();
}

void FieldOperationsModel::tick()
{
    m_elapsedSeconds += 1.0;
    for (int i = 0; i < m_fieldCount; ++i) {
        m_samples[static_cast<std::size_t>(i)] = FieldOperationSimulator::sampleAt(i, m_elapsedSeconds);
    }
    emit dataChanged(index(0, 0), index(m_fieldCount - 1, ColumnCount - 1));
    emit samplesUpdated();
}

int FieldOperationsModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_fieldCount;
}

int FieldOperationsModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant FieldOperationsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_fieldCount || role != Qt::DisplayRole) {
        return {};
    }
    const FieldSample& sample = m_samples[static_cast<std::size_t>(index.row())];
    switch (index.column()) {
    case VehicleColumn:
        return sample.vehicleId;
    case FieldColumn:
        return sample.fieldId;
    case CoverageColumn:
        return QStringLiteral("%1 %").arg(sample.coveragePercent, 0, 'f', 1);
    case StatusColumn:
        return sample.status;
    default:
        return {};
    }
}

QVariant FieldOperationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case VehicleColumn:
        return QStringLiteral("Vehicle");
    case FieldColumn:
        return QStringLiteral("Field");
    case CoverageColumn:
        return QStringLiteral("Pass coverage");
    case StatusColumn:
        return QStringLiteral("Status");
    default:
        return {};
    }
}

FieldSample FieldOperationsModel::sampleAt(int row) const
{
    if (row < 0 || row >= m_fieldCount) {
        return {};
    }
    return m_samples[static_cast<std::size_t>(row)];
}

} // namespace qttutorial::agriculture::ops
