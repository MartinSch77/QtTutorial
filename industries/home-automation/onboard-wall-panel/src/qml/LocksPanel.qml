// SPDX-License-Identifier: MIT
import QtQuick

Column {
    id: root

    required property var lockModel

    spacing: 24

    Grid {
        width: parent.width
        columns: 3
        columnSpacing: 16
        rowSpacing: 16

        Repeater {
            model: root.lockModel
            LockTile {
                width: (root.width - 32) / 3
                height: 110
                lockName: model.name
                locked: model.locked
                onToggled: (locked) => root.lockModel.setLocked(index, locked)
            }
        }
    }

    Column {
        width: parent.width
        spacing: 6

        Text {
            text: qsTr("Recent activity")
            color: "#9aa4b2"
            font.pixelSize: 14
            font.bold: true
        }
        Repeater {
            model: root.lockModel.recentActivity
            Text {
                text: modelData
                color: "#e6edf3"
                font.pixelSize: 13
            }
        }
    }
}
