// SPDX-License-Identifier: MIT
#include "DetectionSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::defence {

DetectionSimulator::DetectionSimulator()
{
    seed();
}

void DetectionSimulator::seed()
{
    const struct { const char* label; double x; double y; double w; double h; double vx; double vy; } seeds[] = {
        {"vehicle", 0.10, 0.55, 0.18, 0.14, 0.05, 0.015},
        {"person", 0.60, 0.30, 0.06, 0.16, -0.03, 0.02},
        {"structure", 0.70, 0.60, 0.22, 0.18, 0.0, 0.0},
        {"vehicle", 0.35, 0.70, 0.16, 0.12, -0.04, -0.01},
    };

    m_detections.clear();
    m_vx.clear();
    m_vy.clear();
    int id = 1;
    for (const auto& s : seeds) {
        DetectedObject object;
        object.id = id++;
        object.label = QString::fromLatin1(s.label);
        object.confidence = 0.8;
        object.x = s.x;
        object.y = s.y;
        object.w = s.w;
        object.h = s.h;
        m_detections.push_back(object);
        m_vx.push_back(s.vx);
        m_vy.push_back(s.vy);
    }
}

void DetectionSimulator::advance(double dtSeconds)
{
    m_elapsedSeconds += dtSeconds;

    for (std::size_t i = 0; i < m_detections.size(); ++i) {
        DetectedObject& object = m_detections[i];

        double newX = object.x + m_vx[i] * dtSeconds;
        double newY = object.y + m_vy[i] * dtSeconds;

        if (newX < 0.0 || newX + object.w > 1.0) {
            m_vx[i] = -m_vx[i];
            newX = std::clamp(newX, 0.0, 1.0 - object.w);
        }
        if (newY < 0.0 || newY + object.h > 1.0) {
            m_vy[i] = -m_vy[i];
            newY = std::clamp(newY, 0.0, 1.0 - object.h);
        }

        object.x = newX;
        object.y = newY;
        object.confidence = std::clamp(0.75 + 0.2 * std::sin(m_elapsedSeconds * 0.6 + i * 1.3), 0.0, 1.0);
    }
}

} // namespace qttutorial::defence
