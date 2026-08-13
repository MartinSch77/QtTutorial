// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Step 6: instant theme switching (dark/light/high-contrast) and desktop
// <-> simplified-tablet layout switching, both driven purely by writing to
// the Theme singleton's properties - see Theme.qml for why this is a plain
// QML object rather than Qt6::LabsStyleKit.
Row {
    id: root
    spacing: Theme.spacingUnit

    Repeater {
        model: ["dark", "light", "high-contrast"]
        delegate: Button {
            required property string modelData
            text: modelData
            highlighted: Theme.mode === modelData
            onClicked: Theme.mode = modelData
        }
    }

    Rectangle { width: 1; height: 24; color: Theme.border }

    Button {
        text: Theme.isTablet ? qsTr("Desktop layout") : qsTr("Tablet layout")
        onClicked: Theme.layout = Theme.isTablet ? "desktop" : "tablet"
    }
}
