// SPDX-License-Identifier: MIT
#include "VerticalTape.h"

#include <QPainter>

#include <cmath>

namespace qttutorial::avionics {

VerticalTape::VerticalTape(double majorStep, double pixelsPerUnit, QWidget* parent)
    : QWidget(parent)
    , m_majorStep(majorStep)
    , m_pixelsPerUnit(pixelsPerUnit)
{
    setMinimumSize(90, 260);
}

void VerticalTape::setValue(double value)
{
    m_value = value;
    update();
}

void VerticalTape::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x10, 0x10, 0x10));

    const double centerY = height() / 2.0;
    const double firstVisibleMark = std::floor((m_value - height() / 2.0 / m_pixelsPerUnit) / m_majorStep) * m_majorStep;

    painter.setPen(QPen(Qt::white, 1.0));
    for (double mark = firstVisibleMark; mark < m_value + height() / 2.0 / m_pixelsPerUnit + m_majorStep; mark += m_majorStep) {
        const double y = centerY - (mark - m_value) * m_pixelsPerUnit;
        if (y < -10.0 || y > height() + 10.0) {
            continue;
        }
        painter.drawLine(QPointF(width() - 18.0, y), QPointF(width() - 4.0, y));
        painter.drawText(QRectF(0.0, y - 9.0, width() - 22.0, 18.0), Qt::AlignRight | Qt::AlignVCenter,
                          QString::number(mark, 'f', 0));
    }

    const QRectF readoutRect(4.0, centerY - 14.0, width() - 8.0, 28.0);
    painter.setPen(QPen(Qt::yellow, 2.0));
    painter.setBrush(QColor(0x10, 0x10, 0x10));
    painter.drawRect(readoutRect);
    painter.setPen(Qt::yellow);
    QFont readoutFont = font();
    readoutFont.setBold(true);
    readoutFont.setPointSize(12);
    painter.setFont(readoutFont);
    painter.drawText(readoutRect, Qt::AlignCenter, QString::number(m_value, 'f', 0));
}

} // namespace qttutorial::avionics
