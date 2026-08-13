// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    qttutorial::serial_and_devices::MainWindow window;
    window.show();
    return app.exec();
}
