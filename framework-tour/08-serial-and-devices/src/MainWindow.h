// SPDX-License-Identifier: MIT
#pragma once

#include "DeviceLink.h"
#include "SimulatedSerialLink.h"

#include <QMainWindow>

class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QPushButton;

namespace qttutorial::serial_and_devices {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void rescanPorts();
    void sendFramedMessage();
    void sendCorruptedFrame();

private:
    void log(const QString& line);

    QListWidget* m_portList;
    QPushButton* m_rescanButton;
    QLineEdit* m_messageEdit;
    QPushButton* m_sendButton;
    QPushButton* m_corruptButton;
    QPlainTextEdit* m_log;

    // The "wire": a labeled simulation, not real hardware. See SimulatedSerialLink.h.
    SimulatedSerialLink* m_link;
    DeviceLink* m_controllerLink; // Sits on the application/controller end.
    DeviceLink* m_deviceSideLink; // Sits on the simulated device end and echoes back.
};

} // namespace qttutorial::serial_and_devices
