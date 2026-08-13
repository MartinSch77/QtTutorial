// SPDX-License-Identifier: GPL-3.0-or-later
//
// Second, deliberately separate entry point using QtQuick3D.Xr (Technology
// Preview since Qt 6.8). It wraps the same physics scene as Main.qml/View3D
// but as an actual XR scene rooted at XrView instead of a desktop View3D.
//
// This file is NOT wired into this module's qt_add_qml_module() QML_FILES
// list and is not exercised by any build or test in this repository: running
// it for real requires an XR runtime and headset (e.g. via OpenXR) that this
// sandbox - and this repository's CI - does not have. It is kept here,
// documented, as the XR-ready counterpart of the desktop demo. See
// ../README.md for details.
import QtQuick
import QtQuick3D
import QtQuick3D.Physics
import QtQuick3D.Xr
import QtTutorial.CustomRenderingXr.Quick3DPhysicsXr

XrView {
    id: xrView
    referenceSpace: XrView.ReferenceSpaceLocalFloor

    environment: SceneEnvironment {
        clearColor: "black"
        backgroundMode: SceneEnvironment.Color
    }

    onInitializeFailed: (errorString) => console.warn("XrView failed to initialize:", errorString)

    xrOrigin: XrOrigin {
        id: origin

        DirectionalLight {
            eulerRotation: Qt.vector3d(-40, -30, 0)
            brightness: 1.2
        }

        StaticRigidBody {
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
            model: DropLayoutProvider.gridPositions(6, 40, 420)

            DynamicRigidBody {
                position: modelData
                collisionShapes: BoxShape {
                    extents: Qt.vector3d(80, 80, 80)
                }

                Model {
                    source: "#Cube"
                    scale: Qt.vector3d(0.8, 0.8, 0.8)
                    materials: PrincipledMaterial {
                        baseColor: Qt.hsla(index / 6.0, 0.65, 0.55, 1.0)
                    }
                }
            }
        }
    }

    PhysicsWorld {
        scene: origin
        gravity: Qt.vector3d(0, -981, 0)
    }
}
