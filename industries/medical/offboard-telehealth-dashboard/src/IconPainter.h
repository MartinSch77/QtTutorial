// SPDX-License-Identifier: MIT
#pragma once

#include <QColor>
#include <QRectF>

class QPainter;

namespace qttutorial::medical::telehealth {

// Small procedurally-drawn clinical icons used as labels next to each
// vital-sign panel, so the dashboard reads as a real clinical tool rather
// than a generic list of numbers. Every icon is hand-drawn with QPainter
// paths (lines, cubic beziers, arcs) at paint time - no external icon font,
// image, or SVG asset is loaded.
class IconPainter {
public:
    static void paintHeartbeat(QPainter& painter, const QRectF& rect, const QColor& color);
    static void paintDroplet(QPainter& painter, const QRectF& rect, const QColor& color);
    static void paintCuff(QPainter& painter, const QRectF& rect, const QColor& color);
    static void paintBell(QPainter& painter, const QRectF& rect, const QColor& color);
};

} // namespace qttutorial::medical::telehealth
