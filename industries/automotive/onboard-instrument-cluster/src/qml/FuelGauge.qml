// SPDX-License-Identifier: MIT
import QtQuick

// A vector fuel-pump icon (Canvas-painted outline + nozzle) paired with a level
// bar, standing in for the small fuel/battery pictogram found on a real cluster
// instead of a plain percentage label.
Item {
    id: root

    property real levelPercent: 100
    property bool warning: false
    property color accentColor: "#39c0ff"

    implicitWidth: 96
    implicitHeight: 130

    readonly property color barColor: warning ? "#ff4b5c" : accentColor

    Canvas {
        id: icon
        width: 40
        height: 40
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            ctx.strokeStyle = root.barColor;
            ctx.lineWidth = 2.4;
            ctx.lineJoin = "round";

            // Pump body.
            ctx.beginPath();
            ctx.rect(6, 8, 18, 28);
            ctx.stroke();

            // Fill cap.
            ctx.beginPath();
            ctx.moveTo(10, 8);
            ctx.lineTo(10, 3);
            ctx.lineTo(20, 3);
            ctx.stroke();

            // Nozzle hose, a simple curved path.
            ctx.beginPath();
            ctx.moveTo(24, 14);
            ctx.lineTo(30, 14);
            ctx.lineTo(30, 30);
            ctx.lineTo(26, 34);
            ctx.stroke();
        }
    }

    Rectangle {
        id: track
        anchors.top: icon.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.8
        height: 14
        radius: 4
        color: "#1c212b"
        border.color: "#2a2f3a"

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 2
            width: Math.max(0, (parent.width - 4) * Math.max(0, Math.min(1, root.levelPercent / 100)))
            radius: 3
            color: root.barColor

            Behavior on width { NumberAnimation { duration: 200 } }
        }
    }

    Text {
        anchors.top: track.bottom
        anchors.topMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        text: Math.round(root.levelPercent) + "%"
        color: root.warning ? "#ff4b5c" : "#f2f4f8"
        font.pixelSize: 16
        font.bold: true
    }

    Text {
        anchors.top: parent.top
        anchors.right: parent.right
        text: qsTr("FUEL")
        color: "#9aa4b2"
        font.pixelSize: 10
    }
}
