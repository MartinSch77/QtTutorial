// SPDX-License-Identifier: MIT
#pragma once

class QPainter;
class QRectF;
class QColor;

namespace qttutorial::space::icons {

// A small set of procedurally-drawn vector icons (QPainterPath, no external
// icon fonts/images/SVG assets) giving the dashboard the visual grammar of a
// real mission-control display: a satellite body, an orbit arc, a
// ground-station dish and a warning triangle.

void drawSatellite(QPainter& painter, const QRectF& rect, const QColor& color);
void drawOrbitArc(QPainter& painter, const QRectF& rect, const QColor& color);
void drawGroundStation(QPainter& painter, const QRectF& rect, const QColor& color, bool active);
void drawWarningTriangle(QPainter& painter, const QRectF& rect, const QColor& color);

} // namespace qttutorial::space::icons
