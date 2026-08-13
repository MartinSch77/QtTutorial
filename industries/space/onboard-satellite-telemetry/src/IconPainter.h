// SPDX-License-Identifier: MIT
#pragma once

class QPainter;
class QRectF;
class QColor;

namespace qttutorial::space::icons {

// A small set of procedurally-drawn vector icons (QPainterPath, no external
// icon fonts/images/SVG assets) giving the kiosk display the visual grammar
// of a real telemetry/ops panel: a satellite body, an orbit arc, a
// ground-station dish, a battery/solar charge glyph and a warning triangle.
// Each function draws into `rect` of the given QPainter using `color`.

void drawSatellite(QPainter& painter, const QRectF& rect, const QColor& color);
void drawOrbitArc(QPainter& painter, const QRectF& rect, const QColor& color);
void drawGroundStation(QPainter& painter, const QRectF& rect, const QColor& color, bool active);
void drawBatteryCharge(QPainter& painter, const QRectF& rect, const QColor& color, bool charging);
void drawWarningTriangle(QPainter& painter, const QRectF& rect, const QColor& color);

} // namespace qttutorial::space::icons
