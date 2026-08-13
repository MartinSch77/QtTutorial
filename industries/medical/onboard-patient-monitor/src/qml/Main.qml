// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.Medical.Onboard

// Kiosk-style bedside patient monitor. In a real embedded deployment this
// window would be shown with visibility: Window.FullScreen and no window
// decorations; it is left windowed here so the demo runs on a regular desktop.
//
// Design reference: the dark background, large colour-coded waveform traces
// (ECG in green, respiration in yellow), boxed numeric vitals, and an
// urgency-coloured alarm banner are stylistic homage to the general visual
// language of bedside patient monitors such as the Philips IntelliVue
// MX-series / GE Healthcare CARESCAPE family - genre inspiration only. No
// trademarked logo, wordmark, or exact layout from any vendor is reproduced;
// every icon and waveform on screen is drawn procedurally at paint time. See
// README.md, "Design reference".
Window {
    id: window
    width: 1000
    height: 720
    visible: true
    color: "#0d1117"
    title: qsTr("QtTutorial - Patient Monitor")

    PatientMonitor {
        id: monitor
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Rectangle {
            id: alarmBanner
            width: parent.width
            height: 44
            radius: 6
            visible: monitor.alarmLevelText !== "Normal"
            color: monitor.alarmLevelText === "Critical" ? "#c0392b" : "#d4a12b"

            Row {
                anchors.centerIn: parent
                spacing: 10

                ClinicalIcon {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 24
                    height: 24
                    kind: "bell"
                    color: "white"
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    color: "white"
                    font.bold: true
                    font.pixelSize: 15
                    text: monitor.alarmLevelText === "Critical"
                          ? qsTr("CRITICAL: patient vitals require immediate attention")
                          : qsTr("CAUTION: patient vitals outside normal range")
                }
            }

            SequentialAnimation on opacity {
                running: monitor.alarmLevelText === "Critical"
                loops: Animation.Infinite
                NumberAnimation { from: 1.0; to: 0.4; duration: 400 }
                NumberAnimation { from: 0.4; to: 1.0; duration: 400 }
            }
        }

        Row {
            width: parent.width
            spacing: 8
            visible: !alarmBanner.visible
            height: alarmBanner.height

            Rectangle {
                width: parent.width
                height: parent.height
                radius: 6
                color: "#16311f"
                border.color: "#3ddc6f"

                Row {
                    anchors.centerIn: parent
                    spacing: 10
                    ClinicalIcon {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 22; height: 22
                        kind: "bell"
                        color: "#3ddc6f"
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#3ddc6f"
                        font.bold: true
                        font.pixelSize: 15
                        text: qsTr("NORMAL: patient vitals within expected range")
                    }
                }
            }
        }

        Row {
            width: parent.width
            spacing: 8
            height: 20

            ClinicalIcon { width: 18; height: 18; kind: "heartbeat"; color: "#3ddc6f" }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: "#9aa4b2"
                font.pixelSize: 13
                text: qsTr("ECG - Lead II (simulated)")
            }
        }

        EcgCanvas {
            width: parent.width
            height: 150
            samples: monitor.ecgSamples
        }

        Row {
            width: parent.width
            spacing: 8
            height: 20

            ClinicalIcon { width: 18; height: 18; kind: "respiration"; color: "#e0c341" }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: "#9aa4b2"
                font.pixelSize: 13
                text: qsTr("Respiration (simulated)")
            }
        }

        RespirationCanvas {
            width: parent.width
            height: 90
            samples: monitor.respirationSamples
        }

        Row {
            width: parent.width
            spacing: 16

            VitalTile {
                width: (parent.width - 3 * 16) / 4
                height: 100
                label: qsTr("Heart rate (bpm)")
                value: Math.round(monitor.heartRate).toString()
                accentColor: "#3ddc6f"
                iconKind: "heartbeat"
            }
            VitalTile {
                width: (parent.width - 3 * 16) / 4
                height: 100
                label: qsTr("SpO2 (%)")
                value: Math.round(monitor.spo2).toString()
                accentColor: "#39c0ff"
                iconKind: "droplet"
            }
            VitalTile {
                width: (parent.width - 3 * 16) / 4
                height: 100
                label: qsTr("Blood pressure (mmHg)")
                value: Math.round(monitor.systolic) + "/" + Math.round(monitor.diastolic)
                accentColor: "#ff9f43"
                iconKind: "cuff"
            }
            VitalTile {
                width: (parent.width - 3 * 16) / 4
                height: 100
                label: qsTr("Resp. rate / Temp.")
                value: Math.round(monitor.respirationRate) + " / " + monitor.temperature.toFixed(1) + "°C"
                accentColor: "#e0c341"
                iconKind: "respiration"
            }
        }
    }
}
