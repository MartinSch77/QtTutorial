// SPDX-License-Identifier: MIT
import QtQuick

// Small front/rear tyre pressure and temperature readout, the kind of panel a
// modern motorcycle TPMS (tyre pressure monitoring system) surfaces on-dash.
Rectangle {
    id: root

    property real frontPressureBar: 0
    property real rearPressureBar: 0
    property real frontTempC: 0
    property real rearTempC: 0

    radius: 8
    color: "#1c212b"

    Column {
        anchors.centerIn: parent
        spacing: 6

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("TYRES")
            color: "#9aa4b2"
            font.pixelSize: 12
            font.bold: true
        }

        Row {
            spacing: 18

            Column {
                spacing: 2
                Text { text: qsTr("FRONT"); color: "#9aa4b2"; font.pixelSize: 10 }
                Text {
                    text: root.frontPressureBar.toFixed(2) + " bar"
                    color: "#f2f4f8"
                    font.pixelSize: 13
                    font.bold: true
                }
                Text {
                    text: root.frontTempC.toFixed(0) + " °C"
                    color: "#f2f4f8"
                    font.pixelSize: 13
                }
            }

            Column {
                spacing: 2
                Text { text: qsTr("REAR"); color: "#9aa4b2"; font.pixelSize: 10 }
                Text {
                    text: root.rearPressureBar.toFixed(2) + " bar"
                    color: "#f2f4f8"
                    font.pixelSize: 13
                    font.bold: true
                }
                Text {
                    text: root.rearTempC.toFixed(0) + " °C"
                    color: "#f2f4f8"
                    font.pixelSize: 13
                }
            }
        }
    }
}
