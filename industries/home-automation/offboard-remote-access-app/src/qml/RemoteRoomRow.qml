// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string roomName: ""
    property bool lightOn: false
    property int brightness: 0
    property int blindPosition: 0

    signal lightToggled(bool on)
    signal brightnessMoved(real value)
    signal blindMoved(real value)

    height: 96
    radius: 12
    color: "#1c212b"
    border.color: "#2a3140"
    border.width: 1

    Column {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 6

        Row {
            width: parent.width
            spacing: 6

            LightbulbIcon {
                width: 18
                height: 18
                anchors.verticalCenter: parent.verticalCenter
                on: root.lightOn
                brightness: root.brightness
            }
            Text {
                width: parent.width - 76
                anchors.verticalCenter: parent.verticalCenter
                text: root.roomName
                color: "#e6edf3"
                font.pixelSize: 15
                font.bold: true
            }
            ToggleSwitch {
                anchors.verticalCenter: parent.verticalCenter
                checked: root.lightOn
                onColor: "#f4c860"
                onToggled: (checked) => root.lightToggled(checked)
            }
        }

        Row {
            width: parent.width
            spacing: 8
            Text { text: qsTr("Light"); color: "#9aa4b2"; font.pixelSize: 11; width: 40; anchors.verticalCenter: parent.verticalCenter }
            SliderControl {
                width: parent.width - 48
                anchors.verticalCenter: parent.verticalCenter
                enabled: root.lightOn
                from: 0
                to: 100
                value: root.brightness
                accentColor: "#f4c860"
                onMoved: (value) => root.brightnessMoved(value)
            }
        }
        Row {
            width: parent.width
            spacing: 8
            Text { text: qsTr("Shade"); color: "#9aa4b2"; font.pixelSize: 11; width: 40; anchors.verticalCenter: parent.verticalCenter }
            SliderControl {
                width: parent.width - 48
                anchors.verticalCenter: parent.verticalCenter
                from: 0
                to: 100
                value: root.blindPosition
                accentColor: "#39c0ff"
                onMoved: (value) => root.blindMoved(value)
            }
        }
    }
}
