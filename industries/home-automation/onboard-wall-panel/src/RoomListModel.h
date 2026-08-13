// SPDX-License-Identifier: MIT
#pragma once

#include "RoomRegistry.h"
#include "SceneRegistry.h"

#include <QAbstractListModel>
#include <QQmlEngine>

namespace qttutorial::homeautomation {

// QML-facing list model over RoomRegistry: one row per room, exposing the
// lighting and blind state the Lighting/Blinds panels bind to. All of the
// actual toggling/clamping logic lives in RoomRegistry, which has no Qt GUI
// dependency and is unit tested independent of this class.
class RoomListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
public:
    enum Role {
        NameRole = Qt::UserRole + 1,
        LightOnRole,
        BrightnessRole,
        BlindPositionRole,
    };

    explicit RoomListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void setLightOn(int row, bool on);
    Q_INVOKABLE void setBrightness(int row, int brightness);
    Q_INVOKABLE void setBlindPosition(int row, int position);
    // Applies a scene (see Scene/SceneRegistry) to every room at once - the
    // cross-room effect itself lives in SceneRegistry, this just re-renders.
    Q_INVOKABLE void applyScene(int sceneId);

private:
    RoomRegistry m_registry;
};

} // namespace qttutorial::homeautomation
