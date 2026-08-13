// SPDX-License-Identifier: MIT
#pragma once

#include "ActivityLogStore.h"
#include "EnergyMonitor.h"
#include "HomeLinkClient.h"
#include "HomeLinkServer.h"
#include "Protocol.h"

#include <QObject>
#include <QQmlEngine>
#include <QVariantList>

namespace qttutorial::homeautomation::remote {

// QObject/QML façade for the remote-access app: owns a HomeLinkClient
// talking over Qt6::Network to a HomeLinkServer (in this self-contained demo
// a locally-hosted one, standing in for the home system reachable over the
// internet in a real deployment), republishes the latest Snapshot as
// QML-friendly properties, and turns every incoming state change into a
// human-readable entry in an ActivityLogStore. The command-building,
// snapshot-diffing and persistence logic all live in Protocol/HomeSimulator/
// SnapshotDiff/ActivityLogStore, which have no UI dependency and are unit
// tested independent of this class.
class RemoteHomeController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QVariantList rooms READ rooms NOTIFY stateChanged)
    Q_PROPERTY(QVariantList locks READ locks NOTIFY stateChanged)
    Q_PROPERTY(bool armed READ armed NOTIFY stateChanged)
    Q_PROPERTY(QVariantList sensors READ sensors NOTIFY stateChanged)
    Q_PROPERTY(int thermostatMode READ thermostatMode NOTIFY stateChanged)
    Q_PROPERTY(double thermostatCurrent READ thermostatCurrent NOTIFY stateChanged)
    Q_PROPERTY(double thermostatTarget READ thermostatTarget NOTIFY stateChanged)
    Q_PROPERTY(QStringList activityLog READ activityLog NOTIFY activityLogChanged)
    Q_PROPERTY(double currentWatts READ currentWatts NOTIFY stateChanged)
    Q_PROPERTY(double averageWatts READ averageWatts NOTIFY stateChanged)
    Q_PROPERTY(double peakWatts READ peakWatts NOTIFY stateChanged)
    Q_PROPERTY(QVariantList energyHistory READ energyHistory NOTIFY stateChanged)
public:
    explicit RemoteHomeController(QObject* parent = nullptr);

    [[nodiscard]] bool connected() const { return m_connected; }
    [[nodiscard]] QVariantList rooms() const;
    [[nodiscard]] QVariantList locks() const;
    [[nodiscard]] bool armed() const { return m_latest.armed; }
    [[nodiscard]] QVariantList sensors() const;
    [[nodiscard]] int thermostatMode() const { return m_latest.thermostatMode; }
    [[nodiscard]] double thermostatCurrent() const { return m_latest.thermostatCurrent; }
    [[nodiscard]] double thermostatTarget() const { return m_latest.thermostatTarget; }
    [[nodiscard]] QStringList activityLog() const;
    [[nodiscard]] double currentWatts() const { return m_energyHistory.samples().empty() ? 0.0 : m_energyHistory.samples().back().watts; }
    [[nodiscard]] double averageWatts() const { return m_energyHistory.averageWatts(); }
    [[nodiscard]] double peakWatts() const { return m_energyHistory.peakWatts(); }
    // A list of watt values, oldest first, for a Canvas-drawn trend chart -
    // see EnergyPanel.qml.
    [[nodiscard]] QVariantList energyHistory() const;

    Q_INVOKABLE void setLightOn(const QString& room, bool on);
    Q_INVOKABLE void setBrightness(const QString& room, int brightness);
    Q_INVOKABLE void setBlindPosition(const QString& room, int position);
    Q_INVOKABLE void setLockLocked(const QString& lockName, bool locked);
    Q_INVOKABLE void setArmed(bool armed);
    Q_INVOKABLE void setThermostatTarget(double celsius);
    Q_INVOKABLE void setThermostatMode(int mode);

signals:
    void connectedChanged();
    void stateChanged();
    void activityLogChanged();

private:
    void handleSnapshot(const Snapshot& snapshot);

    HomeLinkServer m_server;
    HomeLinkClient m_client;
    ActivityLogStore m_activityStore;
    EnergyHistory m_energyHistory;
    Snapshot m_latest;
    bool m_connected = false;
};

} // namespace qttutorial::homeautomation::remote
