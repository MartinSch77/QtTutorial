// SPDX-License-Identifier: MIT
#include "DetectionSimulator.h"

#include <QTest>

using namespace qttutorial::defence;

class TestDetectionSimulator : public QObject {
    Q_OBJECT
private slots:
    void producesInitialDetections()
    {
        DetectionSimulator sim;
        QVERIFY(!sim.detections().empty());
        for (const DetectedObject& object : sim.detections()) {
            QVERIFY(!object.label.isEmpty());
        }
    }

    void boundingBoxesStayWithinFrame()
    {
        DetectionSimulator sim;
        for (int i = 0; i < 500; ++i) {
            sim.advance(0.2);
            for (const DetectedObject& object : sim.detections()) {
                QVERIFY(object.x >= -1e-9);
                QVERIFY(object.y >= -1e-9);
                QVERIFY(object.x + object.w <= 1.0 + 1e-9);
                QVERIFY(object.y + object.h <= 1.0 + 1e-9);
            }
        }
    }

    void confidenceStaysInUnitRange()
    {
        DetectionSimulator sim;
        for (int i = 0; i < 500; ++i) {
            sim.advance(0.2);
            for (const DetectedObject& object : sim.detections()) {
                QVERIFY(object.confidence >= 0.0);
                QVERIFY(object.confidence <= 1.0);
            }
        }
    }
};

QTEST_MAIN(TestDetectionSimulator)
#include "test_detection_simulator.moc"
