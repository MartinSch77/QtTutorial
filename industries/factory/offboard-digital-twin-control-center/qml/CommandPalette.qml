// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Global search / command palette: type a machine name or a command
// ("theme dark", "play demo") and press Enter. Kept intentionally small -
// a real product's palette would fuzzy-match a much larger command set.
Rectangle {
    id: root
    color: Theme.glassSurface
    border.color: Theme.border
    border.width: 1
    radius: Theme.radius
    height: visible ? 44 : 0
    property bool expanded: false

    property var machineModel

    signal machineChosen(int machineId)
    signal commandChosen(string command)

    Row {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 8

        TextField {
            id: field
            width: root.width - 100
            placeholderText: qsTr("Search machines or type a command (e.g. \"theme light\")")
            color: Theme.textPrimary
            onAccepted: root.runQuery(text)
        }

        Button {
            text: qsTr("Go")
            onClicked: root.runQuery(field.text)
        }
    }

    function runQuery(query) {
        const q = query.trim().toLowerCase();
        if (q.length === 0)
            return;

        if (q.startsWith("theme ")) {
            root.commandChosen("theme:" + q.substring(6).trim());
            field.text = "";
            return;
        }
        if (q === "play demo" || q === "demo") {
            root.commandChosen("play-demo");
            field.text = "";
            return;
        }

        for (let i = 0; i < root.machineModel.rowCount(); ++i) {
            const idx = root.machineModel.index(i, 0);
            const name = root.machineModel.data(idx, Qt.UserRole + 2); // NameRole
            if (String(name).toLowerCase().includes(q)) {
                const id = root.machineModel.data(idx, Qt.UserRole + 1); // IdRole
                root.machineChosen(id);
                field.text = "";
                return;
            }
        }
    }
}
