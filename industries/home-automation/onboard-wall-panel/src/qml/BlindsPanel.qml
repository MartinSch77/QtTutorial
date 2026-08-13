// SPDX-License-Identifier: MIT
import QtQuick

Column {
    id: root

    required property var roomModel

    spacing: 16

    Repeater {
        model: root.roomModel
        RoomBlindCard {
            width: root.width
            height: 100
            roomName: model.name
            position: model.blindPosition
            onPositionMoved: (value) => root.roomModel.setBlindPosition(index, Math.round(value))
        }
    }
}
