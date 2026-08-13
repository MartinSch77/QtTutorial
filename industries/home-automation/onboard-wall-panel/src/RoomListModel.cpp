// SPDX-License-Identifier: MIT
#include "RoomListModel.h"

namespace qttutorial::homeautomation {

RoomListModel::RoomListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int RoomListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_registry.count();
}

QVariant RoomListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_registry.count()) {
        return {};
    }
    const Room& room = m_registry.room(index.row());
    switch (role) {
    case NameRole:
        return room.name;
    case LightOnRole:
        return room.lightOn;
    case BrightnessRole:
        return room.brightness;
    case BlindPositionRole:
        return room.blindPosition;
    default:
        return {};
    }
}

QHash<int, QByteArray> RoomListModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {LightOnRole, "lightOn"},
        {BrightnessRole, "brightness"},
        {BlindPositionRole, "blindPosition"},
    };
}

void RoomListModel::setLightOn(int row, bool on)
{
    m_registry.setLightOn(row, on);
    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {LightOnRole});
}

void RoomListModel::setBrightness(int row, int brightness)
{
    m_registry.setBrightness(row, brightness);
    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {BrightnessRole});
}

void RoomListModel::setBlindPosition(int row, int position)
{
    m_registry.setBlindPosition(row, position);
    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {BlindPositionRole});
}

void RoomListModel::applyScene(int sceneId)
{
    SceneRegistry::applyToRooms(static_cast<Scene>(sceneId), m_registry);
    if (m_registry.count() == 0) {
        return;
    }
    emit dataChanged(index(0), index(m_registry.count() - 1), {LightOnRole, BrightnessRole});
}

} // namespace qttutorial::homeautomation
