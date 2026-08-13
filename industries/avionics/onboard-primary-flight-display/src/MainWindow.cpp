// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "AnnunciatorPanel.h"
#include "AttitudeIndicator.h"
#include "CompassRose.h"
#include "HeadingIndicator.h"
#include "VerticalSpeedIndicator.h"
#include "VerticalTape.h"

#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QShortcut>

namespace qttutorial::avionics {

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_attitude(new AttitudeIndicator(this))
    , m_airspeedTape(new VerticalTape(10.0, 2.0, this))
    , m_altitudeTape(new VerticalTape(100.0, 0.12, this))
    , m_heading(new HeadingIndicator(this))
    , m_verticalSpeed(new VerticalSpeedIndicator(this))
    , m_compassRose(new CompassRose(this))
    , m_annunciatorPanel(new AnnunciatorPanel(this))
    , m_statusLabel(new QLabel(this))
{
    setWindowTitle(tr("Primary Flight Display"));

    QPalette dark = palette();
    dark.setColor(QPalette::Window, QColor(0x05, 0x05, 0x05));
    dark.setColor(QPalette::WindowText, Qt::white);
    setPalette(dark);
    setAutoFillBackground(true);

    m_statusLabel->setStyleSheet(QStringLiteral("color: #a0a0a0; font-size: 11px;"));
    m_statusLabel->setText(tr("Simulated onboard PFD — no network, no persistence. Esc to exit."));

    auto* layout = new QGridLayout(this);
    layout->addWidget(m_airspeedTape, 0, 0);
    layout->addWidget(m_attitude, 0, 1);
    layout->addWidget(m_altitudeTape, 0, 2);
    layout->addWidget(m_verticalSpeed, 0, 3);
    layout->addWidget(m_compassRose, 0, 4);
    layout->addWidget(m_heading, 1, 0, 1, 4);
    layout->addWidget(m_annunciatorPanel, 1, 4);
    layout->addWidget(m_statusLabel, 2, 0, 1, 5);
    layout->setColumnStretch(1, 3);

    connect(&m_simulator, &FlightDataSimulator::stateChanged, this, &MainWindow::onStateChanged);
    m_simulator.start();

    auto* quitShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    resize(1000, 640);
}

void MainWindow::onStateChanged(const FlightState& state)
{
    m_attitude->setAttitude(state.pitchDeg, state.rollDeg);
    m_airspeedTape->setValue(state.airspeedKt);
    m_altitudeTape->setValue(state.altitudeFt);
    m_heading->setHeading(state.headingDeg);
    m_verticalSpeed->setVerticalSpeed(state.verticalSpeedFtPerMin);
    m_compassRose->setHeading(state.headingDeg);
    m_annunciatorPanel->setMessages(m_annunciatorLogic.evaluate(state));
}

} // namespace qttutorial::avionics
