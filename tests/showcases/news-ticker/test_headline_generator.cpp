// SPDX-License-Identifier: MIT
#include "HeadlineGenerator.h"

#include <QTest>

#include <algorithm>

using qttutorial::news_ticker::Headline;
using qttutorial::news_ticker::HeadlineGenerator;

class TestHeadlineGenerator : public QObject {
    Q_OBJECT
private slots:
    void poolIsNonEmptyAndCoversAllCategories()
    {
        const std::vector<Headline>& pool = HeadlineGenerator::pool();
        QVERIFY(!pool.empty());

        for (const QString& category : HeadlineGenerator::categories()) {
            const bool found = std::any_of(pool.begin(), pool.end(), [&](const Headline& h) {
                return h.category == category;
            });
            QVERIFY2(found, qPrintable(QStringLiteral("no pool headline found for category ") + category));
        }
    }

    void headlineAtRotatesDeterministicallyThroughThePool()
    {
        const std::vector<Headline>& pool = HeadlineGenerator::pool();
        const int size = static_cast<int>(pool.size());

        for (int tick = 0; tick < size * 2; ++tick) {
            QCOMPARE(HeadlineGenerator::headlineAt(tick).text, pool[static_cast<std::size_t>(tick % size)].text);
        }

        // Same tick always yields the same headline: this is the "deterministic
        // rotation", not randomness dressed up as rotation.
        QCOMPARE(HeadlineGenerator::headlineAt(3).text, HeadlineGenerator::headlineAt(3).text);
    }

    void headlineAtWrapsAroundThePool()
    {
        const std::vector<Headline>& pool = HeadlineGenerator::pool();
        const int size = static_cast<int>(pool.size());
        QCOMPARE(HeadlineGenerator::headlineAt(size).text, HeadlineGenerator::headlineAt(0).text);
        QCOMPARE(HeadlineGenerator::headlineAt(size + 2).text, HeadlineGenerator::headlineAt(2).text);
    }

    void filterByCategoryAllReturnsEverything()
    {
        const std::vector<Headline>& pool = HeadlineGenerator::pool();
        const std::vector<Headline> filtered = HeadlineGenerator::filterByCategory(pool, QStringLiteral("All"));
        QCOMPARE(filtered.size(), pool.size());
    }

    void filterByCategoryRestrictsToOneCategory()
    {
        const std::vector<Headline>& pool = HeadlineGenerator::pool();
        const std::vector<Headline> filtered = HeadlineGenerator::filterByCategory(pool, QStringLiteral("Tech"));
        QVERIFY(!filtered.empty());
        for (const Headline& headline : filtered) {
            QCOMPARE(headline.category, QStringLiteral("Tech"));
        }
    }

    void windowRespectsRequestedCountAndCategory()
    {
        const std::vector<Headline> window = HeadlineGenerator::window(0, 3, QStringLiteral("Sports"));
        QCOMPARE(window.size(), std::size_t(3));
        for (const Headline& headline : window) {
            QCOMPARE(headline.category, QStringLiteral("Sports"));
        }
    }

    void windowIsShorterThanRequestedWhenCategoryHasFewMatches()
    {
        // Every category in the pool has at least one breaking headline but far
        // fewer than a large window size, so asking for more than exist in one
        // full cycle of the pool must not fabricate extra entries.
        const std::vector<Headline> window = HeadlineGenerator::window(0, 100, QStringLiteral("World"));
        QVERIFY(window.size() < 100);
        for (const Headline& headline : window) {
            QCOMPARE(headline.category, QStringLiteral("World"));
        }
    }

    void breakingFlagIsPreservedFromThePool()
    {
        const std::vector<Headline>& pool = HeadlineGenerator::pool();
        const bool anyBreaking = std::any_of(pool.begin(), pool.end(), [](const Headline& h) { return h.breaking; });
        QVERIFY(anyBreaking);

        for (std::size_t i = 0; i < pool.size(); ++i) {
            QCOMPARE(HeadlineGenerator::headlineAt(static_cast<int>(i)).breaking, pool[i].breaking);
        }
    }
};

QTEST_MAIN(TestHeadlineGenerator)
#include "test_headline_generator.moc"
