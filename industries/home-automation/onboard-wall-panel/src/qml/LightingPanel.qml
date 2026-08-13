// SPDX-License-Identifier: MIT
import QtQuick

Grid {
    id: root

    required property var roomModel

    columns: 2
    columnSpacing: 16
    rowSpacing: 16

    Repeater {
        model: root.roomModel
        RoomLightCard {
            width: (root.width - root.columnSpacing) / 2
            height: 130
            roomName: model.name
            lightOn: model.lightOn
            brightness: model.brightness
            onToggled: (on) => root.roomModel.setLightOn(index, on)
            onBrightnessMoved: (value) => root.roomModel.setBrightness(index, Math.round(value))
        }
    }
}
