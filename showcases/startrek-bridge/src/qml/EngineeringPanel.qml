// SPDX-License-Identifier: MIT
import QtQuick

// Engineering station: simulated INTERNAL ship conditions - warp-core power
// output, hull integrity, internal temperature, and a deck-by-deck
// life-support status grid driven by real DeckLifeSupportMachine state
// machines so status changes are believable transitions, not flicker. The
// gauges wrap (Flow) and the whole panel scrolls (Flickable) so it still
// fits on a narrow, short phone-sized window instead of overflowing.
Item {
    id: root
    property var bridge

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: column.implicitHeight + 32
        clip: true

        Column {
            id: column
            x: 16
            y: 16
            width: parent.width - 32
            spacing: 16

            Text { text: qsTr("ENGINEERING"); color: "#ff9c00"; font.pixelSize: 18; font.bold: true }

            Flow {
                width: parent.width
                spacing: 24

                RadialGauge {
                    width: 130
                    height: 130
                    value: root.bridge ? root.bridge.powerOutputPercent : 0
                    minValue: 0
                    maxValue: 100
                    label: qsTr("WARP CORE OUTPUT")
                    valueText: root.bridge ? root.bridge.powerOutputPercent.toFixed(0) + "%" : "0%"
                    accentColor: "#ff9c00"
                }
                RadialGauge {
                    width: 130
                    height: 130
                    value: root.bridge ? root.bridge.hullIntegrityPercent : 100
                    minValue: 80
                    maxValue: 100
                    label: qsTr("HULL INTEGRITY")
                    valueText: root.bridge ? root.bridge.hullIntegrityPercent.toFixed(0) + "%" : "100%"
                    accentColor: "#39c0ff"
                }
                RadialGauge {
                    width: 130
                    height: 130
                    value: root.bridge ? root.bridge.internalTempC : 21
                    minValue: 15
                    maxValue: 32
                    label: qsTr("INTERNAL TEMP")
                    valueText: root.bridge ? root.bridge.internalTempC.toFixed(1) + "°C" : "21.0°C"
                    accentColor: "#c6a0ff"
                }
            }

            Text { text: qsTr("DECK LIFE SUPPORT"); color: "#9c8fae"; font.pixelSize: 13 }

            DeckGrid {
                width: parent.width
                decks: root.bridge ? root.bridge.deckStatuses : []
            }
        }
    }
}
