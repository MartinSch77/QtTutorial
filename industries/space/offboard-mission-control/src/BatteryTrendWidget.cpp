// SPDX-License-Identifier: MIT
#include "BatteryTrendWidget.h"

#include <QPainter>

#include <utility>

namespace qttutorial::space {

BatteryTrendWidget::BatteryTrendWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(320, 180);
}

void BatteryTrendWidget::setSeries(std::vector<double> batteryPercent, std::vector<bool> inEclipse)
{
    m_batteryPercent = std::move(batteryPercent);
    m_inEclipse = std::move(inEclipse);
    update();
}

void BatteryTrendWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x14, 0x14, 0x1c));

    if (m_batteryPercent.empty()) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, tr("Select a satellite to view its battery trend"));
        return;
    }

    const std::size_t count = m_batteryPercent.size();
    const double stepX = count > 1 ? width() / static_cast<double>(count - 1) : 0.0;
    const auto toY = [&](double percent) { return height() - percent / 100.0 * height(); };

    for (std::size_t i = 0; i < count; ++i) {
        if (i < m_inEclipse.size() && m_inEclipse[i]) {
            const double x = static_cast<double>(i) * stepX;
            painter.fillRect(QRectF(x, 0.0, stepX + 1.0, height()), QColor(0x24, 0x24, 0x38));
        }
    }

    painter.setPen(QPen(Qt::gray, 1.0, Qt::DashLine));
    painter.drawLine(QPointF(0.0, toY(30.0)), QPointF(width(), toY(30.0)));

    painter.setPen(QPen(QColor(0x40, 0xd0, 0xff), 2.0));
    QPolygonF polyline;
    for (std::size_t i = 0; i < count; ++i) {
        polyline << QPointF(static_cast<double>(i) * stepX, toY(m_batteryPercent[i]));
    }
    painter.drawPolyline(polyline);
}

} // namespace qttutorial::space
