// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick3D
import QtQuick3D.Physics
import QtTutorial.CustomRenderingXr.Quick3DPhysicsXr

Window {
    id: window
    width: 800
    height: 600
    visible: true
    title: qsTr("Qt Quick 3D Physics - falling rigid bodies on a static floor")

    property real bodyRadius: 40

    View3D {
        anchors.fill: parent
        camera: camera

        environment: SceneEnvironment {
            clearColor: "#1c2026"
            backgroundMode: SceneEnvironment.Color
        }

        PerspectiveCamera {
            id: camera
            position: Qt.vector3d(0, 420, 900)
            eulerRotation.x: -18
        }

        DirectionalLight {
            eulerRotation: Qt.vector3d(-40, -30, 0)
            brightness: 1.2
        }

        Node {
            id: physicsScene

            PhysicsWorld {
                scene: physicsScene
                gravity: Qt.vector3d(0, -981, 0)
            }

            StaticRigidBody {
                position: Qt.vector3d(0, 0, 0)
                eulerRotation: Qt.vector3d(-90, 0, 0)
                collisionShapes: PlaneShape {}

                Model {
                    source: "#Rectangle"
                    scale: Qt.vector3d(8, 8, 1)
                    materials: PrincipledMaterial {
                        baseColor: "#39404c"
                    }
                }
            }

            Repeater3D {
                model: DropLayoutProvider.gridPositions(6, window.bodyRadius, 420)

                DynamicRigidBody {
                    position: modelData
                    collisionShapes: BoxShape {
                        extents: Qt.vector3d(window.bodyRadius * 2, window.bodyRadius * 2,
                                              window.bodyRadius * 2)
                    }

                    Model {
                        source: "#Cube"
                        scale: Qt.vector3d(window.bodyRadius / 50, window.bodyRadius / 50,
                                            window.bodyRadius / 50)
                        materials: PrincipledMaterial {
                            baseColor: Qt.hsla(index / 6.0, 0.65, 0.55, 1.0)
                        }
                    }
                }
            }
        }
    }
}
