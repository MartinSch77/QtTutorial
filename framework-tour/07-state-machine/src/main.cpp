// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    qttutorial::state_machine::MainWindow window;
    window.show();
    return app.exec();
}
