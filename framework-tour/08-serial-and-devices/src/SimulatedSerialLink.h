// SPDX-License-Identifier: MIT
#pragma once

#include <QObject>

class QLocalServer;
class QLocalSocket;

namespace qttutorial::serial_and_devices {

// SIMULATED transport, NOT real hardware.
//
// This class does not talk to any physical serial port. It creates a real,
// genuinely asynchronous local IPC byte pipe (a QLocalServer plus two connected
// QLocalSocket endpoints) and hands out both ends so that higher-level code can
// exercise real framing/checksum/timeout logic over a real byte stream, without a
// physical cable or device attached.
//
// Both endpoints are QLocalSocket, which is a QIODevice, so any code written
// against QIODevice& (see DeviceLink) works unmodified if clientEnd()/deviceEnd()
// are later replaced with a real QSerialPort instance talking to actual hardware.
class SimulatedSerialLink : public QObject {
    Q_OBJECT
public:
    explicit SimulatedSerialLink(QObject* parent = nullptr);
    ~SimulatedSerialLink() override;

    bool isConnected() const;

    // The "controller" side, e.g. what a desktop application would hold.
    QLocalSocket* clientEnd() const { return m_clientSocket; }

    // The "device" side, e.g. what firmware on the other end of the wire would hold.
    QLocalSocket* deviceEnd() const { return m_deviceSocket; }

private:
    QLocalServer* m_server;
    QLocalSocket* m_clientSocket;
    QLocalSocket* m_deviceSocket = nullptr;
};

} // namespace qttutorial::serial_and_devices
