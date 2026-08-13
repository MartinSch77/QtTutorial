// SPDX-License-Identifier: MIT
#include "FieldMapWidget.h"

#include "FarmIconPainter.h"

#include <QPainter>
#include <QRectF>

#include <algorithm>

namespace qttutorial::agriculture::ops {

namespace {

QPointF pointOnRectanglePerimeter(const QRectF& rect, double fraction)
{
    const double perimeter = 2.0 * (rect.width() + rect.height());
    double distance = fraction * perimeter;

    if (distance <= rect.width()) {
        return {rect.left() + distance, rect.top()};
    }
    distance -= rect.width();
    if (distance <= rect.height()) {
        return {rect.right(), rect.top() + distance};
    }
    distance -= rect.height();
    if (distance <= rect.width()) {
        return {rect.right() - distance, rect.bottom()};
    }
    distance -= rect.width();
    return {rect.left(), rect.bottom() - distance};
}

} // namespace

FieldMapWidget::FieldMapWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(220);
}

void FieldMapWidget::setProgress(double coveragePercent, bool hasData)
{
    m_coveragePercent = coveragePercent;
    m_hasData = hasData;
    update();
}

void FieldMapWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#1c212b"));

    if (!m_hasData) {
        painter.setPen(QColor("#9aa4b2"));
        painter.drawText(rect(), Qt::AlignCenter, tr("Select a field to see its position"));
        return;
    }

    const QRectF fieldRect(rect().adjusted(24, 24, -24, -24));

    painter.setPen(QPen(QColor("#5a7a4a"), 3));
    painter.setBrush(QColor("#233a1e"));
    painter.drawRect(fieldRect);

    const double fraction = std::clamp(m_coveragePercent / 100.0, 0.0, 1.0);
    const QPointF marker = pointOnRectanglePerimeter(fieldRect, fraction);

    const QRectF markerRect(marker.x() - 14.0, marker.y() - 14.0, 28.0, 28.0);
    icons::paintTractorGlyph(painter, markerRect, QColor("#e5b93d"));
}

} // namespace qttutorial::agriculture::ops
