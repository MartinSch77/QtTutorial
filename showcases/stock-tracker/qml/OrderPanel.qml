// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls.Basic
import Qt.labs.lottieqt 1.0
import QtTutorial.StockTracker

// The order-entry panel: binds to a single OrderLifecycle instance
// (StockTrackerBackend.currentOrder) owned entirely in C++. QML only reads its
// properties and calls its slots (submit/cancel/reset) - the guard logic that
// decides whether submit() actually transitions the state machine lives in
// OrderLifecycle::buildStateMachine(), not here.
//
// `order` is declared `property var`, not a concrete OrderLifecycle type
// annotation: OrderLifecycle deliberately has no QML_ELEMENT registration (it
// lives in the headless, QML-independent stock_tracker_lib), so its C++ type
// name is unknown to the QML type system here. Its properties/slots are still
// fully accessible through the object pointer via normal QObject introspection.
Item {
    id: root
    required property StockTrackerBackend backend
    readonly property var order: backend.currentOrder

    implicitHeight: column.implicitHeight

    Column {
        id: column
        width: parent.width
        spacing: 10

        Label {
            text: qsTr("Order")
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }

        OrderStateBadge {
            state: root.order.currentState
        }

        Row {
            spacing: 8
            width: parent.width
            enabled: root.order.currentState === "Draft"

            ComboBox {
                id: symbolCombo
                width: 90
                model: ["QTX", "BLUE", "NOVA", "HRZN"]
                onActivated: root.order.symbol = currentText
                Component.onCompleted: root.order.symbol = currentText
            }
            SpinBox {
                id: quantitySpin
                width: 100
                from: 0
                to: 2000
                stepSize: 10
                value: 100
                onValueModified: root.order.quantity = value
                Component.onCompleted: root.order.quantity = value
            }
            Button {
                text: root.order.isBuy ? qsTr("BUY") : qsTr("SELL")
                onClicked: root.order.isBuy = !root.order.isBuy
            }
        }

        Row {
            spacing: 8

            Button {
                text: qsTr("Submit")
                enabled: root.order.currentState === "Draft" && root.order.isValid
                onClicked: backend.submitCurrentOrder()
            }
            Button {
                text: qsTr("Cancel")
                enabled: root.order.currentState === "Submitted"
                onClicked: root.order.cancel()
            }
            Button {
                text: qsTr("New order")
                enabled: root.order.currentState === "Filled" || root.order.currentState === "Cancelled"
                         || root.order.currentState === "Rejected"
                onClicked: root.order.reset()
            }
        }

        Text {
            text: root.order.isValid
                ? qsTr("Ready to submit %1 x %2").arg(quantitySpin.value).arg(symbolCombo.currentText)
                : qsTr("Pick a symbol and quantity > 0 before submitting")
            color: "#8fa0b3"
            font.pixelSize: 11
            visible: root.order.currentState === "Draft"
        }

        Item {
            width: parent.width
            height: 90
            visible: root.order.currentState === "Filled"

            LottieAnimation {
                id: filledConfirmation
                anchors.centerIn: parent
                width: 90
                height: 90
                source: "qrc:/stock_tracker/lottie/order-filled.json"
                loops: 1
                autoPlay: false
                onStatusChanged: if (status === LottieAnimation.Ready) start()
            }

            Connections {
                target: root.order
                function onStateChanged(stateName) {
                    if (stateName === "Filled" && filledConfirmation.status === LottieAnimation.Ready) {
                        filledConfirmation.start();
                    }
                }
            }
        }
    }
}
