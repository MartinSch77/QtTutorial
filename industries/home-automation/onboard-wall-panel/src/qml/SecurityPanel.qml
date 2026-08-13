// SPDX-License-Identifier: MIT
import QtQuick

Column {
    id: root

    required property var securityModel

    spacing: 20

    Row {
        width: parent.width
        spacing: 16

        Text {
            width: parent.width - toggle.width - 16
            text: root.securityModel.armed
                  ? (root.securityModel.breach ? qsTr("ARMED - BREACH DETECTED") : qsTr("Armed"))
                  : qsTr("Disarmed")
            color: root.securityModel.breach ? "#c0392b" : (root.securityModel.armed ? "#3ddc6f" : "#9aa4b2")
            font.pixelSize: 18
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            height: toggle.height
        }
        ToggleSwitch {
            id: toggle
            checked: root.securityModel.armed
            onColor: "#3ddc6f"
            onToggled: (checked) => root.securityModel.armed = checked
        }
    }

    Grid {
        width: parent.width
        columns: 2
        columnSpacing: 16
        rowSpacing: 16

        Repeater {
            model: root.securityModel
            SecurityTile {
                width: (root.width - 16) / 2
                height: 70
                sensorName: model.name
                triggered: model.triggered
            }
        }
    }
}
