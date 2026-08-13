// SPDX-License-Identifier: MIT
#include "HeadlineGenerator.h"

namespace qttutorial::news_ticker {

const std::vector<Headline>& HeadlineGenerator::pool()
{
    static const std::vector<Headline> kPool = {
        {QStringLiteral("Coalition talks continue as summit enters second day"), QStringLiteral("World"), false},
        {QStringLiteral("BREAKING: Major cross-border trade agreement signed"), QStringLiteral("World"), true},
        {QStringLiteral("Regional elections draw record voter turnout"), QStringLiteral("World"), false},
        {QStringLiteral("New quantum chip claims 2x efficiency over predecessor"), QStringLiteral("Tech"), false},
        {QStringLiteral("BREAKING: Widely used cloud provider reports major outage"), QStringLiteral("Tech"), true},
        {QStringLiteral("Open-source framework releases long-awaited major version"), QStringLiteral("Tech"), false},
        {QStringLiteral("Quarterly earnings beat analyst expectations across sector"), QStringLiteral("Business"), false},
        {QStringLiteral("Central bank holds interest rates steady for third meeting"), QStringLiteral("Business"), false},
        {QStringLiteral("BREAKING: Merger of two logistics giants announced"), QStringLiteral("Business"), true},
        {QStringLiteral("Underdog team advances to championship final"), QStringLiteral("Sports"), false},
        {QStringLiteral("Star player signs record-breaking contract extension"), QStringLiteral("Sports"), false},
        {QStringLiteral("Host city announces venue upgrades ahead of tournament"), QStringLiteral("Sports"), false},
    };
    return kPool;
}

std::vector<QString> HeadlineGenerator::categories()
{
    return {QStringLiteral("World"), QStringLiteral("Tech"), QStringLiteral("Business"), QStringLiteral("Sports")};
}

Headline HeadlineGenerator::headlineAt(int tick)
{
    const std::vector<Headline>& all = pool();
    const int index = ((tick % static_cast<int>(all.size())) + static_cast<int>(all.size())) % static_cast<int>(all.size());
    return all[static_cast<std::size_t>(index)];
}

std::vector<Headline> HeadlineGenerator::window(int tick, int count, const QString& category)
{
    std::vector<Headline> result;
    if (count <= 0) {
        return result;
    }
    const std::vector<Headline>& all = pool();
    const int poolSize = static_cast<int>(all.size());
    for (int offset = 0; offset < poolSize && static_cast<int>(result.size()) < count; ++offset) {
        Headline candidate = headlineAt(tick + offset);
        if (category == QString::fromLatin1(kAllCategories) || candidate.category == category) {
            result.push_back(std::move(candidate));
        }
    }
    return result;
}

std::vector<Headline> HeadlineGenerator::filterByCategory(const std::vector<Headline>& headlines,
                                                           const QString& category)
{
    if (category == QString::fromLatin1(kAllCategories)) {
        return headlines;
    }
    std::vector<Headline> result;
    for (const Headline& headline : headlines) {
        if (headline.category == category) {
            result.push_back(headline);
        }
    }
    return result;
}

} // namespace qttutorial::news_ticker
