// SPDX-License-Identifier: GPL-3.0-or-later
#include "AlarmAudioController.h"
#include "DemoConductor.h"
#include "MachineListModel.h"
#include "MachineTelemetrySimulator.h"
#include "MaintenanceWorkflow.h"
#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_GRAPHS
#include "SurfaceGridBridge.h"
#endif

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

using namespace qttutorial::offboard_digital_twin;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Qt Nexus - Digital Twin Control Center"));
    app.setOrganizationName(QStringLiteral("QtTutorial"));

    auto *simulator = new MachineTelemetrySimulator(&app);
    auto *machineModel = new MachineListModel(simulator, &app);
    auto *demoConductor = new DemoConductor(&app);
    auto *alarmAudio = new AlarmAudioController(&app);
    // One maintenance workflow instance, targeting whichever machine is
    // currently selected/overheating; the demo scenario always exercises it
    // against CNC Mill B2 (machine id 1).
    auto *maintenanceWorkflow = new MaintenanceWorkflow(1, &app);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("telemetrySimulator"), simulator);
    engine.rootContext()->setContextProperty(QStringLiteral("machineModel"), machineModel);
    engine.rootContext()->setContextProperty(QStringLiteral("demoConductor"), demoConductor);
    engine.rootContext()->setContextProperty(QStringLiteral("alarmAudio"), alarmAudio);
    engine.rootContext()->setContextProperty(QStringLiteral("maintenanceWorkflow"), maintenanceWorkflow);
#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_GRAPHS
    engine.rootContext()->setContextProperty(QStringLiteral("graphsAvailable"), true);
    auto *surfaceGridBridge = new SurfaceGridBridge(&app);
    engine.rootContext()->setContextProperty(QStringLiteral("surfaceGridBridge"), surfaceGridBridge);
#else
    engine.rootContext()->setContextProperty(QStringLiteral("graphsAvailable"), false);
#endif

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() {
        QCoreApplication::exit(-1);
    });

    simulator->start();

    // engine.loadFromModule() needs Qt >= 6.5; this repository's local Qt
    // 6.4.2 baseline uses the qrc: URL form instead, exactly like
    // framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/src/main.cpp
    // and industries/games/kicker/src/main.cpp already do for the same reason.
    engine.load(QUrl(QStringLiteral(
        "qrc:/QtTutorial/Factory/OffboardDigitalTwinControlCenter/qml/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
