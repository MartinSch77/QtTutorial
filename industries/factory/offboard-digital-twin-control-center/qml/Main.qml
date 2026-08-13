// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls.Basic
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Qt Nexus - Digital Twin Control Center.
//
// Top bar: system-online Lottie icon, global search/command palette, theme
// switcher, "Play demo" button.
// Center: 3D factory scene (left = equipment tree, center = View3D,
// right = live machine panel).
// Bottom: charts/alarms + the maintenance-workflow panel.
ApplicationWindow {
    id: window
    width: 1400
    height: 860
    visible: true
    title: qsTr("Qt Nexus - Digital Twin Control Center")
    color: Theme.background

    property int selectedMachineId: -1
    property var selectedMachine: null

    // Captures the "machineModel" root-context property (set via
    // engine.rootContext()->setContextProperty() in main.cpp) under a
    // distinct name. FactoryScene3D/EquipmentTree/CommandPalette below each
    // declare their OWN "property var machineModel" - writing
    // "machineModel: machineModel" directly in their object literals
    // resolves the unqualified RHS to that same component's own property
    // first (self-shadowing), not the outer context property, producing a
    // genuine "Binding loop detected for property machineModel" at
    // runtime. "window" has no property named "machineModel" itself, so
    // this line is unambiguous, and "window.globalMachineModel" below is
    // unambiguous from any scope.
    property var globalMachineModel: machineModel

    function machineAt(index) {
        if (index < 0 || index >= machineModel.rowCount())
            return null;
        const idx = machineModel.index(index, 0);
        return {
            machineId: machineModel.data(idx, Qt.UserRole + 1),
            name: machineModel.data(idx, Qt.UserRole + 2),
            kind: machineModel.data(idx, Qt.UserRole + 3),
            position: machineModel.data(idx, Qt.UserRole + 4),
            temperature: machineModel.data(idx, Qt.UserRole + 5),
            vibration: machineModel.data(idx, Qt.UserRole + 6),
            speed: machineModel.data(idx, Qt.UserRole + 7),
            state: machineModel.data(idx, Qt.UserRole + 8),
            defectivePart: machineModel.data(idx, Qt.UserRole + 9),
        };
    }

    function selectMachine(machineId) {
        window.selectedMachineId = machineId;
        window.selectedMachine = window.machineAt(machineModel.indexOfMachineId(machineId));
        if (window.selectedMachine)
            scene3D.flyTo(window.selectedMachine.position.x, window.selectedMachine.position.z);
    }

    function refreshSelectedMachine() {
        if (window.selectedMachineId >= 0)
            window.selectedMachine = window.machineAt(machineModel.indexOfMachineId(window.selectedMachineId));
    }

    Connections {
        target: telemetrySimulator
        function onMachineTelemetryChanged(machineId) {
            if (machineId === window.selectedMachineId)
                window.refreshSelectedMachine();
            if (alarmAudio.active && machineId === window.selectedMachineId)
                alarmAudio.startAlarm(window.selectedMachine.position);
        }
    }

    // --- Demo conductor wiring (the automated "Play demo" walkthrough) ----
    Connections {
        target: demoConductor
        function onRequestExplore() {
            window.selectMachine(machineModel.data(machineModel.index(0, 0), Qt.UserRole + 1));
        }
        function onRequestOverheat(machineId) {
            window.selectMachine(machineId);
            telemetrySimulator.beginOverheat(machineId);
            alarmAudio.startAlarm(window.selectedMachine.position);
        }
        function onRequestInspect(machineId) {
            window.selectMachine(machineId);
            thermalOverlayEnabled = true;
        }
        function onRequestAnalyze(machineId) {
            window.selectMachine(machineId);
        }
        function onRequestMaintenance(machineId) {
            window.selectMachine(machineId);
            maintenanceWorkflow.start();
        }
        function onRequestThemeSwitch(themeName) {
            Theme.mode = themeName;
        }
        function onFinished() {
            alarmAudio.stopAlarm();
            telemetrySimulator.clearFault(window.selectedMachineId);
        }
    }

    property bool thermalOverlayEnabled: false

    Column {
        anchors.fill: parent
        anchors.margins: Theme.spacingUnit
        spacing: Theme.spacingUnit

        // --- Top bar --------------------------------------------------
        Rectangle {
            width: parent.width
            height: 56
            radius: Theme.radius
            color: Theme.glassSurface
            border.color: Theme.border
            border.width: 1

            Row {
                anchors.fill: parent
                anchors.margins: 8
                spacing: Theme.spacingUnit

                StatusIcon {}

                Text {
                    text: qsTr("QT NEXUS")
                    color: Theme.textPrimary
                    font.bold: true
                    font.pixelSize: Theme.fontSizeHeading
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: demoConductor.playing ? demoConductor.narration : qsTr("Factory Digital Twin Control Center")
                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSizeBody - 1
                    anchors.verticalCenter: parent.verticalCenter
                    elide: Text.ElideRight
                    width: 380
                }

                CommandPalette {
                    width: 380
                    height: 40
                    machineModel: window.globalMachineModel
                    onMachineChosen: (machineId) => window.selectMachine(machineId)
                    onCommandChosen: (command) => {
                        if (command === "play-demo") { demoConductor.play(); return; }
                        if (command.startsWith("theme:")) { Theme.mode = command.substring(6); return; }
                    }
                }

                Button {
                    text: demoConductor.playing ? qsTr("Stop demo") : qsTr("Play demo")
                    onClicked: demoConductor.playing ? demoConductor.stop() : demoConductor.play()
                }

                ThemeSwitcher {}
            }
        }

        // --- Main 3-pane layout ---------------------------------------
        Row {
            width: parent.width
            height: parent.height * (Theme.isTablet ? 0.55 : 0.62)
            spacing: Theme.spacingUnit

            EquipmentTree {
                width: Theme.isTablet ? 0 : parent.width * 0.18
                visible: !Theme.isTablet
                height: parent.height
                machineModel: window.globalMachineModel
                selectedMachineId: window.selectedMachineId
                onMachineSelected: (machineId) => window.selectMachine(machineId)
            }

            FactoryScene3D {
                id: scene3D
                width: Theme.isTablet ? parent.width * 0.7 : parent.width * 0.56
                height: parent.height
                machineModel: window.globalMachineModel
                selectedMachineId: window.selectedMachineId
                thermalOverlay: window.thermalOverlayEnabled
                onMachineClicked: (machineId) => window.selectMachine(machineId)
                onCameraMoved: (position, forward) => alarmAudio.updateListener(position, forward)
            }

            MachinePanel {
                width: Theme.isTablet ? parent.width * 0.3 : parent.width * 0.24
                height: parent.height
                machine: window.selectedMachine
                thermalOverlay: window.thermalOverlayEnabled
                onThermalOverlayToggled: (enabled) => window.thermalOverlayEnabled = enabled
                onStartMaintenanceRequested: maintenanceWorkflow.start()
            }
        }

        // --- Bottom: charts + maintenance workflow ---------------------
        Row {
            width: parent.width
            height: parent.height * (Theme.isTablet ? 0.33 : 0.3)
            spacing: Theme.spacingUnit

            ChartsPanel {
                width: parent.width * 0.65
                height: parent.height
                machine: window.selectedMachine
            }

            MaintenancePanel {
                width: parent.width * 0.34
                height: parent.height
                workflow: maintenanceWorkflow
            }
        }
    }
}
