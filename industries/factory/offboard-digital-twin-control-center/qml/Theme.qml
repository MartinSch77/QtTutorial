// SPDX-License-Identifier: GPL-3.0-or-later
pragma Singleton
import QtQuick

// Hand-written QML theme singleton for step 6 of the demo scenario
// ("instant theme switching: dark/light/high-contrast").
//
// The brief asked us to attempt Qt6::LabsStyleKit first. A
// qt_documentation_search for "LabsStyleKit" returned zero results - no
// public QML API for it could be confirmed via the Qt Documentation MCP (the
// brief itself notes only private headers were seen locally at the time of
// writing). Per the brief's explicit instruction not to fabricate an API
// from guesswork, this singleton is a plain hand-written QML `Theme`
// substitute instead: every color/spacing value the UI needs lives here, and
// switching `Theme.mode` re-evaluates every binding in the app instantly
// (QML's property binding system, not a Loader reload), which is what
// actually delivers the "instant theme switching" requirement.
QtObject {
    id: root

    // "desktop" | "tablet" - layout density, independent of color mode.
    property string layout: "desktop"
    // "dark" | "light" | "high-contrast" - color mode.
    property string mode: "dark"

    readonly property bool isTablet: layout === "tablet"
    readonly property bool isHighContrast: mode === "high-contrast"

    readonly property color background: mode === "light" ? "#eef1f4"
        : mode === "high-contrast" ? "#000000" : "#14171a"
    readonly property color surface: mode === "light" ? "#ffffff"
        : mode === "high-contrast" ? "#0a0a0a" : "#1c2024"
    readonly property color glassSurface: mode === "light" ? Qt.rgba(1, 1, 1, 0.72)
        : mode === "high-contrast" ? Qt.rgba(0, 0, 0, 0.92) : Qt.rgba(0.14, 0.16, 0.18, 0.72)
    readonly property color border: mode === "light" ? "#c7ccd1"
        : mode === "high-contrast" ? "#ffffff" : "#2c3136"
    readonly property color textPrimary: mode === "light" ? "#151a1e"
        : mode === "high-contrast" ? "#ffffff" : "#e7ecef"
    readonly property color textSecondary: mode === "light" ? "#5b6570"
        : mode === "high-contrast" ? "#d8d8d8" : "#8a939c"

    // Industrial-HMI accent palette: cyan for normal, amber for warning, red
    // for alarm - never a generic sci-fi purple/neon scheme.
    readonly property color accentNormal: isHighContrast ? "#00e5ff" : "#29c7e0"
    readonly property color accentWarning: isHighContrast ? "#ffcc00" : "#e0a72a"
    readonly property color accentAlarm: isHighContrast ? "#ff3b30" : "#d94f3d"
    readonly property color accentStopped: mode === "light" ? "#8a939c" : "#5c6570"

    readonly property int spacingUnit: isTablet ? 14 : 8
    readonly property int radius: isTablet ? 10 : 6
    readonly property int fontSizeBody: isTablet ? 15 : 13
    readonly property int fontSizeHeading: isTablet ? 20 : 16
    readonly property int transitionMs: 260

    function colorForState(state) {
        switch (state) {
        case "warning": return accentWarning;
        case "alarm": return accentAlarm;
        case "stopped": return accentStopped;
        case "maintenance": return accentWarning;
        default: return accentNormal;
        }
    }
}
