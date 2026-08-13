// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick

// Visualizes OrderLifecycle.currentState as a stepper (Draft -> Submitted ->
// Filled) or, for the two terminal failure states, a single colored badge.
Item {
    id: root
    required property string state
    height: 28

    readonly property var happyPath: ["Draft", "Submitted", "Filled"]
    readonly property bool isTerminalFailure: root.state === "Cancelled" || root.state === "Rejected"
    readonly property int currentStepIndex: happyPath.indexOf(root.state)

    implicitWidth: isTerminalFailure ? failureBadge.implicitWidth : stepper.implicitWidth

    Row {
        id: stepper
        visible: !root.isTerminalFailure
        spacing: 6
        anchors.verticalCenter: parent.verticalCenter

        Repeater {
            model: root.happyPath
            delegate: Row {
                spacing: 6
                Rectangle {
                    width: 22
                    height: 22
                    radius: 11
                    anchors.verticalCenter: parent.verticalCenter
                    color: index < root.currentStepIndex ? "#3ddc84"
                         : index === root.currentStepIndex ? "#4ac1e0"
                         : "#2a3040"
                    border.color: "#101418"
                    Text {
                        anchors.centerIn: parent
                        text: index + 1
                        color: "white"
                        font.pixelSize: 11
                        font.bold: true
                    }
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData
                    color: index === root.currentStepIndex ? "white" : "#8fa0b3"
                    font.pixelSize: 12
                    font.bold: index === root.currentStepIndex
                }
                Rectangle {
                    visible: index < root.happyPath.length - 1
                    width: 18
                    height: 2
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#2a3040"
                }
            }
        }
    }

    Rectangle {
        id: failureBadge
        visible: root.isTerminalFailure
        anchors.verticalCenter: parent.verticalCenter
        implicitWidth: failureLabel.implicitWidth + 16
        height: 24
        radius: 4
        color: root.state === "Cancelled" ? "#5a6b80" : "#e5484d"

        Text {
            id: failureLabel
            anchors.centerIn: parent
            text: root.state
            color: "white"
            font.pixelSize: 12
            font.bold: true
        }
    }
}
