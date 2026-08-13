// SPDX-License-Identifier: MIT
#include "EngineTrend.h"

#include <numeric>

namespace qttutorial::avionics {

EngineTrend::EngineTrend(double nominalLow, double nominalHigh, std::size_t windowSize)
    : m_nominalLow(nominalLow)
    , m_nominalHigh(nominalHigh)
    , m_windowSize(windowSize)
{
}

void EngineTrend::addSample(double value)
{
    m_samples.push_back(value);
    while (m_samples.size() > m_windowSize) {
        m_samples.pop_front();
    }
}

bool EngineTrend::isOutOfBand() const
{
    if (m_samples.empty()) {
        return false;
    }
    return latest() < m_nominalLow || latest() > m_nominalHigh;
}

double EngineTrend::latest() const
{
    return m_samples.empty() ? 0.0 : m_samples.back();
}

double EngineTrend::slopePerSample() const
{
    const auto n = m_samples.size();
    if (n < 2) {
        return 0.0;
    }

    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumXX = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        const double x = static_cast<double>(i);
        const double y = m_samples[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumXX += x * x;
    }
    const double denominator = static_cast<double>(n) * sumXX - sumX * sumX;
    if (denominator == 0.0) {
        return 0.0;
    }
    return (static_cast<double>(n) * sumXY - sumX * sumY) / denominator;
}

bool EngineTrend::isDrifting() const
{
    if (m_samples.size() < m_windowSize) {
        return false;
    }
    const double bandWidth = m_nominalHigh - m_nominalLow;
    if (bandWidth <= 0.0) {
        return false;
    }
    const double slope = slopePerSample();
    const double projectedChange = slope * static_cast<double>(m_windowSize);
    const bool trendingUpOutOfBand = slope > 0.0 && latest() + projectedChange > m_nominalHigh;
    const bool trendingDownOutOfBand = slope < 0.0 && latest() + projectedChange < m_nominalLow;
    return isOutOfBand() || trendingUpOutOfBand || trendingDownOutOfBand;
}

} // namespace qttutorial::avionics
