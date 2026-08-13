// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::medical {

// Produces a simplified respiration (impedance pneumography style) waveform
// sample for a given time and respiration rate: a fast inspiratory rise
// followed by a slower expiratory fall, built from two smooth cosine ramps
// rather than random noise, so it stays perfectly reproducible for tests and
// visually reads as a breathing trace rather than a generic wiggle.
class RespirationWaveformGenerator {
public:
    [[nodiscard]] static double sample(double timeSeconds, double respirationRateBpm);
};

} // namespace qttutorial::medical
