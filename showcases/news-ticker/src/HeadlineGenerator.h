// SPDX-License-Identifier: MIT
#pragma once

#include "Headline.h"

#include <QString>

#include <vector>

namespace qttutorial::news_ticker {

// Pure, deterministic source of simulated headlines: no QObject, no QTimer, no
// randomness, so rotation order, category filtering and the "breaking" flag are
// all trivially unit-testable independent of the UI. NewsFeedModel (app-side, not
// in this library) is the only thing that wraps this in a QTimer for the QML UI.
class HeadlineGenerator {
public:
    [[nodiscard]] static const std::vector<Headline>& pool();

    [[nodiscard]] static std::vector<QString> categories();

    // pool()[tick % pool().size()]: the deterministic rotation used to simulate a
    // live feed one step at a time.
    [[nodiscard]] static Headline headlineAt(int tick);

    // A deterministic sliding window of `count` headlines starting at `tick`,
    // optionally restricted to one category ("All" returns every category).
    // If fewer than `count` headlines match within one full cycle of the pool,
    // the returned vector is simply shorter.
    [[nodiscard]] static std::vector<Headline> window(int tick, int count, const QString& category);

    [[nodiscard]] static std::vector<Headline> filterByCategory(const std::vector<Headline>& headlines,
                                                                  const QString& category);

    static constexpr const char* kAllCategories = "All";
};

} // namespace qttutorial::news_ticker
