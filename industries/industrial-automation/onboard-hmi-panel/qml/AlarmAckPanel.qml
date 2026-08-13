// SPDX-License-Identifier: MIT
import QtQuick

// The alarm/fault acknowledgement panel: a severity-coloured alarm history
// list where every entry can be acknowledged individually, plus an
// "acknowledge all" action and an outstanding-alarm counter badge. This is
// the operator workflow a real HMI alarm banner drives towards (see, ack,
// clear) rather than just a scrolling read-only log.
Item {
    id: root
    property QtObject alarmModel

    Column {
        anchors.fill: parent
        spacing: 8

        Item {
            width: parent.width
            height: 22

            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Alarms")
                color: "#f2f4f7"
                font.pixelSize: 14
                font.bold: true
            }

            Rectangle {
                id: ackBadge
                anchors.right: ackAllButton.left
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                visible: root.alarmModel !== null && root.alarmModel.unacknowledgedCount > 0
                width: 26
                height: 20
                radius: 4
                color: "#e5484d"

                Text {
                    anchors.centerIn: parent
                    text: root.alarmModel ? root.alarmModel.unacknowledgedCount : 0
                    color: "white"
                    font.pixelSize: 11
                    font.bold: true
                }
            }

            Rectangle {
                id: ackAllButton
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                width: 84
                height: 22
                radius: 4
                color: "#243040"
                border.color: "#3a4452"

                Text {
                    anchors.centerIn: parent
                    text: qsTr("ACK ALL")
                    color: "#c8d0da"
                    font.pixelSize: 10
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: if (root.alarmModel) root.alarmModel.acknowledgeAll()
                }
            }
        }

        ListView {
            width: parent.width
            height: parent.height - 30
            clip: true
            model: root.alarmModel
            spacing: 6
            delegate: Rectangle {
                required property string message
                required property int severity
                required property var timestamp
                required property bool acknowledged
                required property int index

                width: ListView.view.width
                height: 50
                radius: 4
                color: severity === 2 ? "#3a1d20" : (severity === 1 ? "#3a341a" : "#1d2733")
                border.color: severity === 2 ? "#e5484d" : (severity === 1 ? "#e5b93d" : "#2c3644")
                opacity: acknowledged ? 0.55 : 1.0

                Row {
                    anchors.fill: parent
                    anchors.margins: 6
                    spacing: 6

                    Column {
                        width: parent.width - ackButton.width - 6
                        Text {
                            width: parent.width
                            text: message
                            color: "#f2f4f7"
                            font.pixelSize: 11
                            elide: Text.ElideRight
                        }
                        Text {
                            text: Qt.formatTime(timestamp, "hh:mm:ss") + (acknowledged ? qsTr(" · acked") : "")
                            color: "#8a94a3"
                            font.pixelSize: 9
                        }
                    }

                    Rectangle {
                        id: ackButton
                        width: 46
                        height: 36
                        radius: 4
                        anchors.verticalCenter: parent.verticalCenter
                        visible: !acknowledged
                        color: "#243040"
                        border.color: "#3a4452"

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("ACK")
                            color: "#c8d0da"
                            font.pixelSize: 9
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: if (root.alarmModel) root.alarmModel.acknowledge(index)
                        }
                    }
                }
            }
        }
    }
}
