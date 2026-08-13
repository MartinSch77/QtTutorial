// SPDX-License-Identifier: MIT
#include "FleetSimulator.h"

#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::defence;

class TestFleetSimulator : public QObject {
    Q_OBJECT
private slots:
    void seedsAMixedFleet()
    {
        FleetSimulator sim;
        QVERIFY(!sim.assets().empty());

        bool hasVehicle = false;
        bool hasInstallation = false;
        for (const Asset& asset : sim.assets()) {
            hasVehicle |= asset.type == AssetType::Vehicle;
            hasInstallation |= asset.type == AssetType::Installation;
        }
        QVERIFY(hasVehicle);
        QVERIFY(hasInstallation);
    }

    void installationsDoNotMove()
    {
        FleetSimulator sim;
        QString installationId;
        double x = 0.0;
        double y = 0.0;
        for (const Asset& asset : sim.assets()) {
            if (asset.type == AssetType::Installation) {
                installationId = asset.id;
                x = asset.xKm;
                y = asset.yKm;
                break;
            }
        }
        QVERIFY(!installationId.isEmpty());

        for (int i = 0; i < 20; ++i) {
            sim.advance(1.0);
        }

        for (const Asset& asset : sim.assets()) {
            if (asset.id == installationId) {
                QCOMPARE(asset.xKm, x);
                QCOMPARE(asset.yKm, y);
                return;
            }
        }
        QFAIL("installation not found after advancing");
    }

    void eventuallyRaisesAnAlertForTheDriftingAsset()
    {
        FleetSimulator sim;
        QSignalSpy spy(&sim, &FleetSimulator::alertRaised);

        for (int i = 0; i < 6000 && spy.isEmpty(); ++i) {
            sim.advance(1.0);
        }
        QVERIFY(!spy.isEmpty());
    }
};

QTEST_MAIN(TestFleetSimulator)
#include "test_fleet_simulator.moc"
