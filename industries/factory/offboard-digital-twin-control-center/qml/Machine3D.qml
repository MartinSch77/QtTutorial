// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick3D
import QtQuick3D.Particles3D
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// One procedurally-built machine (no glTF assets - this sandbox has no
// fetchable 3D model files, per the brief). Built from Quick3D's built-in
// primitive meshes (#Cube/#Cylinder/#Cone) composed into a machine-like
// shape, with real PBR PrincipledMaterial and a simple continuous animated
// part (rotating roller/fan), matching framework-tour/10's
// quick3d-physics-xr precedent of primitive-only Quick3D scenes.
Node {
    id: root

    property string machineId: ""
    property string kind: "cnc-mill"
    property string state: "normal"
    property bool selected: false
    property bool thermalOverlay: false
    property real temperature: 25
    property real vibration: 0.2

    readonly property color baseColor: thermalOverlay
        ? Qt.rgba(Math.min(1, temperature / 90), Math.max(0, 1 - temperature / 90), 0.25, 1)
        : Theme.colorForState(state)

    // Semi-transparent "reveal internals" mode while selected/inspected.
    // Not readonly: a Behavior needs write access to animate this property,
    // even though its value is still fully driven by the binding below.
    property real bodyOpacity: selected ? 0.35 : 1.0

    Behavior on bodyOpacity { NumberAnimation { duration: Theme.transitionMs } }

    PrincipledMaterial {
        id: bodyMaterial
        baseColor: root.baseColor
        metalness: 0.65
        roughness: 0.35
        opacity: root.bodyOpacity

        Behavior on baseColor { ColorAnimation { duration: 2600 } } // "blue -> orange/red" overheat ramp
    }

    PrincipledMaterial {
        id: internalsMaterial
        baseColor: "#9aa4ad"
        metalness: 0.2
        roughness: 0.55
        opacity: root.selected ? 0.95 : 0.0
        Behavior on opacity { NumberAnimation { duration: Theme.transitionMs } }
    }

    PrincipledMaterial {
        id: defectMaterial
        baseColor: "#ff5540"
        emissiveFactor: Qt.vector3d(0.9, 0.18, 0.05) // verified emissive-color highlight, see README
        metalness: 0.1
        roughness: 0.4
    }

    // --- Machine housing, procedurally shaped per `kind` -------------------
    Model {
        source: "#Cube"
        scale: Qt.vector3d(1.6, 0.9, 1.1)
        position: Qt.vector3d(0, 0.45, 0)
        materials: [bodyMaterial]
        pickable: true
        objectName: root.machineId
    }

    Model {
        visible: root.kind === "cnc-mill" || root.kind === "press"
        source: "#Cylinder"
        scale: Qt.vector3d(0.3, 0.6, 0.3)
        position: Qt.vector3d(0, 1.1, 0)
        materials: [bodyMaterial]
        pickable: true
        objectName: root.machineId
    }

    Model {
        id: spinner
        visible: root.kind === "conveyor" || root.kind === "fan-array"
        source: root.kind === "fan-array" ? "#Cone" : "#Cylinder"
        scale: Qt.vector3d(0.35, 0.35, 0.9)
        eulerRotation: root.kind === "conveyor" ? Qt.vector3d(0, 0, 90) : Qt.vector3d(90, 0, 0)
        position: Qt.vector3d(0.85, 0.45, 0)
        materials: [bodyMaterial]
        pickable: true
        objectName: root.machineId

        // Simple animated machinery: a rotating conveyor roller / spinning
        // fan blade transform animation, per the brief's step 1.
        PropertyAnimation on eulerRotation.y {
            running: root.state !== "stopped"
            loops: Animation.Infinite
            from: 0; to: 360
            duration: root.kind === "fan-array" ? 700 : 2200
        }
    }

    // --- Exploded/transparent interior reveal (procedural, no assets) -----
    Model {
        source: "#Sphere"
        scale: Qt.vector3d(0.18, 0.18, 0.18)
        position: root.selected ? Qt.vector3d(0, 0.65, 0.5) : Qt.vector3d(0, 0.55, 0)
        materials: [internalsMaterial]
        Behavior on position { NumberAnimation { duration: 500; easing.type: Easing.OutCubic } }
    }

    Model {
        id: defectivePart
        visible: root.selected
        source: "#Cube"
        scale: Qt.vector3d(0.12, 0.12, 0.12)
        position: root.selected ? Qt.vector3d(0.3, 0.85, 0.35) : Qt.vector3d(0.3, 0.45, 0)
        materials: [defectMaterial]
        Behavior on position { NumberAnimation { duration: 500; easing.type: Easing.OutCubic } }

        SequentialAnimation on opacity {
            running: root.selected
            loops: Animation.Infinite
            NumberAnimation { from: 1.0; to: 0.4; duration: 500 }
            NumberAnimation { from: 0.4; to: 1.0; duration: 500 }
        }
    }

    // --- Heat-shimmer / airflow / vibration particles ----------------------
    ParticleSystem3D {
        id: heatParticles
        running: root.state === "alarm" || root.state === "warning"
        position: Qt.vector3d(0, 1.3, 0)

        SpriteParticle3D {
            id: heatSprite
            color: root.state === "alarm" ? "#ff6a3d" : "#e0a72a"
            colorVariation: Qt.vector4d(0.1, 0.1, 0.1, 0.2)
            particleScale: 6
            blendMode: SpriteParticle3D.Screen
            billboard: true
        }

        ParticleEmitter3D {
            particle: heatSprite
            emitRate: root.state === "alarm" ? 40 : 15
            lifeSpan: 1400
            lifeSpanVariation: 300
            velocity: VectorDirection3D {
                direction: Qt.vector3d(0, 1, 0)
                directionVariation: Qt.vector3d(0.3, 0.2, 0.3)
            }
            particleScaleVariation: 0.4
            shape: ParticleShape3D { type: ParticleShape3D.Cube; extents: Qt.vector3d(0.7, 0.1, 0.5) }
        }
    }
}
