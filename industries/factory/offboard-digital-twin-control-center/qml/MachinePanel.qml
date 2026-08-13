// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Right-hand live machine info/controls panel - step 3's "inspect the
// machine" (defective-part callout) and the entry point into step 5's
// maintenance workflow both live here.
Rectangle {
    id: root
    color: Theme.glassSurface
    border.color: Theme.border
    border.width: 1
    radius: Theme.radius

    property var machine // {machineId, name, kind, temperature, vibration, speed, state, defectivePart}
    property bool thermalOverlay: false

    signal thermalOverlayToggled(bool enabled)
    signal startMaintenanceRequested()

    Column {
        anchors.fill: parent
        anchors.margins: Theme.spacingUnit
        spacing: Theme.spacingUnit

        Text {
            text: root.machine ? root.machine.name : qsTr("No machine selected")
            color: Theme.textPrimary
            font.pixelSize: Theme.fontSizeHeading
            font.bold: true
        }

        Rectangle {
            visible: root.machine !== null && root.machine !== undefined
            width: parent.width
            height: 26
            radius: 13
            color: Qt.rgba(Theme.colorForState(root.machine ? root.machine.state : "normal").r,
                            Theme.colorForState(root.machine ? root.machine.state : "normal").g,
                            Theme.colorForState(root.machine ? root.machine.state : "normal").b, 0.2)
            Behavior on color { ColorAnimation { duration: Theme.transitionMs } }
            Text {
                anchors.centerIn: parent
                text: root.machine ? root.machine.state.toUpperCase() : ""
                color: Theme.colorForState(root.machine ? root.machine.state : "normal")
                font.bold: true
                font.pixelSize: Theme.fontSizeBody - 2
            }
        }

        Grid {
            visible: root.machine !== null && root.machine !== undefined
            columns: 2
            columnSpacing: Theme.spacingUnit
            rowSpacing: 4
            Text { text: qsTr("Temperature"); color: Theme.textSecondary; font.pixelSize: Theme.fontSizeBody - 1 }
            Text {
                text: root.machine ? root.machine.temperature.toFixed(1) + " degC" : ""
                color: Theme.textPrimary; font.pixelSize: Theme.fontSizeBody
            }
            Text { text: qsTr("Vibration"); color: Theme.textSecondary; font.pixelSize: Theme.fontSizeBody - 1 }
            Text {
                text: root.machine ? root.machine.vibration.toFixed(2) + " mm/s" : ""
                color: Theme.textPrimary; font.pixelSize: Theme.fontSizeBody
            }
            Text { text: qsTr("Speed"); color: Theme.textSecondary; font.pixelSize: Theme.fontSizeBody - 1 }
            Text {
                text: root.machine ? root.machine.speed.toFixed(0) + " RPM" : ""
                color: Theme.textPrimary; font.pixelSize: Theme.fontSizeBody
            }
        }

        Rectangle {
            visible: root.machine && root.machine.defectivePart !== ""
            width: parent.width
            height: defectColumn.implicitHeight + 16
            radius: Theme.radius - 2
            color: Qt.rgba(Theme.accentAlarm.r, Theme.accentAlarm.g, Theme.accentAlarm.b, 0.15)
            border.color: Theme.accentAlarm
            border.width: 1

            Column {
                id: defectColumn
                anchors.fill: parent
                anchors.margins: 8
                spacing: 2
                Text { text: qsTr("Defective part identified"); color: Theme.accentAlarm; font.bold: true; font.pixelSize: Theme.fontSizeBody - 1 }
                Text {
                    text: root.machine ? root.machine.defectivePart : ""
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeBody - 1
                    wrapMode: Text.WordWrap
                    width: parent.width
                }
            }
        }

        Switch {
            text: qsTr("Thermal overlay")
            checked: root.thermalOverlay
            onToggled: root.thermalOverlayToggled(checked)
        }

        Button {
            text: qsTr("Start maintenance workflow")
            enabled: root.machine !== null && root.machine !== undefined
            onClicked: root.startMaintenanceRequested()
        }
    }
}
