// SPDX-License-Identifier: MIT
#include "HmiApp.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qttutorial::industrial_hmi::HmiApp hmiApp;

    QQmlApplicationEngine engine;
    QQmlContext* rootContext = engine.rootContext();
    rootContext->setContextProperty("processSim", hmiApp.process());
    rootContext->setContextProperty("pump", hmiApp.pump());
    rootContext->setContextProperty("alarmLog", hmiApp.alarms());
    rootContext->setContextProperty("hmiApp", &hmiApp);

    // qt_add_qml_module resolves QML_FILES relative to their source path
    // under a resource prefix derived from the module URI; loadFromModule()
    // (Qt 6.5+) is not available on the Qt 6.4 baseline this tutorial
    // targets, so we load the qrc URL directly.
    engine.load(QUrl(QStringLiteral("qrc:/IndustrialAutomation/OnboardHmiPanel/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
