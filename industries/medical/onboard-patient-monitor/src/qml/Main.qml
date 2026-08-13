// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.Medical.Onboard

// Kiosk-style bedside patient monitor. In a real embedded deployment this
// window would be shown with visibility: Window.FullScreen and no window
// decorations; it is left windowed here so the demo runs on a regular desktop.
Window {
    id: window
    width: 900
    height: 500
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
            height: 36
            radius: 6
            visible: monitor.alarmLevelText !== "Normal"
            color: monitor.alarmLevelText === "Critical" ? "#c0392b" : "#d4a12b"

            Text {
                anchors.centerIn: parent
                color: "white"
                font.bold: true
                text: monitor.alarmLevelText === "Critical"
                      ? qsTr("CRITICAL: patient vitals require immediate attention")
                      : qsTr("WARNING: patient vitals outside normal range")
            }

            SequentialAnimation on opacity {
                running: monitor.alarmLevelText === "Critical"
                loops: Animation.Infinite
                NumberAnimation { from: 1.0; to: 0.4; duration: 400 }
                NumberAnimation { from: 0.4; to: 1.0; duration: 400 }
            }
        }

        EcgCanvas {
            width: parent.width
            height: 220
            samples: monitor.ecgSamples
        }

        Row {
            width: parent.width
            spacing: 16

            VitalTile {
                width: (parent.width - 32) / 3
                height: 100
                label: qsTr("Heart rate (bpm)")
                value: Math.round(monitor.heartRate).toString()
                accentColor: "#3ddc6f"
            }
            VitalTile {
                width: (parent.width - 32) / 3
                height: 100
                label: qsTr("SpO2 (%)")
                value: Math.round(monitor.spo2).toString()
                accentColor: "#39c0ff"
            }
            VitalTile {
                width: (parent.width - 32) / 3
                height: 100
                label: qsTr("Blood pressure (mmHg)")
                value: Math.round(monitor.systolic) + "/" + Math.round(monitor.diastolic)
                accentColor: "#ff9f43"
            }
        }
    }
}
