// SPDX-License-Identifier: MIT
import QtQuick

// The in-match screen: Table3D (the 3D view) with Hud (2D overlay) drawn on
// top, and the MouseArea that turns raw mouse movement into rod commands.
//
// Controls (see README "Controls" for the full explanation): the mouse's
// horizontal position across this area maps directly to the active rod's
// slide position; the mouse's horizontal *velocity* maps to the active
// rod's spin. Which rod is "active" is decided by KickerController
// (nearest to the ball in 2-player mode, or the seat's one fixed rod in
// 4-player mode) -- the whole play area is one control surface, not
// divided into per-rod zones.
Item {
    id: root

    required property var controller

    property real lastNormalizedX: 0
    property double lastTimestampMs: 0

    Table3D {
        id: table3D
        anchors.fill: parent
        ballX: root.controller.ballX
        ballZ: root.controller.ballZ
        rod0Slide: root.controller.rod0Slide
        rod0Rotation: root.controller.rod0Rotation
        rod1Slide: root.controller.rod1Slide
        rod1Rotation: root.controller.rod1Rotation
        rod2Slide: root.controller.rod2Slide
        rod2Rotation: root.controller.rod2Rotation
        rod3Slide: root.controller.rod3Slide
        rod3Rotation: root.controller.rod3Rotation
    }

    // The rod's angular velocity is sent explicitly, not derived from
    // decaying physics on the client: without a fresh mouse-move event, a
    // stationary mouse would otherwise leave the last non-zero spin
    // command in effect forever. This timer zeroes the spin once the
    // mouse has been idle, while still holding the last slide position.
    Timer {
        interval: 80
        running: true
        repeat: true
        onTriggered: {
            if (Date.now() - root.lastTimestampMs > 80) {
                root.controller.mouseInput(root.lastNormalizedX, 0)
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onPositionChanged: (mouse) => {
            const normalizedX = Math.max(-1, Math.min(1, (mouse.x / width) * 2 - 1))
            const now = Date.now()
            const dtMs = root.lastTimestampMs > 0 ? Math.max(1, now - root.lastTimestampMs) : 16
            const spinRate = (normalizedX - root.lastNormalizedX) / (dtMs / 1000)

            root.controller.mouseInput(normalizedX, spinRate)

            root.lastNormalizedX = normalizedX
            root.lastTimestampMs = now
        }
    }

    Hud {
        anchors.fill: parent
        scoreA: root.controller.scoreA
        scoreB: root.controller.scoreB
        connectionStatus: root.controller.connectionStatus
        localSeat: root.controller.localSeat
        onLeaveRequested: root.controller.leaveMatch()
    }
}
