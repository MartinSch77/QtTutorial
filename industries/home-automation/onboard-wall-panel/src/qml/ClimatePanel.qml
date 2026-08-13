// SPDX-License-Identifier: MIT
import QtQuick

Column {
    id: root

    required property var thermostat

    spacing: 24

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        ThermostatGauge {
            currentTemperature: root.thermostat.currentTemperature
            targetTemperature: root.thermostat.targetTemperature
            accentColor: root.thermostat.mode === 1 ? "#ff9f43" : (root.thermostat.mode === 2 ? "#39c0ff" : "#4a5568")
        }
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 12

        Repeater {
            model: [
                {label: qsTr("Off"), mode: 0},
                {label: qsTr("Heat"), mode: 1},
                {label: qsTr("Cool"), mode: 2},
            ]
            Rectangle {
                width: 90
                height: 40
                radius: 8
                color: root.thermostat.mode === modelData.mode ? "#39c0ff" : "#1c212b"
                border.color: "#2a3140"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: modelData.label
                    color: root.thermostat.mode === modelData.mode ? "#0d1117" : "#e6edf3"
                    font.bold: true
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.thermostat.mode = modelData.mode
                }
            }
        }
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 12

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Target")
            color: "#9aa4b2"
            font.pixelSize: 14
        }
        SliderControl {
            width: 220
            anchors.verticalCenter: parent.verticalCenter
            from: 10
            to: 30
            value: root.thermostat.targetTemperature
            accentColor: "#ff9f43"
            onMoved: (value) => root.thermostat.targetTemperature = Math.round(value * 2) / 2
        }
    }
}
