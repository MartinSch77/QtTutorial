// SPDX-License-Identifier: MIT
#pragma once

#include <QColor>
#include <QIcon>

class QPainter;
class QRectF;

namespace qttutorial::mining::pit {

// Small procedurally-drawn vector glyphs shared by the fleet table and the
// pit-map view, painted with QPainter/QPainterPath rather than loaded from
// any icon font or image asset. Kept deliberately generic/geometric: no
// manufacturer trademark, logo or exact vehicle profile is reproduced.
namespace icons {

// A filled warning-triangle icon, e.g. for flagging an overloaded truck row.
[[nodiscard]] QIcon warningTriangleIcon(const QColor& color, int pixelSize);

// Paints a simplified haul-truck glyph (cab + raised dump body) into rect,
// used as the truck marker on the pit-map view.
void paintTruckGlyph(QPainter& painter, const QRectF& rect, const QColor& color);

// Paints a top-down open-pit motif (concentric benches) into rect, used as
// the pit-map's backdrop.
void paintPitBenches(QPainter& painter, const QRectF& rect, const QColor& benchColor);

} // namespace icons

} // namespace qttutorial::mining::pit
