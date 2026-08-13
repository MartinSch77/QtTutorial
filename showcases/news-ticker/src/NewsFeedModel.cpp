// SPDX-License-Identifier: MIT
#include "NewsFeedModel.h"

#include "HeadlineGenerator.h"

namespace qttutorial::news_ticker {

NewsFeedModel::NewsFeedModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_activeCategory(QString::fromLatin1(HeadlineGenerator::kAllCategories))
{
    refresh();
    connect(&m_timer, &QTimer::timeout, this, &NewsFeedModel::advance);
    m_timer.start(kTickIntervalMs);
}

int NewsFeedModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_visible.size());
}

QVariant NewsFeedModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_visible.size())) {
        return {};
    }
    const Headline& headline = m_visible[static_cast<std::size_t>(index.row())];
    switch (role) {
    case TextRole:
        return headline.text;
    case CategoryRole:
        return headline.category;
    case BreakingRole:
        return headline.breaking;
    default:
        return {};
    }
}

QHash<int, QByteArray> NewsFeedModel::roleNames() const
{
    return {
        {TextRole, "text"},
        {CategoryRole, "category"},
        {BreakingRole, "breaking"},
    };
}

QString NewsFeedModel::activeCategory() const
{
    return m_activeCategory;
}

void NewsFeedModel::setActiveCategory(const QString& category)
{
    if (m_activeCategory == category) {
        return;
    }
    m_activeCategory = category;
    emit activeCategoryChanged();
    refresh();
}

QStringList NewsFeedModel::availableCategories() const
{
    QStringList categories = {QString::fromLatin1(HeadlineGenerator::kAllCategories)};
    for (const QString& category : HeadlineGenerator::categories()) {
        categories << category;
    }
    return categories;
}

void NewsFeedModel::advance()
{
    ++m_tick;
    refresh();
}

void NewsFeedModel::refresh()
{
    std::vector<Headline> next = HeadlineGenerator::window(m_tick, kWindowSize, m_activeCategory);

    // A plain reset would tear down and immediately recreate every delegate on
    // every timer tick (this runs every ~2 seconds), which is needlessly
    // expensive and makes any in-flight delegate animation (e.g. the marquee
    // scroll position) restart. When the row count hasn't changed - the common
    // case, since the window is normally a fixed size - update rows in place
    // instead so unaffected views/animations are left alone.
    if (next.size() == m_visible.size()) {
        m_visible = std::move(next);
        if (!m_visible.empty()) {
            emit dataChanged(index(0), index(static_cast<int>(m_visible.size()) - 1));
        }
        return;
    }

    beginResetModel();
    m_visible = std::move(next);
    endResetModel();
}

} // namespace qttutorial::news_ticker
