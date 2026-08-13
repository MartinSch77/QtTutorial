// SPDX-License-Identifier: MIT
#include "SecurityListModel.h"

namespace qttutorial::homeautomation {

namespace {
constexpr int kBlipIntervalMs = 2500;
}

SecurityListModel::SecurityListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    m_timer.setInterval(kBlipIntervalMs);
    connect(&m_timer, &QTimer::timeout, this, &SecurityListModel::simulateSensorBlip);
    m_timer.start();
}

int SecurityListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_center.sensorCount();
}

QVariant SecurityListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_center.sensorCount()) {
        return {};
    }
    const Sensor& sensor = m_center.sensor(index.row());
    switch (role) {
    case NameRole:
        return sensor.name;
    case TriggeredRole:
        return sensor.triggered;
    default:
        return {};
    }
}

QHash<int, QByteArray> SecurityListModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {TriggeredRole, "triggered"},
    };
}

void SecurityListModel::setArmed(bool armed)
{
    if (m_center.armed() == armed) {
        return;
    }
    m_center.setArmed(armed);
    emit armedChanged();
    emit breachChanged();
}

void SecurityListModel::applyScene(int sceneId)
{
    setArmed(SceneRegistry::definition(static_cast<Scene>(sceneId)).armSecurity);
}

void SecurityListModel::simulateSensorBlip()
{
    if (m_activeSensor != -1) {
        m_center.setSensorTriggered(m_activeSensor, false);
        m_activeSensor = -1;
    }

    ++m_tick;
    // Every fifth tick stays quiet so the panel does not feel like it is
    // triggering constantly; the rest cycle a single sensor's state.
    if (m_tick % 5 != 4) {
        m_activeSensor = m_tick % m_center.sensorCount();
        m_center.setSensorTriggered(m_activeSensor, true);
    }

    emit dataChanged(index(0), index(m_center.sensorCount() - 1), {TriggeredRole});
    emit breachChanged();
}

} // namespace qttutorial::homeautomation
