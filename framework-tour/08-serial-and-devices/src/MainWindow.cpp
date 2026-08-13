// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "FrameCodec.h"

#include <QDateTime>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QLocalSocket>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::serial_and_devices {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_portList(new QListWidget(this))
    , m_rescanButton(new QPushButton(tr("Rescan ports"), this))
    , m_messageEdit(new QLineEdit(this))
    , m_sendButton(new QPushButton(tr("Send framed message"), this))
    , m_corruptButton(new QPushButton(tr("Send corrupted frame"), this))
    , m_log(new QPlainTextEdit(this))
    , m_link(new SimulatedSerialLink(this))
    , m_controllerLink(new DeviceLink(m_link->clientEnd(), 800, this))
    , m_deviceSideLink(new DeviceLink(m_link->deviceEnd(), 800, this))
{
    setWindowTitle(tr("Qt Serial & Devices"));

    auto* portsBox = new QGroupBox(tr("Real hardware scan (QSerialPortInfo)"), this);
    auto* portsLayout = new QVBoxLayout(portsBox);
    portsLayout->addWidget(m_portList);
    portsLayout->addWidget(m_rescanButton);

    auto* linkBox = new QGroupBox(tr("Simulated link (QLocalSocket loopback, not real hardware)"), this);
    auto* linkLayout = new QVBoxLayout(linkBox);
    m_messageEdit->setPlaceholderText(tr("Message to frame and send"));
    m_messageEdit->setText(tr("hello device"));
    auto* buttonRow = new QHBoxLayout;
    buttonRow->addWidget(m_sendButton);
    buttonRow->addWidget(m_corruptButton);
    linkLayout->addWidget(m_messageEdit);
    linkLayout->addLayout(buttonRow);
    linkLayout->addWidget(m_log);
    m_log->setReadOnly(true);

    auto* central = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->addWidget(portsBox);
    rootLayout->addWidget(linkBox);
    setCentralWidget(central);

    connect(m_rescanButton, &QPushButton::clicked, this, &MainWindow::rescanPorts);
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::sendFramedMessage);
    connect(m_corruptButton, &QPushButton::clicked, this, &MainWindow::sendCorruptedFrame);

    // The simulated device side echoes back anything it correctly decodes, and
    // reports (rather than silently drops) anything it rejects.
    connect(m_deviceSideLink, &DeviceLink::frameReceived, this, [this](const QByteArray& payload) {
        log(tr("[device] decoded valid frame, echoing back: \"%1\"").arg(QString::fromUtf8(payload)));
        m_deviceSideLink->sendFrame(payload);
    });
    connect(m_deviceSideLink, &DeviceLink::frameRejected, this, [this](const QString& reason) {
        log(tr("[device] %1").arg(reason));
    });

    connect(m_controllerLink, &DeviceLink::frameReceived, this, [this](const QByteArray& payload) {
        log(tr("[controller] received echoed frame: \"%1\"").arg(QString::fromUtf8(payload)));
    });
    connect(m_controllerLink, &DeviceLink::frameRejected, this, [this](const QString& reason) {
        log(tr("[controller] %1").arg(reason));
    });
    connect(m_controllerLink, &DeviceLink::timedOut, this, [this] {
        log(tr("[controller] timed out waiting for a reply"));
    });

    rescanPorts();
    log(tr("Simulated link connected: %1").arg(m_link->isConnected() ? tr("yes") : tr("no")));

    resize(560, 480);
}

void MainWindow::rescanPorts()
{
    m_portList->clear();

    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty()) {
        m_portList->addItem(tr("No physical serial ports detected on this system"));
        return;
    }

    for (const QSerialPortInfo& info : ports) {
        m_portList->addItem(tr("%1 (%2)").arg(info.portName(), info.description()));
    }
}

void MainWindow::sendFramedMessage()
{
    const QByteArray payload = m_messageEdit->text().toUtf8();
    log(tr("[controller] sending framed message: \"%1\"").arg(m_messageEdit->text()));
    m_controllerLink->sendFrame(payload);
}

void MainWindow::sendCorruptedFrame()
{
    const QByteArray payload = m_messageEdit->text().toUtf8();
    QByteArray corrupted = encodeFrame(payload);
    if (corrupted.size() >= 2) {
        // Flip the checksum byte (second-to-last byte, right before ETX) so the
        // frame is well-formed in shape but fails the checksum check.
        const qsizetype checksumIndex = corrupted.size() - 2;
        corrupted[checksumIndex] = static_cast<char>(~corrupted[checksumIndex]);
    }
    log(tr("[controller] sending deliberately corrupted frame (bad checksum)"));
    m_controllerLink->sendRaw(corrupted);
}

void MainWindow::log(const QString& line)
{
    m_log->appendPlainText(QStringLiteral("%1  %2")
                                .arg(QDateTime::currentDateTime().toString(Qt::ISODate), line));
}

} // namespace qttutorial::serial_and_devices
