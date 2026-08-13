// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    qttutorial::automotive::fleet::MainWindow window;
    window.show();
    return app.exec();
}
