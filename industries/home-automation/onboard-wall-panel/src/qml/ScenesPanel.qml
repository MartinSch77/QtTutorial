// SPDX-License-Identifier: MIT
import QtQuick

// A Google Home/Apple Home/Control4-style scene selector: one tap changes
// lighting, locks, security arming and the thermostat setpoint together,
// rather than making the user visit five separate panels. The actual
// per-scene device policy lives in SceneRegistry (see
// industries/home-automation/onboard-wall-panel/src/SceneRegistry.h); this
// panel just fans one tap out to the four models that own that state.
Column {
    id: root

    required property var roomModel
    required property var lockModel
    required property var securityModel
    required property var thermostat

    property int activeScene: -1

    readonly property var sceneDefs: [
        {id: 0, name: qsTr("Home"), description: qsTr("Everyday comfort: lights on, doors as-is, climate steady.")},
        {id: 1, name: qsTr("Away"), description: qsTr("Lights off, doors locked, security armed, heating set back.")},
        {id: 2, name: qsTr("Night"), description: qsTr("Lights dimmed low, doors locked, security armed, cooler for sleep.")},
        {id: 3, name: qsTr("Morning"), description: qsTr("Lights bright, security stands down, climate back to comfortable.")},
    ]

    spacing: 20

    Text {
        text: qsTr("Scenes")
        color: "#e6edf3"
        font.pixelSize: 20
        font.bold: true
    }
    Text {
        width: parent.width
        text: qsTr("Tap a scene to apply it to every room, lock, the security system and the thermostat at once.")
        color: "#9aa4b2"
        font.pixelSize: 13
        wrapMode: Text.WordWrap
    }

    Grid {
        width: parent.width
        columns: 2
        columnSpacing: 16
        rowSpacing: 16

        Repeater {
            model: root.sceneDefs
            SceneCard {
                width: (root.width - 16) / 2
                height: 150
                sceneId: modelData.id
                sceneName: modelData.name
                sceneDescription: modelData.description
                active: root.activeScene === modelData.id
                onActivated: {
                    root.roomModel.applyScene(modelData.id);
                    root.lockModel.applyScene(modelData.id);
                    root.securityModel.applyScene(modelData.id);
                    root.thermostat.applyScene(modelData.id);
                    root.activeScene = modelData.id;
                }
            }
        }
    }
}
