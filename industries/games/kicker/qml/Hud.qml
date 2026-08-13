// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

// The 2D overlay drawn on top of the View3D: score, connection status and
// an in-game "leave match" control. Alongside Table3D.qml's 3D scene, this
// is what makes the kicker app demonstrate both 2D and 3D Qt Quick
// rendering in one window.
Item {
    id: root

    property int scoreA: 0
    property int scoreB: 0
    property string connectionStatus: ""
    property int localSeat: -1

    signal leaveRequested()

    Rectangle {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 16
        width: 220
        height: 64
        radius: 8
        color: "#202225"
        opacity: 0.85

        Row {
            anchors.centerIn: parent
            spacing: 18

            Text { text: qsTr("Team A"); color: "#ef5350"; font.pixelSize: 14 }
            Text { text: root.scoreA + " : " + root.scoreB; color: "white"; font.pixelSize: 22; font.bold: true }
            Text { text: qsTr("Team B"); color: "#42a5f5"; font.pixelSize: 14 }
        }
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 12
        width: statusText.implicitWidth + 20
        height: statusText.implicitHeight + 12
        radius: 6
        color: "#202225"
        opacity: 0.85

        Text {
            id: statusText
            anchors.centerIn: parent
            color: "#b0bec5"
            font.pixelSize: 12
            text: qsTr("%1 (seat %2)").arg(root.connectionStatus).arg(root.localSeat)
        }
    }

    Button {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 12
        text: qsTr("Leave")
        onClicked: root.leaveRequested()
    }
}
