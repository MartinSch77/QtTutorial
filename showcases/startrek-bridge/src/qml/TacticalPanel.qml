// SPDX-License-Identifier: MIT
import QtQuick

// Tactical/Science station: simulated EXTERNAL condition data - ambient
// radiation and hull-stress readings, a purely cosmetic shield-strength
// percentage (not a real defensive system), and a sensor-scan readout of a
// few simulated detected external objects. All fictional/cosmetic flavour
// data, not a real detection or targeting system. Reflows from side-by-side
// (readouts | scan list) on wide layouts to stacked on narrow ones.
Item {
    id: root
    property var bridge
    readonly property bool wide: width >= 560

    Column {
        id: stats
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 16
        width: root.wide ? 220 : parent.width - 32
        spacing: 16

        Text { text: qsTr("TACTICAL / SCIENCE"); color: "#ff9c00"; font.pixelSize: 18; font.bold: true }

        RadialGauge {
            width: 140
            height: 140
            value: root.bridge ? root.bridge.shieldStrengthPercent : 0
            minValue: 0
            maxValue: 100
            label: qsTr("SHIELD STRENGTH")
            valueText: root.bridge ? root.bridge.shieldStrengthPercent.toFixed(0) + "%" : "0%"
            accentColor: "#c0392b"
        }

        Row {
            spacing: 24
            Column {
                spacing: 2
                Text { text: qsTr("EXT. RADIATION"); color: "#9c8fae"; font.pixelSize: 12 }
                Text {
                    text: root.bridge ? root.bridge.externalRadiationLevel.toFixed(2) + " mRad/s" : "0.00 mRad/s"
                    color: "#c6a0ff"
                    font.pixelSize: 16
                    font.bold: true
                }
            }
            Column {
                spacing: 2
                Text { text: qsTr("HULL STRESS"); color: "#9c8fae"; font.pixelSize: 12 }
                Text {
                    text: root.bridge ? root.bridge.hullStressLevel.toFixed(0) + " %" : "0 %"
                    color: "#c6a0ff"
                    font.pixelSize: 16
                    font.bold: true
                }
            }
        }
    }

    Rectangle {
        id: scanPanel
        anchors.left: root.wide ? stats.right : parent.left
        anchors.right: parent.right
        anchors.top: root.wide ? parent.top : stats.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: root.wide ? 24 : 16
        anchors.rightMargin: 16
        anchors.topMargin: 16
        anchors.bottomMargin: 16
        radius: 8
        color: "#0a0710"
        border.color: "#3a2f47"

        Column {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 10

            Text { text: qsTr("SENSOR SCAN"); color: "#9c8fae"; font.pixelSize: 13 }

            ListView {
                width: parent.width
                height: parent.height - 30
                clip: true
                spacing: 8
                model: root.bridge ? root.bridge.sensorContacts : []
                delegate: Rectangle {
                    width: ListView.view.width
                    height: 48
                    radius: 6
                    color: "#15111a"

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        spacing: 16

                        Text { text: modelData.label; color: "#f2e9ff"; font.pixelSize: 13; font.bold: true }
                        Text {
                            text: qsTr("RANGE %1 km").arg(modelData.distanceKm.toFixed(1))
                            color: "#c6a0ff"
                            font.pixelSize: 12
                        }
                        Text {
                            text: qsTr("BEARING %1°").arg(modelData.bearingDeg.toFixed(0))
                            color: "#c6a0ff"
                            font.pixelSize: 12
                        }
                    }
                }
            }
        }
    }
}
