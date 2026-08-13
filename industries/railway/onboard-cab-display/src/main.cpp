// SPDX-License-Identifier: MIT
#include "TrainSimulator.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

namespace {

qttutorial::cab_display::RouteProfile buildDemoRoute()
{
    using qttutorial::cab_display::SpeedSegment;
    using qttutorial::cab_display::Station;

    std::vector<SpeedSegment> segments{
        {0.0, 1800.0, 140.0},
        {1800.0, 2200.0, 80.0},
        // A steep downhill grade on the approach to the 60 km/h restriction
        // ahead: the flat-track braking curve still sets the permitted-speed
        // target from here, but the train's real braking authority is
        // reduced on this grade, so the brake-warning indication can
        // genuinely (briefly) latch on — the same divergence between
        // advisory curve and physical reality a real ETCS-fitted line's
        // brake-intervention supervision exists to catch.
        {2200.0, 6000.0, 140.0, -6.0},
        {6000.0, 6400.0, 60.0},
        {6400.0, 9000.0, 140.0},
    };
    std::vector<Station> stations{
        {"Riverside", 4000.0},
        {"Uptown", 8600.0},
    };
    return qttutorial::cab_display::RouteProfile(std::move(segments), std::move(stations));
}

} // namespace

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    qttutorial::cab_display::TrainSimulator train(buildDemoRoute());

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("train", &train);

    // qt_add_qml_module resolves QML_FILES relative to their source path
    // under a resource prefix derived from the module URI; loadFromModule()
    // (Qt 6.5+) is not available on the Qt 6.4 baseline this tutorial
    // targets, so we load the qrc URL directly.
    engine.load(QUrl(QStringLiteral("qrc:/Railway/OnboardCabDisplay/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
