// SPDX-License-Identifier: MIT
#pragma once

class QPainter;
class QRectF;
class QColor;

namespace qttutorial::two_wheelers::fleet {

// Small, self-contained procedurally-drawn vector icons for the fleet table,
// painted with QPainterPath (no image/SVG assets, consistent with TrendWidget's
// existing QPainter-only convention). Purely geometric glyphs - no manufacturer
// branding of any kind.
namespace icons {

// Side-view motorcycle/e-scooter silhouette, used to identify vehicle rows.
void paintMotorcycleIcon(QPainter& painter, const QRectF& bounds, const QColor& strokeColor);

// A cog/gear badge outline, used as the "maintenance due" indicator glyph - the
// back-office equivalent of a gear-position badge, repurposed here as a service
// reminder rather than a transmission-gear readout.
void paintMaintenanceBadgeIcon(QPainter& painter, const QRectF& bounds, const QColor& color, bool due);

// A small analogue-style gauge with a needle, reused from the onboard rider
// dashboard's fuel-gauge visual language to represent battery level (the e-bike/
// e-scooter equivalent of a fuel gauge).
void paintBatteryGaugeIcon(QPainter& painter, const QRectF& bounds, double percent);

} // namespace icons

} // namespace qttutorial::two_wheelers::fleet
