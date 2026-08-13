// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "RadialGauge.h"
#include "SubsystemHealthGrid.h"

#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QShortcut>

namespace qttutorial::space {

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_orbitLabel(new QLabel(this))
    , m_eclipseLabel(new QLabel(this))
    , m_batteryGauge(new RadialGauge(tr("Battery SoC"), QStringLiteral("%"), 0.0, 100.0, this))
    , m_radiatorGauge(new RadialGauge(tr("Radiator"), QStringLiteral("°C"), -50.0, 20.0, this))
    , m_batteryBayGauge(new RadialGauge(tr("Battery Bay"), QStringLiteral("°C"), -10.0, 40.0, this))
    , m_payloadGauge(new RadialGauge(tr("Payload"), QStringLiteral("°C"), -30.0, 45.0, this))
    , m_avionicsGauge(new RadialGauge(tr("Avionics"), QStringLiteral("°C"), -20.0, 40.0, this))
    , m_healthGrid(new SubsystemHealthGrid(this))
{
    setWindowTitle(tr("Satellite Onboard Telemetry"));

    QPalette dark = palette();
    dark.setColor(QPalette::Window, QColor(0x05, 0x05, 0x08));
    dark.setColor(QPalette::WindowText, Qt::white);
    setPalette(dark);
    setAutoFillBackground(true);

    for (auto* label : {m_orbitLabel, m_eclipseLabel}) {
        label->setStyleSheet(QStringLiteral("color: white; font-size: 13px;"));
    }

    for (const auto& subsystem : m_simulator.subsystemMachines()) {
        m_healthGrid->addSubsystem(subsystem->subsystemName());
        connect(subsystem.get(), &SubsystemHealthMachine::healthChanged, m_healthGrid,
                &SubsystemHealthGrid::onHealthChanged);
    }

    auto* layout = new QGridLayout(this);
    layout->addWidget(m_orbitLabel, 0, 0, 1, 5);
    layout->addWidget(m_eclipseLabel, 1, 0, 1, 5);
    layout->addWidget(m_batteryGauge, 2, 0);
    layout->addWidget(m_radiatorGauge, 2, 1);
    layout->addWidget(m_batteryBayGauge, 2, 2);
    layout->addWidget(m_payloadGauge, 2, 3);
    layout->addWidget(m_avionicsGauge, 2, 4);
    layout->addWidget(m_healthGrid, 3, 0, 1, 5);

    connect(&m_simulator, &TelemetrySimulator::telemetryUpdated, this, &MainWindow::onTelemetryUpdated);
    m_simulator.start();

    auto* quitShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(quitShortcut, &QShortcut::activated, this, &QWidget::close);

    resize(1100, 640);
}

void MainWindow::onTelemetryUpdated()
{
    const OrbitalState& orbit = m_simulator.orbitalState();
    m_orbitLabel->setText(tr("True anomaly: %1 deg  |  Altitude: %2 km  |  Period: %3 min")
                               .arg(orbit.trueAnomalyDeg, 0, 'f', 1)
                               .arg(orbit.altitudeKm, 0, 'f', 0)
                               .arg(orbit.orbitalPeriodMinutes, 0, 'f', 1));
    m_eclipseLabel->setText(orbit.inEclipse ? tr("Eclipse - solar input 0%")
                                             : tr("Sunlit - solar input %1%").arg(orbit.solarInputFraction * 100.0, 0, 'f', 0));

    m_batteryGauge->setValue(m_simulator.powerState().batterySocPercent);
    m_radiatorGauge->setValue(m_simulator.thermalZoneTemperatureC(ThermalZone::Radiator));
    m_batteryBayGauge->setValue(m_simulator.thermalZoneTemperatureC(ThermalZone::BatteryBay));
    m_payloadGauge->setValue(m_simulator.thermalZoneTemperatureC(ThermalZone::Payload));
    m_avionicsGauge->setValue(m_simulator.thermalZoneTemperatureC(ThermalZone::Avionics));
}

} // namespace qttutorial::space
