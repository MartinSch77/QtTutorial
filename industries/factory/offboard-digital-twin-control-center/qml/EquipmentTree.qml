// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Left-hand equipment/navigation panel - a flat list grouped by state is
// "tree enough" for a four-machine demo floor without inventing a fake
// multi-level hierarchy; selecting a row drives the same
// machineClicked(machineId) path clicking the machine in the 3D scene does.
Rectangle {
    id: root
    color: Theme.glassSurface
    border.color: Theme.border
    border.width: 1
    radius: Theme.radius

    property var machineModel
    property int selectedMachineId: -1

    signal machineSelected(int machineId)

    Column {
        anchors.fill: parent
        anchors.margins: Theme.spacingUnit
        spacing: Theme.spacingUnit / 2

        Text {
            text: qsTr("EQUIPMENT")
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSizeBody - 1
            font.letterSpacing: 1.5
            font.bold: true
        }

        Repeater {
            model: root.machineModel
            delegate: Rectangle {
                id: row
                required property var model
                width: parent ? parent.width : 0
                height: 48
                radius: Theme.radius - 2
                color: model.machineId === root.selectedMachineId
                       ? Qt.rgba(Theme.accentNormal.r, Theme.accentNormal.g, Theme.accentNormal.b, 0.18)
                       : "transparent"
                Behavior on color { ColorAnimation { duration: Theme.transitionMs } }

                Row {
                    anchors.fill: parent
                    anchors.margins: 6
                    spacing: 8

                    Rectangle {
                        width: 10; height: 10; radius: 5
                        anchors.verticalCenter: parent.verticalCenter
                        color: Theme.colorForState(row.model.state)
                        Behavior on color { ColorAnimation { duration: Theme.transitionMs } }
                    }

                    Column {
                        Text { text: row.model.name; color: Theme.textPrimary; font.pixelSize: Theme.fontSizeBody }
                        Text {
                            text: row.model.temperature.toFixed(1) + " degC  |  " + row.model.vibration.toFixed(2) + " mm/s"
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSizeBody - 3
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.machineSelected(row.model.machineId)
                }
            }
        }
    }
}
