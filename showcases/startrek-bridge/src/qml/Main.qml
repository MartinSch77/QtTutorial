// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.Showcases.StartrekBridge

// LCARS-styled starship bridge dashboard: a main viewscreen plus three
// selectable station panels (Helm, Engineering, Tactical), all sharing one
// BridgeSimulator instance so the ship-wide alert level (set from the
// pill buttons in the top status strip) is reflected consistently across
// every station. Reflows between a wide desktop aspect ratio (sidebar
// station selector) and a narrow/tall mobile one (bottom tab strip)
// purely via anchors and the wideLayout property below - resize the window
// to see it happen.
Window {
    id: window
    width: 1100
    height: 680
    minimumWidth: 340
    minimumHeight: 480
    visible: true
    color: "#0a0710"
    title: qsTr("QtTutorial - Starship Bridge")

    readonly property bool wideLayout: width >= 820
    readonly property int selectorExtent: wideLayout ? 180 : 72

    BridgeSimulator {
        id: bridge
    }

    StatusStrip {
        id: statusStrip
        bridge: bridge
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 48
    }

    StationSelector {
        id: selector
        horizontal: !window.wideLayout
        accentColor: "#ff9c00"

        anchors.top: window.wideLayout ? statusStrip.bottom : undefined
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: window.wideLayout ? undefined : parent.right
        width: window.wideLayout ? window.selectorExtent : undefined
        height: window.wideLayout ? undefined : window.selectorExtent
    }

    Item {
        id: content
        anchors.top: statusStrip.bottom
        anchors.left: window.wideLayout ? selector.right : parent.left
        anchors.right: parent.right
        anchors.bottom: window.wideLayout ? parent.bottom : selector.top

        ViewscreenPanel { anchors.fill: parent; bridge: bridge; visible: selector.currentIndex === 0 }
        HelmPanel { anchors.fill: parent; bridge: bridge; visible: selector.currentIndex === 1 }
        EngineeringPanel { anchors.fill: parent; bridge: bridge; visible: selector.currentIndex === 2 }
        TacticalPanel { anchors.fill: parent; bridge: bridge; visible: selector.currentIndex === 3 }
    }
}
