// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls.Basic
import QtTutorial.NewsTicker

ApplicationWindow {
    id: window
    width: 900
    height: 260
    visible: true
    title: qsTr("News Ticker – Qt Quick showcase")
    color: "#0e1116"

    NewsFeedModel {
        id: feedModel
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Text {
            text: qsTr("Live headlines")
            color: "white"
            font.pixelSize: 20
            font.bold: true
        }

        TabBar {
            id: categoryTabs
            width: parent.width
            Repeater {
                model: feedModel.availableCategories
                delegate: TabButton {
                    text: modelData
                }
            }
            onCurrentIndexChanged: feedModel.activeCategory = feedModel.availableCategories[currentIndex]
        }

        Rectangle {
            width: parent.width
            height: 72
            radius: 8
            color: "#171b23"
            border.color: "#2a3040"

            Item {
                id: tickerBand
                anchors.fill: parent
                anchors.margins: 4
                clip: true

                // Seamless-loop marquee: two back-to-back copies of the same headline
                // set scroll left together; when the animation reaches -halfWidth it
                // snaps back to 0, which is visually invisible because both halves are
                // identical. This is a smooth, per-frame QML animation, not a
                // timer-driven jump from one fixed position to the next.
                Row {
                    id: track
                    height: parent.height
                    y: 0
                    spacing: 0

                    property real halfWidth: firstHalf.implicitWidth + firstHalf.spacing

                    Row {
                        id: firstHalf
                        spacing: 48
                        height: track.height
                        Repeater {
                            model: feedModel
                            delegate: HeadlineChip {
                                anchors.verticalCenter: parent.verticalCenter
                                headlineText: text
                                headlineCategory: category
                                breaking: model.breaking
                            }
                        }
                    }

                    Row {
                        spacing: 48
                        height: track.height
                        x: firstHalf.implicitWidth + firstHalf.spacing
                        Repeater {
                            model: feedModel
                            delegate: HeadlineChip {
                                anchors.verticalCenter: parent.verticalCenter
                                headlineText: text
                                headlineCategory: category
                                breaking: model.breaking
                            }
                        }
                    }

                    NumberAnimation on x {
                        id: marquee
                        from: 0
                        to: -track.halfWidth
                        duration: Math.max(4000, track.halfWidth * 18)
                        loops: Animation.Infinite
                    }
                }
            }
        }

        Text {
            text: qsTr("Category: %1 — headlines rotate automatically every ~2 seconds").arg(feedModel.activeCategory)
            color: "#8fa0b3"
            font.pixelSize: 12
        }
    }
}
