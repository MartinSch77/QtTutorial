// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Qt.labs.lottieqt 1.0

// Small animated "system online" icon (a pulsing cyan dot), the one Lottie
// animation the brief asks for. Import verified against the Qt 6.11
// documentation (qt_documentation_read on
// qml-qt-labs-lottieqt-lottieanimation.html) before use - matches
// showcases/stock-tracker/qml/OrderPanel.qml's already-working
// `import Qt.labs.lottieqt 1.0` / `LottieAnimation` pattern exactly.
Item {
    id: root
    width: 28
    height: 28

    LottieAnimation {
        id: pulse
        anchors.fill: parent
        source: "qrc:/offboard_digital_twin/lottie/system-online.json"
        // `loops: 1` + a restarting Timer, exactly like
        // showcases/stock-tracker/qml/OrderPanel.qml's LottieAnimation usage
        // (autoPlay: false, loops: 1, start() called explicitly) - stock-
        // tracker's file is this module's only other real, working
        // LottieAnimation usage in the repo, and it does not use any
        // "infinite loop" enum/property, so this file does not invent one.
        loops: 1
        autoPlay: false
        onStatusChanged: if (status === LottieAnimation.Ready) start()
    }

    Timer {
        interval: 2000
        running: true
        repeat: true
        onTriggered: if (pulse.status === LottieAnimation.Ready) pulse.start()
    }
}
