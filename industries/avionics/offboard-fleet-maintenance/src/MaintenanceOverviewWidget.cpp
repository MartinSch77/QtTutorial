// SPDX-License-Identifier: MIT
#include "MaintenanceOverviewWidget.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace qttutorial::avionics {

namespace {

// A plain geometric caution triangle (outline + exclamation), drawn from
// primitive path segments - no external SVG/image asset - mirroring the genre
// convention used by the onboard PFD's annunciator panel, without sharing code
// across the two independent apps.
void drawCautionTriangle(QPainter& painter, const QRectF& rect, const QColor& color)
{
    QPainterPath triangle;
    triangle.moveTo(rect.center().x(), rect.top());
    triangle.lineTo(rect.right(), rect.bottom());
    triangle.lineTo(rect.left(), rect.bottom());
    triangle.closeSubpath();

    painter.setPen(QPen(color, 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(triangle);

    const double midX = rect.center().x();
    painter.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(midX, rect.top() + rect.height() * 0.38), QPointF(midX, rect.top() + rect.height() * 0.66));
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(QPointF(midX, rect.top() + rect.height() * 0.8), 1.4, 1.4);
}

QColor urgencyColor(InspectionUrgency urgency)
{
    switch (urgency) {
    case InspectionUrgency::Urgent:
        return QColor(0xd0, 0x40, 0x40);
    case InspectionUrgency::DueSoon:
        return QColor(0xd0, 0xa0, 0x20);
    case InspectionUrgency::Nominal:
    default:
        return QColor(0x40, 0xa0, 0x50);
    }
}

QString urgencyLabel(InspectionUrgency urgency)
{
    switch (urgency) {
    case InspectionUrgency::Urgent:
        return QStringLiteral("URGENT");
    case InspectionUrgency::DueSoon:
        return QStringLiteral("Due soon");
    case InspectionUrgency::Nominal:
    default:
        return QStringLiteral("Nominal");
    }
}

} // namespace

MaintenanceOverviewWidget::MaintenanceOverviewWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(320, 160);
}

void MaintenanceOverviewWidget::setEntries(std::vector<FleetOverviewEntry> entries)
{
    std::sort(entries.begin(), entries.end(), [](const FleetOverviewEntry& a, const FleetOverviewEntry& b) {
        if (a.status.urgency != b.status.urgency) {
            return a.status.urgency > b.status.urgency;
        }
        return a.status.hoursRemaining < b.status.hoursRemaining;
    });
    m_entries = std::move(entries);
    update();
}

void MaintenanceOverviewWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x1c, 0x1c, 0x1c));

    if (m_entries.empty()) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, tr("No fleet data"));
        return;
    }

    const double rowHeight = std::max(18.0, height() / static_cast<double>(m_entries.size()));
    const double labelWidth = 70.0;
    const double iconWidth = 20.0;
    const double barLeft = labelWidth + iconWidth;
    const double barRight = width() - 90.0;

    QFont labelFont = font();
    labelFont.setBold(true);
    labelFont.setPointSize(9);

    for (std::size_t i = 0; i < m_entries.size(); ++i) {
        const FleetOverviewEntry& entry = m_entries[i];
        const double y = static_cast<double>(i) * rowHeight;
        const QRectF rowRect(0.0, y, width(), rowHeight - 2.0);

        painter.setFont(labelFont);
        painter.setPen(Qt::white);
        painter.drawText(QRectF(4.0, y, labelWidth - 8.0, rowRect.height()), Qt::AlignVCenter | Qt::AlignLeft,
                          entry.tailNumber);

        if (entry.status.urgency == InspectionUrgency::Urgent) {
            drawCautionTriangle(painter, QRectF(labelWidth, y + 2.0, iconWidth - 4.0, rowRect.height() - 4.0),
                                 QColor(0xe0, 0x40, 0x40));
        }

        const double fractionRemaining =
            std::clamp(entry.status.hoursRemaining / InspectionScheduler::kIntervalHours, 0.0, 1.0);
        const QRectF barRect(barLeft, y + rowRect.height() * 0.2, barRight - barLeft, rowRect.height() * 0.6);
        painter.setPen(QPen(QColor(0x50, 0x50, 0x50), 1.0));
        painter.setBrush(QColor(0x28, 0x28, 0x28));
        painter.drawRect(barRect);
        const QRectF filledRect(barRect.left(), barRect.top(), barRect.width() * fractionRemaining, barRect.height());
        painter.setPen(Qt::NoPen);
        painter.setBrush(urgencyColor(entry.status.urgency));
        painter.drawRect(filledRect);

        painter.setPen(Qt::white);
        QFont valueFont = font();
        valueFont.setPointSize(8);
        painter.setFont(valueFont);
        painter.drawText(QRectF(barRight + 4.0, y, 86.0, rowRect.height()), Qt::AlignVCenter | Qt::AlignLeft,
                          tr("%1h / %2 cyc")
                              .arg(QString::number(entry.status.hoursRemaining, 'f', 0))
                              .arg(entry.status.cyclesRemaining));

        painter.setPen(urgencyColor(entry.status.urgency));
        painter.drawText(barRect, Qt::AlignCenter, urgencyLabel(entry.status.urgency));
    }
}

} // namespace qttutorial::avionics
