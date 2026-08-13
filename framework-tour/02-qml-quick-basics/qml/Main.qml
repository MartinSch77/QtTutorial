// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls
import QtTutorial.QmlQuickBasics

ApplicationWindow {
    id: window
    width: 420
    height: 600
    visible: true
    title: qsTr("Qt Quick Basics – System Status")
    color: "#101418"

    SystemStatusBackend {
        id: backend
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: qsTr("Overall status: %1").arg(backend.overallStatus)
            color: "white"
            font.pixelSize: 18
            font.bold: true
        }

        Text {
            text: qsTr("Uptime: %1 s").arg(backend.uptimeSeconds)
            color: "#8fa0b3"
        }

        ListView {
            id: listView
            width: parent.width
            height: parent.height - 64
            spacing: 8
            clip: true
            model: backend.indicators
            delegate: IndicatorTile {
                width: listView.width
                indicatorName: name
                indicatorValue: value
                indicatorStatus: status
            }
        }
    }
}
