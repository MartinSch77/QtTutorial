// SPDX-License-Identifier: MIT
#pragma once

#include <deque>

namespace qttutorial::avionics {

// A small predictive-maintenance heuristic, pure C++23, no Qt dependency:
// tracks a rolling window of a single engine parameter (e.g. EGT margin, oil
// pressure) and flags it when the value drifts outside a nominal band for long
// enough, or when its short-term trend slope suggests it is heading out of band.
// Deliberately simple (this is a showcase, not a certified prognostics
// algorithm) but genuinely computed from the data, not random noise.
class EngineTrend {
public:
    EngineTrend(double nominalLow, double nominalHigh, std::size_t windowSize = 20);

    void addSample(double value);

    [[nodiscard]] bool isOutOfBand() const;
    [[nodiscard]] bool isDrifting() const;
    [[nodiscard]] double latest() const;
    [[nodiscard]] double slopePerSample() const;
    [[nodiscard]] std::size_t sampleCount() const { return m_samples.size(); }

private:
    double m_nominalLow;
    double m_nominalHigh;
    std::size_t m_windowSize;
    std::deque<double> m_samples;
};

} // namespace qttutorial::avionics
