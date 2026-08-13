// SPDX-License-Identifier: GPL-3.0-or-later
#include "StockTrackerBackend.h"

#include <QRandomGenerator>

namespace qttutorial::stock_tracker {

StockTrackerBackend::StockTrackerBackend(QObject* parent)
    : QObject(parent)
    , m_tickers(new TickerListModel(this))
    , m_currentOrder(new OrderLifecycle(this))
{
    addSymbol(QStringLiteral("QTX"), 182.40, 120.0, 260.0, 1);
    addSymbol(QStringLiteral("BLUE"), 64.10, 30.0, 110.0, 2);
    addSymbol(QStringLiteral("NOVA"), 305.75, 200.0, 420.0, 3);
    addSymbol(QStringLiteral("HRZN"), 18.90, 5.0, 45.0, 4);

    m_selectedSymbol = m_simulators.front()->symbol();

    connect(m_currentOrder, &OrderLifecycle::stateChanged, this, &StockTrackerBackend::onOrderStateChanged);

#ifdef STOCK_TRACKER_HAVE_WEBSOCKETS
    m_feed = std::make_unique<WebSocketMarketFeed>(this);
    connect(m_feed.get(), &WebSocketMarketFeed::connected, this, [this] {
        m_feedConnected = true;
        emit feedConnectedChanged();
    });
    connect(m_feed.get(), &WebSocketMarketFeed::disconnected, this, [this] {
        m_feedConnected = false;
        emit feedConnectedChanged();
    });
    connect(m_feed.get(), &WebSocketMarketFeed::tickReceived, this,
            [this](const QString& symbol, double price, qint64 sequence) { applyTick(symbol, price, sequence); });
    m_feed->start();
#endif

    connect(&m_marketTimer, &QTimer::timeout, this, &StockTrackerBackend::advanceTick);
    m_marketTimer.start(kTickIntervalMs);
}

TickerListModel* StockTrackerBackend::tickers() const
{
    return m_tickers;
}

OrderLifecycle* StockTrackerBackend::currentOrder() const
{
    return m_currentOrder;
}

QString StockTrackerBackend::selectedSymbol() const
{
    return m_selectedSymbol;
}

double StockTrackerBackend::selectedPrice() const
{
    const auto it = m_history.find(m_selectedSymbol);
    if (it == m_history.end() || it->second.empty()) {
        return 0.0;
    }
    return it->second.back();
}

QVariantList StockTrackerBackend::selectedPriceHistory() const
{
    QVariantList result;
    const auto it = m_history.find(m_selectedSymbol);
    if (it == m_history.end()) {
        return result;
    }
    result.reserve(static_cast<int>(it->second.size()));
    for (double price : it->second) {
        result.append(price);
    }
    return result;
}

QString StockTrackerBackend::feedMode() const
{
#ifdef STOCK_TRACKER_HAVE_WEBSOCKETS
    return QStringLiteral(
        "Live: QWebSocketServer (loopback) -> QWebSocket client, both in this process (Qt WebSockets present)");
#else
    return QStringLiteral(
        "Direct in-process feed: Qt WebSockets not found on this Qt (needs >= 6.5) - "
        "PriceSimulator ticks are applied directly instead of round-tripping over a socket");
#endif
}

bool StockTrackerBackend::feedConnected() const
{
    return m_feedConnected;
}

bool StockTrackerBackend::graphsAvailable() const
{
#ifdef STOCK_TRACKER_HAVE_GRAPHS
    return true;
#else
    return false;
#endif
}

void StockTrackerBackend::setSelectedSymbol(const QString& symbol)
{
    if (m_selectedSymbol == symbol || m_tickers->indexOfSymbol(symbol) < 0) {
        return;
    }
    m_selectedSymbol = symbol;
    emit selectedSymbolChanged();
    emit selectedPriceHistoryChanged();
}

void StockTrackerBackend::submitCurrentOrder()
{
    m_currentOrder->submit();
}

void StockTrackerBackend::onOrderStateChanged(const QString& stateName)
{
    if (stateName == QStringLiteral("Submitted")) {
        scheduleSimulatedExecution();
    }
}

void StockTrackerBackend::addSymbol(const QString& symbol, double startPrice, double minPrice, double maxPrice,
                                     quint32 seed)
{
    m_simulators.push_back(std::make_unique<PriceSimulator>(symbol, startPrice, minPrice, maxPrice, seed));
    m_tickers->addSymbol(symbol, startPrice);
    m_history[symbol] = {startPrice};
}

void StockTrackerBackend::advanceTick()
{
    for (auto& simulator : m_simulators) {
        const PriceSimulator::Sample sample = simulator->next();
#ifdef STOCK_TRACKER_HAVE_WEBSOCKETS
        m_feed->publishTick(simulator->symbol(), sample.price, sample.sequence);
#else
        applyTick(simulator->symbol(), sample.price, sample.sequence);
#endif
    }
}

void StockTrackerBackend::applyTick(const QString& symbol, double price, qint64 /*sequence*/)
{
    m_tickers->updatePrice(symbol, price);

    std::vector<double>& history = m_history[symbol];
    history.push_back(price);
    if (static_cast<int>(history.size()) > kHistoryCap) {
        history.erase(history.begin());
    }

    if (symbol == m_selectedSymbol) {
        emit selectedPriceHistoryChanged();
    }
}

void StockTrackerBackend::scheduleSimulatedExecution()
{
    // Orders over 500 shares are simulated as failing a risk check instead of
    // filling, purely to give the guarded-transition demo some variety beyond
    // "everything always fills".
    const bool willReject = m_currentOrder->quantity() > 500;
    const int delayMs = 1200 + static_cast<int>(QRandomGenerator::global()->bounded(1200));

    QTimer::singleShot(delayMs, m_currentOrder, [this, willReject] {
        if (willReject) {
            m_currentOrder->reject();
        } else {
            m_currentOrder->fill();
        }
    });
}

} // namespace qttutorial::stock_tracker
