// SPDX-License-Identifier: MIT
#pragma once

#include "Headline.h"

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QStringList>
#include <QTimer>

#include <vector>

namespace qttutorial::news_ticker {

// QAbstractListModel that drives the scrolling ticker band. All rotation/category
// filtering math itself lives in the pure HeadlineGenerator (news_ticker_lib); this
// class only owns the QTimer that periodically asks the generator for the next
// window of headlines and adapts the result to Qt Model/View roles.
class NewsFeedModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString activeCategory READ activeCategory WRITE setActiveCategory NOTIFY activeCategoryChanged)
    Q_PROPERTY(QStringList availableCategories READ availableCategories CONSTANT)
public:
    enum Role { TextRole = Qt::UserRole + 1, CategoryRole, BreakingRole };

    explicit NewsFeedModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] QString activeCategory() const;
    void setActiveCategory(const QString& category);

    [[nodiscard]] QStringList availableCategories() const;

signals:
    void activeCategoryChanged();

private slots:
    void advance();

private:
    void refresh();

    QTimer m_timer;
    QString m_activeCategory;
    int m_tick = 0;
    std::vector<Headline> m_visible;
    static constexpr int kWindowSize = 6;
    static constexpr int kTickIntervalMs = 2200;
};

} // namespace qttutorial::news_ticker
