// SPDX-License-Identifier: MIT
#include "TrendWidget.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace qttutorial::two_wheelers::fleet {

TrendWidget::TrendWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(160);
}

void TrendWidget::setValues(const std::vector<double>& values, double maxValue)
{
    m_values = values;
    m_maxValue = maxValue > 0.0 ? maxValue : 1.0;
    update();
}

void TrendWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#1c212b"));

    if (m_values.size() < 2) {
        painter.setPen(QColor("#9aa4b2"));
        painter.drawText(rect(), Qt::AlignCenter, tr("Collecting history..."));
        return;
    }

    QPainterPath path;
    const double stepX = static_cast<double>(width()) / static_cast<double>(m_values.size() - 1);
    for (std::size_t i = 0; i < m_values.size(); ++i) {
        const double x = static_cast<double>(i) * stepX;
        const double normalised = std::clamp(m_values[i] / m_maxValue, 0.0, 1.0);
        const double y = height() - normalised * height();
        if (i == 0) {
            path.moveTo(x, y);
        } else {
            path.lineTo(x, y);
        }
    }

    painter.setPen(QPen(QColor("#39c0ff"), 2));
    painter.drawPath(path);
}

} // namespace qttutorial::two_wheelers::fleet
