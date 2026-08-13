// SPDX-License-Identifier: MIT
import QtQuick

// Picks a small procedurally-drawn glyph for one activity-log line based on
// what kind of change it describes, so the timeline reads like a real
// history feed rather than a plain text log. `kind` is decided by Main.qml
// from the entry's description text (see activityIconKind()).
Item {
    id: root

    property string kind: "other"

    implicitWidth: 20
    implicitHeight: 20

    LightbulbIcon {
        visible: root.kind === "light"
        anchors.fill: parent
        on: true
        brightness: 100
    }
    LockIcon {
        visible: root.kind === "lock"
        anchors.fill: parent
        locked: true
    }
    DoorSensorIcon {
        visible: root.kind === "security"
        anchors.fill: parent
        triggered: false
    }
    ThermostatIcon {
        visible: root.kind === "thermostat"
        anchors.fill: parent
        mode: 1
    }
    Rectangle {
        visible: root.kind === "other"
        anchors.centerIn: parent
        width: 8
        height: 8
        radius: 4
        color: "#4a5568"
    }
}
