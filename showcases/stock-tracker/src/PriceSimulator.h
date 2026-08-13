// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QString>

#include <random>

namespace qttutorial::stock_tracker {

// Pure, headless random-walk price generator for one symbol: no QObject, no
// QTimer. Given the same seed, the sequence of prices it produces is fully
// deterministic and reproducible, and every price stays within [minPrice,
// maxPrice] by reflecting off the bounds instead of clamping (clamping would
// make the walk "stick" at a bound; reflecting keeps it wandering realistically).
class PriceSimulator {
public:
    struct Sample {
        double price = 0.0;
        qint64 sequence = 0;
    };

    PriceSimulator(QString symbol, double startPrice, double minPrice, double maxPrice, quint32 seed);

    [[nodiscard]] Sample next();

    [[nodiscard]] const QString& symbol() const { return m_symbol; }
    [[nodiscard]] double currentPrice() const { return m_price; }
    [[nodiscard]] double minPrice() const { return m_min; }
    [[nodiscard]] double maxPrice() const { return m_max; }

private:
    QString m_symbol;
    double m_price;
    double m_min;
    double m_max;
    std::mt19937 m_rng;
    std::normal_distribution<double> m_step;
    qint64 m_sequence = 0;
};

} // namespace qttutorial::stock_tracker
