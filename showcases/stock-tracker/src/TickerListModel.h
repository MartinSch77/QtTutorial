// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

#include <vector>

namespace qttutorial::stock_tracker {

// QAbstractListModel backing the ticker list in the trading cockpit. Purely an
// adapter: StockTrackerBackend owns the actual PriceSimulator instances and
// calls updatePrice() whenever a new tick (real or WebSocket-relayed) arrives.
class TickerListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Created by StockTrackerBackend")
public:
    enum Role { SymbolRole = Qt::UserRole + 1, PriceRole, ChangeRole, ChangePercentRole, UpRole };

    struct Entry {
        QString symbol;
        double price = 0.0;
        double openPrice = 0.0;
    };

    explicit TickerListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addSymbol(const QString& symbol, double openPrice);
    void updatePrice(const QString& symbol, double price);

    [[nodiscard]] int indexOfSymbol(const QString& symbol) const;

private:
    std::vector<Entry> m_entries;
};

} // namespace qttutorial::stock_tracker
