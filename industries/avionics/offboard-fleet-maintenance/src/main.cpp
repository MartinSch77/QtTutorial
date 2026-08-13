// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("FleetMaintenanceDashboard"));
    qttutorial::avionics::MainWindow window;
    window.show();
    return app.exec();
}
