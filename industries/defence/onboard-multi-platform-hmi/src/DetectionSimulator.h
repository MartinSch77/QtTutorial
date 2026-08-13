// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::defence {

// A single simulated object-detector output: a bounding box (normalized
// 0..1 frame coordinates), a generic class label and a confidence score.
// This models only what a detection pipeline's *output overlay* would show -
// there is no image processing and no targeting/aiming data here at all.
struct DetectedObject {
    int id = 0;
    QString label;
    double confidence = 0.0;
    double x = 0.0;
    double y = 0.0;
    double w = 0.0;
    double h = 0.0;
};

// Moves a handful of simulated bounding boxes smoothly around a normalized
// video frame, bouncing off the edges, with a slowly oscillating confidence
// score per box. Pure C++23, deterministic given elapsed time.
class DetectionSimulator {
public:
    DetectionSimulator();

    void advance(double dtSeconds);

    [[nodiscard]] const std::vector<DetectedObject>& detections() const { return m_detections; }

private:
    void seed();

    std::vector<DetectedObject> m_detections;
    std::vector<double> m_vx;
    std::vector<double> m_vy;
    double m_elapsedSeconds = 0.0;
};

} // namespace qttutorial::defence
