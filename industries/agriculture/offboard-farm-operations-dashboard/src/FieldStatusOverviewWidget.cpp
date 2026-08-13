// SPDX-License-Identifier: MIT
#include "FieldStatusOverviewWidget.h"

#include "FarmIconPainter.h"

#include <QPainter>

#include <algorithm>

namespace qttutorial::agriculture::ops {

namespace {

QColor colorForOverallStatus(const QString& overallStatus)
{
    if (overallStatus == QStringLiteral("complete")) {
        return {0x3d, 0xdc, 0x6f};
    }
    if (overallStatus == QStringLiteral("in_progress")) {
        return {0xe5, 0xb9, 0x3d};
    }
    return {0x5a, 0x62, 0x70}; // not_started
}

QString labelForOverallStatus(const QString& overallStatus)
{
    if (overallStatus == QStringLiteral("complete")) {
        return QObject::tr("Done");
    }
    if (overallStatus == QStringLiteral("in_progress")) {
        return QObject::tr("In progress");
    }
    return QObject::tr("Not started");
}

} // namespace

FieldStatusOverviewWidget::FieldStatusOverviewWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(120);
}

void FieldStatusOverviewWidget::setSamples(const std::vector<FieldSample>& samples)
{
    m_samples = samples;
    update();
}

void FieldStatusOverviewWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#12151b"));

    painter.setPen(QColor("#9aa4b2"));
    QFont headerFont = painter.font();
    headerFont.setBold(true);
    headerFont.setPointSize(headerFont.pointSize() + 1);
    painter.setFont(headerFont);
    icons::paintFieldGlyph(painter, QRectF(8, 6, 20, 20), QColor("#9aa4b2"));
    painter.drawText(QRect(34, 4, 260, 24), Qt::AlignVCenter | Qt::AlignLeft, tr("FARM-WIDE FIELD STATUS"));

    if (m_samples.empty()) {
        return;
    }

    const int top = 34;
    const int tileSpacing = 10;
    const int tileWidth = std::max(72, (width() - tileSpacing * (static_cast<int>(m_samples.size()) + 1))
                                            / static_cast<int>(m_samples.size()));
    const int tileHeight = height() - top - 8;

    QFont bodyFont = painter.font();
    bodyFont.setBold(false);
    bodyFont.setPointSize(bodyFont.pointSize() - 1);

    int x = tileSpacing;
    for (const FieldSample& sample : m_samples) {
        const QRect tileRect(x, top, tileWidth, tileHeight);
        const QColor statusColor = colorForOverallStatus(sample.overallStatus);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0x1c, 0x21, 0x2b));
        painter.drawRoundedRect(tileRect, 6, 6);

        painter.setPen(QPen(statusColor, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(tileRect.adjusted(1, 1, -1, -1), 6, 6);

        painter.setPen(QColor("#f2f4f8"));
        painter.setFont(bodyFont);
        painter.drawText(tileRect.adjusted(6, 6, -6, 0), Qt::AlignTop | Qt::AlignLeft, sample.fieldId);

        painter.setPen(statusColor);
        painter.drawText(tileRect.adjusted(6, 0, -6, -6), Qt::AlignBottom | Qt::AlignLeft,
                          labelForOverallStatus(sample.overallStatus));

        painter.setPen(QColor("#9aa4b2"));
        painter.drawText(tileRect.adjusted(6, 24, -6, -24), Qt::AlignVCenter | Qt::AlignLeft,
                          tr("%1/%2 passes").arg(sample.passNumber).arg(sample.plannedPasses));

        x += tileWidth + tileSpacing;
    }
}

} // namespace qttutorial::agriculture::ops
