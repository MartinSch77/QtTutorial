// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted arc gauge (Canvas) rather than a charting library - Qt
// Charts/Graphs are not MIT licensed. The arc spans a fixed temperature
// range and grows/shrinks as displayTemp eases towards the controller's
// currentTemperature on every simulation tick, so the needle visibly glides
// instead of jumping.
Item {
    id: root

    property real currentTemperature: 19
    property real targetTemperature: 21
    property real minTemperature: 10
    property real maxTemperature: 30
    property color accentColor: "#ff9f43"

    property real displayTemp: currentTemperature
    Behavior on displayTemp { NumberAnimation { duration: 180; easing.type: Easing.OutQuad } }

    implicitWidth: 220
    implicitHeight: 220

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const radius = Math.min(width, height) / 2 - 12;
            const startAngle = Math.PI * 0.75;
            const endAngle = Math.PI * 2.25;

            function angleFor(temp) {
                const ratio = Math.max(0, Math.min(1, (temp - root.minTemperature) / (root.maxTemperature - root.minTemperature)));
                return startAngle + ratio * (endAngle - startAngle);
            }

            ctx.lineWidth = 14;
            ctx.lineCap = "round";

            ctx.strokeStyle = "#2a3140";
            ctx.beginPath();
            ctx.arc(cx, cy, radius, startAngle, endAngle);
            ctx.stroke();

            ctx.strokeStyle = root.accentColor;
            ctx.beginPath();
            ctx.arc(cx, cy, radius, startAngle, angleFor(root.displayTemp));
            ctx.stroke();

            const targetAngle = angleFor(root.targetTemperature);
            ctx.strokeStyle = "#e6edf3";
            ctx.lineWidth = 4;
            ctx.beginPath();
            ctx.moveTo(cx + Math.cos(targetAngle) * (radius - 12), cy + Math.sin(targetAngle) * (radius - 12));
            ctx.lineTo(cx + Math.cos(targetAngle) * (radius + 12), cy + Math.sin(targetAngle) * (radius + 12));
            ctx.stroke();
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 2
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("%1°C").arg(root.displayTemp.toFixed(1))
            color: "#e6edf3"
            font.pixelSize: 32
            font.bold: true
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("target %1°C").arg(root.targetTemperature.toFixed(1))
            color: "#9aa4b2"
            font.pixelSize: 13
        }
    }

    onDisplayTempChanged: canvas.requestPaint()
    onTargetTemperatureChanged: canvas.requestPaint()
    Component.onCompleted: canvas.requestPaint()
}
