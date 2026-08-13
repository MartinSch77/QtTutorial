// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "OrderLifecycle.h"
#include "PriceSimulator.h"
#include "TickerListModel.h"

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QVariantList>

#include <map>
#include <memory>
#include <vector>

#ifdef STOCK_TRACKER_HAVE_WEBSOCKETS
#include "WebSocketMarketFeed.h"
#endif

namespace qttutorial::stock_tracker {

// The single QObject exposed to QML as "backend": owns every symbol's
// PriceSimulator, the TickerListModel adapting them for the ticker list, the
// one OrderLifecycle used by the order panel, and (when Qt6::WebSockets is
// available) the WebSocketMarketFeed that relays simulated ticks over a real
// loopback WebSocket. QML only binds to and displays what this class computes
// - none of the price/order logic is duplicated in QML/JS.
class StockTrackerBackend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(qttutorial::stock_tracker::TickerListModel* tickers READ tickers CONSTANT)
    Q_PROPERTY(qttutorial::stock_tracker::OrderLifecycle* currentOrder READ currentOrder CONSTANT)
    Q_PROPERTY(QString selectedSymbol READ selectedSymbol WRITE setSelectedSymbol NOTIFY selectedSymbolChanged)
    Q_PROPERTY(double selectedPrice READ selectedPrice NOTIFY selectedPriceHistoryChanged)
    Q_PROPERTY(QVariantList selectedPriceHistory READ selectedPriceHistory NOTIFY selectedPriceHistoryChanged)
    Q_PROPERTY(QString feedMode READ feedMode CONSTANT)
    Q_PROPERTY(bool feedConnected READ feedConnected NOTIFY feedConnectedChanged)
    Q_PROPERTY(bool graphsAvailable READ graphsAvailable CONSTANT)
public:
    explicit StockTrackerBackend(QObject* parent = nullptr);

    [[nodiscard]] TickerListModel* tickers() const;
    [[nodiscard]] OrderLifecycle* currentOrder() const;
    [[nodiscard]] QString selectedSymbol() const;
    [[nodiscard]] double selectedPrice() const;
    [[nodiscard]] QVariantList selectedPriceHistory() const;
    [[nodiscard]] QString feedMode() const;
    [[nodiscard]] bool feedConnected() const;
    [[nodiscard]] bool graphsAvailable() const;

    void setSelectedSymbol(const QString& symbol);

    Q_INVOKABLE void submitCurrentOrder();

signals:
    void selectedSymbolChanged();
    void selectedPriceHistoryChanged();
    void feedConnectedChanged();

private slots:
    void onOrderStateChanged(const QString& stateName);

private:
    void addSymbol(const QString& symbol, double startPrice, double minPrice, double maxPrice, quint32 seed);
    void advanceTick();
    void applyTick(const QString& symbol, double price, qint64 sequence);
    void scheduleSimulatedExecution();

    TickerListModel* m_tickers;
    OrderLifecycle* m_currentOrder;
    QTimer m_marketTimer;
    QString m_selectedSymbol;
    bool m_feedConnected = false;

    std::vector<std::unique_ptr<PriceSimulator>> m_simulators;
    std::map<QString, std::vector<double>> m_history;
    static constexpr int kHistoryCap = 90;
    static constexpr int kTickIntervalMs = 700;

#ifdef STOCK_TRACKER_HAVE_WEBSOCKETS
    std::unique_ptr<WebSocketMarketFeed> m_feed;
#endif
};

} // namespace qttutorial::stock_tracker
