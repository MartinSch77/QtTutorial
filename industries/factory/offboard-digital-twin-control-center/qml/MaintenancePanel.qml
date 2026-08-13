// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Step 5: "Diagnose -> Stop machine -> Replace part -> Restart" with
// per-step progress and cancel/retry/error-recovery, bound to the C++
// MaintenanceWorkflow (a hand-rolled QStateMachine - see its header comment
// for why Qt6::TaskTree was not used).
Rectangle {
    id: root
    color: Theme.glassSurface
    border.color: Theme.border
    border.width: 1
    radius: Theme.radius
    visible: workflow !== null && workflow !== undefined

    property var workflow

    readonly property var stepNames: [qsTr("Diagnosing"), qsTr("Stopping"), qsTr("Replacing part"), qsTr("Restarting")]

    Column {
        anchors.fill: parent
        anchors.margins: Theme.spacingUnit
        spacing: Theme.spacingUnit / 2

        Text {
            text: qsTr("MAINTENANCE WORKFLOW")
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSizeBody - 1
            font.bold: true
            font.letterSpacing: 1.5
        }

        Row {
            spacing: 6
            Repeater {
                model: root.stepNames
                delegate: Rectangle {
                    required property string modelData
                    required property int index
                    width: 90; height: 28
                    radius: 6
                    color: {
                        if (!root.workflow) return Theme.border;
                        if (root.workflow.inError && root.workflow.stepIndex - 1 === index) return Theme.accentAlarm;
                        if (root.workflow.stepIndex - 1 > index || root.workflow.stepIndex === 5) return Theme.accentNormal;
                        if (root.workflow.stepIndex - 1 === index) return Theme.accentWarning;
                        return Theme.border;
                    }
                    Behavior on color { ColorAnimation { duration: Theme.transitionMs } }
                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        font.pixelSize: 10
                        color: Theme.textPrimary
                    }
                }
            }
        }

        ProgressBar {
            width: parent.width
            value: root.workflow ? root.workflow.progress : 0
            visible: root.workflow && root.workflow.running
        }

        Text {
            visible: root.workflow && root.workflow.inError
            color: Theme.accentAlarm
            wrapMode: Text.WordWrap
            width: parent.width
            text: root.workflow ? root.workflow.errorMessage : ""
        }

        Row {
            spacing: 8
            Button {
                text: qsTr("Start")
                enabled: root.workflow && !root.workflow.running && !root.workflow.inError
                onClicked: root.workflow.start()
            }
            Button {
                text: qsTr("Cancel")
                enabled: root.workflow && (root.workflow.running || root.workflow.inError)
                onClicked: root.workflow.cancel()
            }
            Button {
                text: qsTr("Retry")
                visible: root.workflow && root.workflow.inError
                onClicked: root.workflow.retry()
            }
            Button {
                text: qsTr("Inject failure (demo)")
                enabled: root.workflow && !root.workflow.running
                onClicked: root.workflow.injectFailureOnNextStep()
            }
        }
    }
}
