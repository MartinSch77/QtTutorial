// SPDX-License-Identifier: MIT
#include "TrendChartWidget.h"

#include "VitalsHistoryStore.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace qttutorial::medical::telehealth {

TrendChartWidget::TrendChartWidget(QWidget* parent)
    : QWidget(parent)
{
}

void TrendChartWidget::setHistoryStore(VitalsHistoryStore* store)
{
    m_store = store;
    update();
}

void TrendChartWidget::setSelectedPatient(const QString& patientId, const QString& patientName)
{
    m_patientId = patientId;
    m_patientName = patientName;
    refresh();
}

void TrendChartWidget::refresh()
{
    update();
}

QSize TrendChartWidget::sizeHint() const
{
    return QSize(500, 160);
}

void TrendChartWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF background = rect();
    painter.fillRect(background, QColor("#1c212b"));
    painter.setPen(QColor("#2a2f3a"));
    painter.drawRect(background.adjusted(0, 0, -1, -1));

    const QString title = m_patientName.isEmpty()
        ? tr("Heart-rate trend - select a patient")
        : tr("Heart-rate trend - %1 (last %2 min)").arg(m_patientName).arg(TrendChartWidget::kTrendMinutes);
    painter.setPen(Qt::white);
    painter.drawText(QRectF(10, 6, background.width() - 20, 20), Qt::AlignLeft | Qt::AlignVCenter, title);

    const QRectF chartRect = background.adjusted(50, 32, -14, -14);

    if (m_store == nullptr || m_patientId.isEmpty()) {
        painter.setPen(QColor("#9aa4b2"));
        painter.drawText(chartRect, Qt::AlignCenter, tr("No patient selected"));
        return;
    }

    const std::vector<double> samples = m_store->recentHeartRates(m_patientId, TrendChartWidget::kMaxSamples);
    if (samples.size() < 2) {
        painter.setPen(QColor("#9aa4b2"));
        painter.drawText(chartRect, Qt::AlignCenter, tr("Collecting samples..."));
        return;
    }

    double minValue = samples.front();
    double maxValue = minValue;
    for (double v : samples) {
        minValue = std::min(minValue, v);
        maxValue = std::max(maxValue, v);
    }
    // Pad the range a little so the trace does not touch the top/bottom
    // edge, and guard against a perfectly flat trend.
    const double padding = std::max(2.0, (maxValue - minValue) * 0.15);
    minValue -= padding;
    maxValue += padding;
    const double range = std::max(1.0, maxValue - minValue);

    painter.setPen(QColor("#2a2f3a"));
    for (int i = 0; i <= 4; ++i) {
        const double y = chartRect.top() + chartRect.height() * i / 4.0;
        painter.drawLine(QPointF(chartRect.left(), y), QPointF(chartRect.right(), y));
    }

    painter.setPen(QColor("#9aa4b2"));
    painter.drawText(QRectF(2, chartRect.top() - 6, 44, 16), Qt::AlignRight | Qt::AlignVCenter,
                      QString::number(qRound(maxValue)));
    painter.drawText(QRectF(2, chartRect.bottom() - 10, 44, 16), Qt::AlignRight | Qt::AlignVCenter,
                      QString::number(qRound(minValue)));

    QPainterPath path;
    const double stepX = chartRect.width() / static_cast<double>(samples.size() - 1);
    for (std::size_t i = 0; i < samples.size(); ++i) {
        const double normalised = (samples[i] - minValue) / range;
        const double x = chartRect.left() + static_cast<double>(i) * stepX;
        const double y = chartRect.bottom() - normalised * chartRect.height();
        if (i == 0) {
            path.moveTo(x, y);
        } else {
            path.lineTo(x, y);
        }
    }
    painter.setPen(QPen(QColor("#39c0ff"), 2));
    painter.drawPath(path);
}

} // namespace qttutorial::medical::telehealth
