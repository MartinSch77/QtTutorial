// SPDX-License-Identifier: MIT
#include "MachinePanelApp.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qttutorial::factory_machine_panel::MachinePanelApp panelApp;

    QQmlApplicationEngine engine;
    QQmlContext* rootContext = engine.rootContext();
    rootContext->setContextProperty("machineSim", panelApp.simulator());
    rootContext->setContextProperty("machineState", panelApp.controller());
    rootContext->setContextProperty("eventLog", panelApp.eventLog());
    rootContext->setContextProperty("panelApp", &panelApp);

    // qt_add_qml_module resolves QML_FILES relative to their source path
    // under a resource prefix derived from the module URI; loadFromModule()
    // (Qt 6.5+) is not available on the Qt 6.4 baseline this tutorial
    // targets, so we load the qrc URL directly.
    engine.load(QUrl(QStringLiteral("qrc:/Factory/OnboardMachinePanel/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
