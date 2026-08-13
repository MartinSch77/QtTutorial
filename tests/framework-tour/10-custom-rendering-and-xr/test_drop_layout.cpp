// SPDX-License-Identifier: GPL-3.0-or-later
#include "DropLayout.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::quick3d_physics;

namespace {

double distance(const DropPosition& a, const DropPosition& b)
{
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    const double dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

} // namespace

class TestDropLayout : public QObject {
    Q_OBJECT
private slots:
    void returnsRequestedCount()
    {
        const auto positions = DropLayout::gridPositions(6, 40.0, 300.0);
        QCOMPARE(positions.size(), std::size_t{6});
    }

    void placesEveryBodyAtTheRequestedHeight()
    {
        const auto positions = DropLayout::gridPositions(6, 40.0, 300.0);
        for (const auto& position : positions) {
            QCOMPARE(position.y, 300.0);
        }
    }

    void neverOverlapsForARangeOfCountsAndRadii()
    {
        for (int count : {1, 2, 3, 5, 6, 9, 12}) {
            for (double radius : {10.0, 40.0, 75.0}) {
                const auto positions = DropLayout::gridPositions(count, radius, 100.0);
                for (std::size_t i = 0; i < positions.size(); ++i) {
                    for (std::size_t j = i + 1; j < positions.size(); ++j) {
                        QVERIFY(distance(positions[i], positions[j]) >= radius * 2.0);
                    }
                }
            }
        }
    }

    void returnsNothingForNonPositiveInput()
    {
        QVERIFY(DropLayout::gridPositions(0, 40.0, 300.0).empty());
        QVERIFY(DropLayout::gridPositions(-3, 40.0, 300.0).empty());
        QVERIFY(DropLayout::gridPositions(6, 0.0, 300.0).empty());
    }
};

QTEST_MAIN(TestDropLayout)
#include "test_drop_layout.moc"
