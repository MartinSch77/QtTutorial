// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string roomName: ""
    property bool lightOn: false
    property int brightness: 0

    signal toggled(bool on)
    signal brightnessMoved(real value)

    radius: 14
    color: "#1c212b"
    border.color: lightOn ? "#f4c860" : "#2a3140"
    border.width: 1

    Behavior on border.color { ColorAnimation { duration: 200 } }

    Column {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 12

        Row {
            width: parent.width
            spacing: 8

            LightbulbIcon {
                width: 22
                height: 22
                anchors.verticalCenter: parent.verticalCenter
                on: root.lightOn
                brightness: root.brightness
            }
            Text {
                width: parent.width - 88
                anchors.verticalCenter: parent.verticalCenter
                text: root.roomName
                color: "#e6edf3"
                font.pixelSize: 18
                font.bold: true
            }
            ToggleSwitch {
                anchors.verticalCenter: parent.verticalCenter
                checked: root.lightOn
                onColor: "#f4c860"
                onToggled: root.toggled(checked)
            }
        }

        Text {
            text: root.lightOn ? qsTr("Brightness %1%").arg(root.brightness) : qsTr("Light off")
            color: "#9aa4b2"
            font.pixelSize: 13
        }

        SliderControl {
            width: parent.width
            enabled: root.lightOn
            from: 0
            to: 100
            value: root.brightness
            accentColor: "#f4c860"
            onMoved: root.brightnessMoved(value)
        }
    }
}
