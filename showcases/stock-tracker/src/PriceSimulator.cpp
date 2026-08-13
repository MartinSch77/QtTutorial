// SPDX-License-Identifier: GPL-3.0-or-later
#include "PriceSimulator.h"

#include <cmath>

namespace qttutorial::stock_tracker {

PriceSimulator::PriceSimulator(QString symbol, double startPrice, double minPrice, double maxPrice, quint32 seed)
    : m_symbol(std::move(symbol))
    , m_price(startPrice)
    , m_min(minPrice)
    , m_max(maxPrice)
    , m_rng(seed)
    , m_step(0.0, (maxPrice - minPrice) * 0.01)
{
}

PriceSimulator::Sample PriceSimulator::next()
{
    m_price += m_step(m_rng);

    // Reflect off the bounds rather than clamp, so the walk keeps moving instead
    // of getting stuck flat against a limit.
    if (m_price > m_max) {
        m_price = m_max - (m_price - m_max);
    } else if (m_price < m_min) {
        m_price = m_min + (m_min - m_price);
    }
    m_price = std::clamp(m_price, m_min, m_max);

    ++m_sequence;
    return {m_price, m_sequence};
}

} // namespace qttutorial::stock_tracker
