// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    qttutorial::avionics::MainWindow window;
    window.showFullScreen();
    return app.exec();
}
