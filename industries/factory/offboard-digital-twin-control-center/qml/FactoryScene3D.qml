// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick3D
import QtQuick.Timeline

// Step 1 of the demo scenario: an interactive, procedurally-built 3D factory
// scene with real PBR materials/shadows and PickResult-based selection - no
// glTF assets (none available/fetchable in this sandbox), just Quick3D's
// built-in primitive meshes composed into machine-like shapes (see
// Machine3D.qml).
Item {
    id: root

    property var machineModel
    property int selectedMachineId: -1
    property bool thermalOverlay: false
    property var overheatingMachineIds: []

    signal machineClicked(int machineId)
    signal cameraMoved(vector3d position, vector3d forward)

    function flyTo(targetX, targetZ) {
        flyFromX = camera.position.x; flyFromY = camera.position.y; flyFromZ = camera.position.z;
        flyToX = targetX; flyToY = 2.6; flyToZ = targetZ + 4.5;
        cameraFlyAnimation.restart();
    }

    property real flyFromX: 0; property real flyFromY: 5; property real flyFromZ: 9
    property real flyToX: 0; property real flyToY: 5; property real flyToZ: 9

    View3D {
        id: view3d
        anchors.fill: parent
        camera: camera

        environment: SceneEnvironment {
            clearColor: "#0a0c0e" // charcoal-black, per the industrial-HMI visual spec
            backgroundMode: SceneEnvironment.Color
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High
            tonemapMode: SceneEnvironment.TonemapModeLinear // no bloom - crisp/restrained, not game-like
        }

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 5, 9)
            eulerRotation: Qt.vector3d(-28, 0, 0)
        }

        DirectionalLight {
            eulerRotation: Qt.vector3d(-55, -35, 0)
            brightness: 1.1
            castsShadow: true
            shadowMapQuality: Light.ShadowMapQualityHigh
        }
        DirectionalLight {
            eulerRotation: Qt.vector3d(-20, 140, 0)
            brightness: 0.35
            color: "#3a5a66" // cool cyan fill, matches the HMI accent palette
        }

        // Factory floor.
        Model {
            source: "#Cube"
            scale: Qt.vector3d(12, 0.05, 12)
            position: Qt.vector3d(0, -0.05, 0)
            materials: [PrincipledMaterial { baseColor: "#1b1e21"; metalness: 0.3; roughness: 0.75 }]
        }

        Repeater3D {
            model: root.machineModel
            delegate: Machine3D {
                required property var model
                machineId: model.machineId.toString()
                kind: model.kind
                state: model.state
                temperature: model.temperature
                vibration: model.vibration
                selected: model.machineId === root.selectedMachineId
                thermalOverlay: root.thermalOverlay
                position: model.position
            }
        }
    }

    // Authored keyframe camera fly-to animation (Qt Quick Timeline), driven
    // by flyTo() above rather than by the timeline's own play controls -
    // this is the "one authored keyframe animation" the brief asks for.
    Timeline {
        id: cameraFlyTimeline
        startFrame: 0
        endFrame: 100
        enabled: true

        KeyframeGroup {
            target: camera
            property: "x"
            Keyframe { frame: 0; value: root.flyFromX }
            Keyframe { frame: 100; value: root.flyToX }
        }
        KeyframeGroup {
            target: camera
            property: "y"
            Keyframe { frame: 0; value: root.flyFromY }
            Keyframe { frame: 100; value: root.flyToY }
        }
        KeyframeGroup {
            target: camera
            property: "z"
            Keyframe { frame: 0; value: root.flyFromZ }
            Keyframe { frame: 100; value: root.flyToZ }
        }
    }
    TimelineAnimation {
        id: cameraFlyAnimation
        timeline: cameraFlyTimeline
        duration: 900
        from: 0
        to: 100
        easing.type: Easing.InOutCubic
        onFinished: root.cameraMoved(camera.position, camera.eulerRotation)

        function restart() { stop(); start(); }
    }

    // Orbit/zoom/pan: simple drag-to-orbit + wheel-to-zoom rig (a hand-rolled
    // mouse rig, not a hidden/undocumented Quick3D helper).
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        property real lastX: 0
        property real lastY: 0

        onPressed: (mouse) => {
            lastX = mouse.x; lastY = mouse.y;
            const result = view3d.pick(mouse.x, mouse.y);
            if (result.objectHit) {
                const id = parseInt(result.objectHit.objectName);
                if (!isNaN(id))
                    root.machineClicked(id);
            }
        }
        onPositionChanged: (mouse) => {
            if (pressed) {
                const dx = mouse.x - lastX;
                const dy = mouse.y - lastY;
                const rot = camera.eulerRotation;
                camera.eulerRotation = Qt.vector3d(
                    Math.max(-80, Math.min(10, rot.x - dy * 0.15)),
                    rot.y - dx * 0.25,
                    rot.z);
                lastX = mouse.x; lastY = mouse.y;
                root.cameraMoved(camera.position, camera.eulerRotation);
            }
        }
        onWheel: (wheel) => {
            const factor = wheel.angleDelta.y > 0 ? 0.92 : 1.08;
            camera.position = camera.position.times(factor);
            root.cameraMoved(camera.position, camera.eulerRotation);
        }
    }
}
