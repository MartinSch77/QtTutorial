// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick3D

// The 3D table: a low-poly, stylized foosball table (built-in "#Cube" /
// "#Cylinder" / "#Sphere" / "#Cone" primitive meshes, no imported assets)
// rendered with real perspective, a directional light and shadows. World
// units match the kicker_lib physics units directly (centimetres):
// x is the slide axis, z is the goal-to-goal axis, y is "up".
Item {
    id: root

    property real ballX: 0
    property real ballZ: 0
    property real rod0Slide: 0
    property real rod0Rotation: 0
    property real rod1Slide: 0
    property real rod1Rotation: 0
    property real rod2Slide: 0
    property real rod2Rotation: 0
    property real rod3Slide: 0
    property real rod3Rotation: 0

    readonly property real halfWidth: 34
    readonly property real halfLength: 60
    readonly property real goalHalfWidth: 10
    readonly property real tableHeight: 0
    readonly property real ballRadius: 1.5
    readonly property var rodZ: [-42, -15, 15, 42]

    View3D {
        anchors.fill: parent

        environment: SceneEnvironment {
            clearColor: "#12151a"
            backgroundMode: SceneEnvironment.Color
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High
        }

        PerspectiveCamera {
            position: Qt.vector3d(0, 95, 105)
            eulerRotation: Qt.vector3d(-52, 0, 0)
            fieldOfView: 55
        }

        DirectionalLight {
            eulerRotation: Qt.vector3d(-55, -25, 0)
            brightness: 1.2
            castsShadow: true
            shadowMapQuality: Light.ShadowMapQualityHigh
        }

        Model {
            source: "#Cube"
            position: Qt.vector3d(0, -2, 0)
            scale: Qt.vector3d(root.halfWidth * 2 / 100, 0.08, root.halfLength * 2 / 100)
            materials: PrincipledMaterial {
                baseColor: "#2e7d32"
                roughness: 0.85
            }
            receivesShadows: true
        }

        Model {
            source: "#Cube"
            position: Qt.vector3d(-root.halfWidth - 1.5, 2, 0)
            scale: Qt.vector3d(0.03, 0.08, root.halfLength * 2 / 100)
            materials: PrincipledMaterial { baseColor: "#4e342e"; roughness: 0.6 }
        }
        Model {
            source: "#Cube"
            position: Qt.vector3d(root.halfWidth + 1.5, 2, 0)
            scale: Qt.vector3d(0.03, 0.08, root.halfLength * 2 / 100)
            materials: PrincipledMaterial { baseColor: "#4e342e"; roughness: 0.6 }
        }

        Repeater3D {
            model: 2
            Model {
                readonly property real endSign: index === 0 ? -1 : 1
                source: "#Cube"
                position: Qt.vector3d((root.halfWidth + root.goalHalfWidth) / 2 * endSign, 2,
                                       root.halfLength * endSign)
                scale: Qt.vector3d((root.halfWidth - root.goalHalfWidth) / 100, 0.08, 0.03)
                materials: PrincipledMaterial { baseColor: "#4e342e"; roughness: 0.6 }
            }
        }

        Model {
            id: ball
            source: "#Sphere"
            position: Qt.vector3d(root.ballX, root.ballRadius, root.ballZ)
            scale: Qt.vector3d(root.ballRadius * 2 / 100, root.ballRadius * 2 / 100, root.ballRadius * 2 / 100)
            materials: PrincipledMaterial {
                baseColor: "#f5f5f5"
                roughness: 0.35
            }
            castsShadows: true
        }

        Repeater3D {
            model: 4
            Model {
                readonly property real rodZ: root.rodZ[index]
                source: "#Cylinder"
                position: Qt.vector3d(0, 6, rodZ)
                eulerRotation: Qt.vector3d(0, 0, 90)
                scale: Qt.vector3d(0.02, root.halfWidth * 2 / 100, 0.02)
                materials: PrincipledMaterial { baseColor: "#9e9e9e"; metalness: 0.8; roughness: 0.3 }
            }
        }

        // Flattened over all 12 figures (4 rods x 3 each) rather than a
        // nested Repeater3D, so each figure's rod-level data (slide,
        // rotation, team color) can be looked up directly from `root` by
        // rodIndex without relying on Repeater3D's node-parenting details.
        Repeater3D {
            model: 12
            Node {
                id: figureNode
                readonly property int rodIndex: Math.floor(index / 3)
                readonly property int figureIndex: index % 3
                readonly property real rodSlide: [root.rod0Slide, root.rod1Slide, root.rod2Slide, root.rod3Slide][rodIndex]
                readonly property real rodRotation: [root.rod0Rotation, root.rod1Rotation, root.rod2Rotation, root.rod3Rotation][rodIndex]
                readonly property real offset: (figureIndex - 1) * 9
                readonly property color teamColor: rodIndex < 2 ? "#c62828" : "#1565c0"

                position: Qt.vector3d(rodSlide + offset, 6, root.rodZ[rodIndex])
                eulerRotation: Qt.vector3d(rodRotation * 180 / Math.PI, 0, 0)

                Model {
                    source: "#Cylinder"
                    scale: Qt.vector3d(0.06, 0.05, 0.06)
                    // Not "parent.teamColor": this material's QQuick3DObject
                    // "parent" is the immediately enclosing Model, not the
                    // outer Node two levels up that actually declares
                    // "teamColor" - that mismatch produced a real
                    // "Unable to assign [undefined] to QColor" at runtime
                    // for every one of the 12 figures, confirmed by
                    // actually running this app. figureNode.teamColor
                    // resolves unambiguously regardless of nesting depth.
                    materials: PrincipledMaterial { baseColor: figureNode.teamColor; roughness: 0.5 }
                    castsShadows: true
                }
                Model {
                    source: "#Cube"
                    position: Qt.vector3d(0, -2.5, 2)
                    scale: Qt.vector3d(0.02, 0.015, 0.08)
                    materials: PrincipledMaterial { baseColor: "#212121"; roughness: 0.6 }
                    castsShadows: true
                }
            }
        }
    }
}
