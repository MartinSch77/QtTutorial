// SPDX-License-Identifier: MIT
#pragma once

#include "DataLinkTrackModel.h"
#include "DetectionSimulator.h"
#include "HudSimulator.h"
#include "SubsystemHealthMachine.h"
#include "TrackMotionModel.h"
#include "VehicleStatusSimulator.h"

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QVariantList>

#include <memory>
#include <vector>

namespace qttutorial::defence {

// Thin QObject/QML facade that owns the timer driving all four onboard
// panels' pure simulation logic (TrackSimulator, VehicleStatusSimulator,
// SubsystemHealthMachine, DetectionSimulator, HudSimulator) and republishes
// their state as QML-bindable properties. All the actual motion/health logic
// lives in those separate classes so it stays unit-testable without QML or
// an event loop.
class PlatformSimulator : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(double headingDeg READ headingDeg NOTIFY dataChanged)
    Q_PROPERTY(double speedUnits READ speedUnits NOTIFY dataChanged)
    Q_PROPERTY(double fuelPercent READ fuelPercent NOTIFY dataChanged)
    Q_PROPERTY(QVariantList subsystems READ subsystems NOTIFY dataChanged)
    Q_PROPERTY(QVariantList tracks READ tracks NOTIFY dataChanged)
    Q_PROPERTY(double trackAreaRadiusKm READ trackAreaRadiusKm CONSTANT)
    Q_PROPERTY(QVariantList detections READ detections NOTIFY dataChanged)
    Q_PROPERTY(double hudHeadingDeg READ hudHeadingDeg NOTIFY dataChanged)
    Q_PROPERTY(QVariantList waypoints READ waypoints NOTIFY dataChanged)
    Q_PROPERTY(QVariantList teammates READ teammates NOTIFY dataChanged)
    Q_PROPERTY(QVariantList dataLinkTracks READ dataLinkTracks NOTIFY dataChanged)
    Q_PROPERTY(double commsQualityPercent READ commsQualityPercent NOTIFY dataChanged)
public:
    explicit PlatformSimulator(QObject* parent = nullptr);

    [[nodiscard]] double headingDeg() const { return m_vehicleStatus.status().headingDeg; }
    [[nodiscard]] double speedUnits() const { return m_vehicleStatus.status().speedUnits; }
    [[nodiscard]] double fuelPercent() const { return m_vehicleStatus.status().fuelPercent; }
    [[nodiscard]] QVariantList subsystems() const { return m_subsystems; }
    [[nodiscard]] QVariantList tracks() const;
    [[nodiscard]] double trackAreaRadiusKm() const { return m_trackSimulator.areaRadiusKm(); }
    [[nodiscard]] QVariantList detections() const;
    [[nodiscard]] double hudHeadingDeg() const { return m_hudSimulator.headingDeg(); }
    [[nodiscard]] QVariantList waypoints() const;
    [[nodiscard]] QVariantList teammates() const;
    [[nodiscard]] QVariantList dataLinkTracks() const;
    [[nodiscard]] double commsQualityPercent() const { return m_commsQualityPercent; }

signals:
    void dataChanged();

private:
    void tick();
    void onSubsystemHealthChanged(const QString& subsystemName, const QString& stateName);

    TrackSimulator m_trackSimulator;
    VehicleStatusSimulator m_vehicleStatus;
    DetectionSimulator m_detectionSimulator;
    HudSimulator m_hudSimulator;
    DataLinkTrackModel m_dataLinkModel;
    std::vector<std::unique_ptr<SubsystemHealthMachine>> m_subsystemMachines;
    QVariantList m_subsystems;
    QTimer m_timer;
    double m_elapsedSeconds = 0.0;
    double m_commsQualityPercent = 100.0;
};

} // namespace qttutorial::defence
