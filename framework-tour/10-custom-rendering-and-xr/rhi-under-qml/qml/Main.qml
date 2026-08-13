// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls.Basic
import QtTutorial.CustomRenderingXr.RhiUnderQml

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("RHI Under QML - custom QRhi drawing beneath ordinary Qt Quick items")
    color: "#101418"

    RhiUnderlayItem {
        id: underlay
        anchors.fill: parent

        NumberAnimation on threshold {
            from: 0.1
            to: 0.9
            duration: 2200
            loops: Animation.Infinite
            easing.type: Easing.InOutQuad
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 12

        Label {
            text: qsTr("Qt Quick UI on top")
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        Rectangle {
            width: 260
            height: 70
            radius: 10
            color: "#20242c"
            border.color: "#3ddc84"
            border.width: 2

            Label {
                anchors.centerIn: parent
                text: qsTr("threshold = %1").arg(underlay.threshold.toFixed(2))
                color: "#3ddc84"
                font.pixelSize: 16
            }
        }

        Label {
            width: 260
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("The two-color triangle behind this text is drawn with raw QRhi calls "
                       + "recorded before the scene graph's own render pass (an \"underlay\") - "
                       + "the QML Rectangle/Label above genuinely composite over it in one "
                       + "frame.")
            color: "#a0a8b4"
            font.pixelSize: 12
        }
    }
}
