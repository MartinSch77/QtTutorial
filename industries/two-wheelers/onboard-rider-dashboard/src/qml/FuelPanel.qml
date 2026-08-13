// SPDX-License-Identifier: MIT
import QtQuick

// Fuel-level readout: a small analogue-style FuelGaugeIcon plus a numeric
// litres/percent readout and a bar, the digital-cluster equivalent of the
// idiot-light fuel gauge on a real bike dash. Fuel level is fed live from
// RideCycleSimulator's rpm/mode-correlated burn-rate model (see
// RideCycleSimulator::fuelBurnRateLitresPerHour), not independent noise.
Rectangle {
    id: root

    property real fuelLitres: 0
    property real fuelPercent: 100
    property real tankCapacityLitres: 15

    radius: 8
    color: "#1c212b"

    readonly property color levelColor: root.fuelPercent < 15 ? "#ff5a5a" : "#39c0ff"

    Row {
        anchors.centerIn: parent
        spacing: 10

        FuelGaugeIcon {
            width: root.height * 0.7
            height: width
            anchors.verticalCenter: parent.verticalCenter
            percent: root.fuelPercent
        }

        Column {
            spacing: 4
            anchors.verticalCenter: parent.verticalCenter

            Text {
                text: qsTr("FUEL")
                color: "#9aa4b2"
                font.pixelSize: 12
                font.bold: true
            }

            Text {
                text: root.fuelLitres.toFixed(1) + " / " + root.tankCapacityLitres.toFixed(0) + " L"
                color: "#f2f4f8"
                font.pixelSize: 13
                font.bold: true
            }

            Rectangle {
                width: 90
                height: 6
                radius: 3
                color: "#2a2f3a"

                Rectangle {
                    width: parent.width * Math.max(0, Math.min(1, root.fuelPercent / 100))
                    height: parent.height
                    radius: 3
                    color: root.levelColor
                }
            }
        }
    }
}
