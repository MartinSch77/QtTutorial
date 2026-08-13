// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::medical {

// Produces a simplified PQRST-shaped ECG sample for a given time and heart
// rate, built from a handful of Gaussian bumps rather than random noise, so
// it looks like a real cardiac cycle (P wave, QRS complex, T wave) and stays
// perfectly reproducible for tests.
class EcgWaveformGenerator {
public:
    [[nodiscard]] static double sample(double timeSeconds, double heartRateBpm);
};

} // namespace qttutorial::medical
