// SPDX-License-Identifier: MIT
#include "DataLinkTrackModel.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::defence;

class TestDataLinkTrackModel : public QObject {
    Q_OBJECT
private slots:
    void seedsTracksWithDomainAndClassification()
    {
        DataLinkTrackModel model(40.0);
        QVERIFY(!model.tracks().empty());
        for (const DataLinkTrack& track : model.tracks()) {
            QVERIFY(!track.classification.isEmpty());
            QVERIFY(track.domain == QStringLiteral("Air") || track.domain == QStringLiteral("Surface")
                     || track.domain == QStringLiteral("Ground"));
            QVERIFY(!track.stale);
            QCOMPARE(track.dataAgeSeconds, 0.0);
        }
    }

    void updateIntervalIsOneAtFullQualityAndGrowsAsQualityDrops()
    {
        QCOMPARE(DataLinkTrackModel::updateIntervalTicks(100.0), 1);
        QVERIFY(DataLinkTrackModel::updateIntervalTicks(0.0) > DataLinkTrackModel::updateIntervalTicks(100.0));
        // Monotonic non-increasing as quality improves, sampled at a few points.
        int previousInterval = DataLinkTrackModel::updateIntervalTicks(0.0);
        for (double quality : {20.0, 40.0, 60.0, 80.0, 100.0}) {
            const int interval = DataLinkTrackModel::updateIntervalTicks(quality);
            QVERIFY(interval <= previousInterval);
            previousInterval = interval;
        }
    }

    void fullQualityLinkNeverGoesStale()
    {
        DataLinkTrackModel model(40.0);
        for (int i = 0; i < 50; ++i) {
            model.advance(1.0, 100.0);
            for (const DataLinkTrack& track : model.tracks()) {
                QVERIFY(!track.stale);
                QCOMPARE(track.dataAgeSeconds, 0.0);
            }
        }
    }

    void degradedLinkAccumulatesStaleAge()
    {
        DataLinkTrackModel model(40.0);
        // updateIntervalTicks(0.0) == 7, so the first 6 advances at zero
        // quality should leave every track stale and aging.
        for (int i = 0; i < 6; ++i) {
            model.advance(1.0, 0.0);
        }
        for (const DataLinkTrack& track : model.tracks()) {
            QVERIFY(track.stale);
            QVERIFY(track.dataAgeSeconds > 0.0);
        }

        // The 7th tick delivers a fresh update and clears staleness.
        model.advance(1.0, 0.0);
        for (const DataLinkTrack& track : model.tracks()) {
            QVERIFY(!track.stale);
            QCOMPARE(track.dataAgeSeconds, 0.0);
        }
    }

    void staysWithinSurveillanceAreaEvenWhileStale()
    {
        DataLinkTrackModel model(40.0);
        for (int i = 0; i < 500; ++i) {
            model.advance(1.0, 10.0);
        }
        for (const DataLinkTrack& track : model.tracks()) {
            const double radius = std::hypot(track.xKm, track.yKm);
            QVERIFY(radius <= model.areaRadiusKm() + 1e-6);
        }
    }
};

QTEST_MAIN(TestDataLinkTrackModel)
#include "test_data_link_track_model.moc"
