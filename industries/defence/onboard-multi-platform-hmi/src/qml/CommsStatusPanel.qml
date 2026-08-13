// SPDX-License-Identifier: MIT
import QtQuick

import "Icons.js" as Icons

// Comms & tactical-data-link status panel: a per-subsystem antenna/signal
// readout plus a list of the off-board tracks received over the simulated
// tactical data link, each shown with its own staleness/age. This panel
// only ever reports data-quality information (signal quality, whether a
// track's position is fresh or stale) - it carries no weapons, targeting or
// fire-control content of any kind.
Rectangle {
    id: root
    property var sim
    color: "#12151b"

    readonly property bool anySubsystemCritical: {
        const list = root.sim ? root.sim.subsystems : [];
        for (let i = 0; i < list.length; ++i) {
            if (list[i].state === "Critical") return true;
        }
        return false;
    }

    Column {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 20

        Row {
            spacing: 16
            Canvas {
                id: shieldCanvas
                width: 48
                height: 48
                onPaint: {
                    const ctx = getContext("2d");
                    ctx.reset();
                    Icons.drawShield(ctx, 20, 18, 16, root.anySubsystemCritical ? "#c0392b" : "#2fa84f",
                                      !root.anySubsystemCritical);
                }
                Timer { interval: 300; running: true; repeat: true; onTriggered: shieldCanvas.requestPaint() }
            }
            Column {
                spacing: 2
                Text {
                    text: qsTr("Comms & Data-Link Status")
                    color: "#f2f4f8"
                    font.pixelSize: 22
                    font.bold: true
                }
                Text {
                    text: root.anySubsystemCritical
                          ? qsTr("Overall platform status: CRITICAL - one or more subsystems degraded")
                          : qsTr("Overall platform status: nominal")
                    color: root.anySubsystemCritical ? "#e0a300" : "#9aa4b2"
                    font.pixelSize: 13
                }
            }
        }

        Text { text: qsTr("Subsystem Comms Readout"); color: "#9aa4b2"; font.pixelSize: 14 }

        Row {
            spacing: 12
            Repeater {
                model: root.sim ? root.sim.subsystems : []
                Rectangle {
                    width: 150
                    height: 70
                    radius: 8
                    color: "#1c222b"
                    border.color: modelData.state === "Critical" ? "#c0392b"
                                  : modelData.state === "Caution" ? "#e0a300" : "#2fa84f"
                    border.width: 2

                    Row {
                        anchors.centerIn: parent
                        spacing: 10

                        Canvas {
                            id: subsystemIcon
                            width: 28
                            height: 28
                            anchors.verticalCenter: parent.verticalCenter
                            onPaint: {
                                const ctx = getContext("2d");
                                ctx.reset();
                                Icons.drawAntennaBars(ctx, 12, 10, 10, modelData.value !== undefined
                                                       ? modelData.value : 100, "#39c0ff");
                            }
                            Timer { interval: 200; running: true; repeat: true; onTriggered: subsystemIcon.requestPaint() }
                        }
                        Column {
                            spacing: 2
                            Text { text: modelData.name; color: "#f2f4f8"; font.pixelSize: 13; font.bold: true }
                            Text {
                                text: modelData.state + " (" + Math.round(modelData.value !== undefined
                                      ? modelData.value : 100) + "%)"
                                color: "#9aa4b2"
                                font.pixelSize: 11
                            }
                        }
                    }
                }
            }
        }

        Text {
            text: qsTr("Tactical Data-Link Tracks (received from other platforms)")
            color: "#9aa4b2"
            font.pixelSize: 14
        }

        Column {
            width: parent.width
            spacing: 6
            Repeater {
                model: root.sim ? root.sim.dataLinkTracks : []
                Rectangle {
                    width: parent.width
                    height: 40
                    radius: 6
                    color: modelData.stale ? "#241f16" : "#182028"
                    border.color: modelData.stale ? "#e0a300" : "#274050"
                    border.width: 1

                    Row {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 12

                        Canvas {
                            width: 24
                            height: 24
                            anchors.verticalCenter: parent.verticalCenter
                            onPaint: {
                                const ctx = getContext("2d");
                                ctx.reset();
                                Icons.drawTrackGlyph(ctx, 12, 12, modelData.domain, 0,
                                                      modelData.stale ? "#7a8a99" : "#39c0ff", 6);
                            }
                        }
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData.classification + " #" + modelData.id + " - " + modelData.domain
                            color: modelData.stale ? "#c9b98a" : "#f2f4f8"
                            font.pixelSize: 12
                            width: 220
                        }
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData.stale
                                  ? qsTr("STALE - last update %1s ago").arg(modelData.dataAgeSeconds.toFixed(0))
                                  : qsTr("Current")
                            color: modelData.stale ? "#e0a300" : "#3ddc6f"
                            font.pixelSize: 12
                            font.bold: modelData.stale
                        }
                    }
                }
            }
        }
    }
}
