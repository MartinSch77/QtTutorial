// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted Canvas trend line (the same approach onboard-wall-panel's
// ThermostatGauge.qml uses for its arc gauge) rather than a charting
// library - Qt Charts/Graphs are not MIT licensed and this app does not
// link either. Shows the estimated household power draw over recent
// snapshots so a manager audience sees the app doing something beyond
// on/off toggles: correlating device state with cost.
Rectangle {
    id: root

    required property var controller

    radius: 12
    color: "#1c212b"
    border.color: "#2a3140"
    border.width: 1

    Column {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Row {
            width: parent.width
            Text {
                width: parent.width - 90
                text: qsTr("%1 W now").arg(Math.round(root.controller.currentWatts))
                color: "#e6edf3"
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("avg %1 W").arg(Math.round(root.controller.averageWatts))
                color: "#9aa4b2"
                font.pixelSize: 12
            }
        }

        Canvas {
            id: chart
            width: parent.width
            height: 70

            property var history: root.controller.energyHistory

            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();
                const w = width;
                const h = height;
                const values = history;

                ctx.strokeStyle = "#2a3140";
                ctx.lineWidth = 1;
                ctx.beginPath();
                ctx.moveTo(0, h - 1);
                ctx.lineTo(w, h - 1);
                ctx.stroke();

                if (!values || values.length < 2) {
                    return;
                }

                let maxV = 1;
                for (let i = 0; i < values.length; ++i) {
                    maxV = Math.max(maxV, values[i]);
                }
                maxV *= 1.1;

                ctx.strokeStyle = "#39c0ff";
                ctx.lineWidth = 2;
                ctx.lineJoin = "round";
                ctx.beginPath();
                for (let i = 0; i < values.length; ++i) {
                    const x = (i / (values.length - 1)) * w;
                    const y = h - (values[i] / maxV) * h;
                    if (i === 0) {
                        ctx.moveTo(x, y);
                    } else {
                        ctx.lineTo(x, y);
                    }
                }
                ctx.stroke();

                ctx.fillStyle = "#39c0ff";
                const lastX = w;
                const lastY = h - (values[values.length - 1] / maxV) * h;
                ctx.beginPath();
                ctx.arc(lastX - 3, lastY, 3, 0, Math.PI * 2);
                ctx.fill();
            }

            onHistoryChanged: requestPaint()
            Component.onCompleted: requestPaint()
        }
    }
}
