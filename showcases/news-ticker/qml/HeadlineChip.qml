// SPDX-License-Identifier: MIT
import QtQuick

// One headline in the scrolling band. A "breaking" headline gets a distinct,
// continuously pulsing accent treatment so it reads as urgent even while
// scrolling past at speed.
Row {
    id: root
    // Named to match NewsFeedModel's role names exactly ("text"/"category"/
    // "breaking", see NewsFeedModel::roleNames()) rather than
    // "headlineText"/"headlineCategory": Qt Quick automatically binds a
    // model role's value to a delegate's required property of the same
    // name with no explicit binding needed - and that automatic binding is
    // the only role-data path that works with AOT-compiled QML
    // (qmlcachegen). An explicit `headlineText: text` binding in the
    // Repeater delegate, as this file previously required, referenced
    // Repeater's dynamically-injected bare role-name context properties,
    // which the AOT compiler cannot resolve statically - confirmed by
    // actually running this app, which failed at runtime with "text is not
    // defined" / "model is not defined" ReferenceErrors.
    required property string text
    required property string category
    required property bool breaking

    spacing: 10
    height: 32

    Rectangle {
        visible: root.breaking
        anchors.verticalCenter: parent.verticalCenter
        width: breakingLabel.implicitWidth + 12
        height: 22
        radius: 4
        color: "#e5484d"

        Text {
            id: breakingLabel
            anchors.centerIn: parent
            text: qsTr("BREAKING")
            color: "white"
            font.pixelSize: 11
            font.bold: true
        }

        SequentialAnimation on opacity {
            running: root.breaking
            loops: Animation.Infinite
            NumberAnimation { from: 1.0; to: 0.35; duration: 500 }
            NumberAnimation { from: 0.35; to: 1.0; duration: 500 }
        }
    }

    Text {
        anchors.verticalCenter: parent.verticalCenter
        text: "[" + root.category + "] " + root.text
        color: root.breaking ? "#ff8a80" : "white"
        font.pixelSize: 16
        font.bold: root.breaking
    }
}
