// SPDX-License-Identifier: MIT
#include "IndicatorListModel.h"

namespace qttutorial::qml_basics {

IndicatorListModel::IndicatorListModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_readings(IndicatorGenerator::generate(0))
{
}

int IndicatorListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_readings.size());
}

QVariant IndicatorListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_readings.size())) {
        return {};
    }
    const IndicatorReading& reading = m_readings[static_cast<std::size_t>(index.row())];
    switch (role) {
    case NameRole:
        return reading.name;
    case ValueRole:
        return reading.value;
    case StatusRole:
        switch (reading.status) {
        case IndicatorStatus::Nominal:
            return QStringLiteral("nominal");
        case IndicatorStatus::Warning:
            return QStringLiteral("warning");
        case IndicatorStatus::Critical:
            return QStringLiteral("critical");
        }
        return {};
    default:
        return {};
    }
}

QHash<int, QByteArray> IndicatorListModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {ValueRole, "value"},
        {StatusRole, "status"},
    };
}

void IndicatorListModel::refresh(int tick)
{
    m_readings = IndicatorGenerator::generate(tick);
    if (!m_readings.empty()) {
        emit dataChanged(index(0), index(static_cast<int>(m_readings.size()) - 1));
    }
}

} // namespace qttutorial::qml_basics
