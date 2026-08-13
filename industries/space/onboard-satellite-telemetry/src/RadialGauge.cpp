// SPDX-License-Identifier: MIT
#include "RadialGauge.h"

#include <QPainter>

#include <algorithm>
#include <utility>

namespace qttutorial::space {

namespace {
constexpr double kStartAngleDeg = 225.0;
constexpr double kSpanAngleDeg = 270.0;
}

RadialGauge::RadialGauge(QString title, QString unit, double minValue, double maxValue, QWidget* parent)
    : QWidget(parent)
    , m_title(std::move(title))
    , m_unit(std::move(unit))
    , m_minValue(minValue)
    , m_maxValue(maxValue)
    , m_value(minValue)
{
    setMinimumSize(160, 160);
}

void RadialGauge::setValue(double value)
{
    m_value = std::clamp(value, m_minValue, m_maxValue);
    update();
}

void RadialGauge::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x12, 0x12, 0x18));

    const double side = std::min(width(), height());
    const QRectF arcRect((width() - side) / 2.0 + 10.0, (height() - side) / 2.0 + 10.0, side - 20.0, side - 20.0);

    QPen backgroundPen(QColor(0x40, 0x40, 0x50), 10.0);
    painter.setPen(backgroundPen);
    painter.drawArc(arcRect, static_cast<int>(-kStartAngleDeg * 16.0), static_cast<int>(-kSpanAngleDeg * 16.0));

    const double fraction = (m_value - m_minValue) / (m_maxValue - m_minValue);
    QPen valuePen(QColor(0x40, 0xc0, 0xff), 10.0);
    painter.setPen(valuePen);
    painter.drawArc(arcRect, static_cast<int>(-kStartAngleDeg * 16.0),
                     static_cast<int>(-kSpanAngleDeg * fraction * 16.0));

    painter.setPen(Qt::white);
    QFont valueFont = font();
    valueFont.setPointSize(16);
    valueFont.setBold(true);
    painter.setFont(valueFont);
    painter.drawText(arcRect, Qt::AlignCenter, QString::number(m_value, 'f', 1) + QStringLiteral("\n") + m_unit);

    QFont titleFont = font();
    titleFont.setPointSize(9);
    painter.setFont(titleFont);
    painter.drawText(QRectF(0.0, height() - 20.0, width(), 18.0), Qt::AlignCenter, m_title);
}

} // namespace qttutorial::space
