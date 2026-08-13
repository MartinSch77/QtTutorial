// SPDX-License-Identifier: MIT
#include "PassScheduleWidget.h"

#include "IconPainter.h"

#include <QPainter>

#include <utility>

namespace qttutorial::space {

PassScheduleWidget::PassScheduleWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(280, 160);
}

void PassScheduleWidget::setContactWindows(std::vector<ContactWindow> windows)
{
    m_windows = std::move(windows);
    update();
}

void PassScheduleWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x14, 0x14, 0x1c));

    painter.setPen(Qt::white);
    QFont headerFont = font();
    headerFont.setBold(true);
    headerFont.setPointSize(10);
    painter.setFont(headerFont);
    painter.drawText(QRectF(8.0, 4.0, width() - 16.0, 20.0), Qt::AlignLeft, tr("Next Contact Window"));

    QFont rowFont = font();
    rowFont.setPointSize(9);
    painter.setFont(rowFont);

    const double rowHeight = 24.0;
    double y = 28.0;
    for (const ContactWindow& window : m_windows) {
        icons::drawGroundStation(painter, QRectF(6.0, y, 18.0, 18.0),
                                  window.inContactNow ? QColor(0x40, 0xd0, 0x80) : QColor(0x70, 0x78, 0x88),
                                  window.inContactNow);

        painter.setPen(Qt::white);
        const QString text = window.inContactNow
            ? tr("%1 — in contact with %2").arg(window.satelliteName, window.stationName)
            : tr("%1 — %2 in %3 min").arg(window.satelliteName, window.stationName)
                  .arg(window.minutesUntilNextContact, 0, 'f', 1);
        painter.drawText(QRectF(30.0, y, width() - 38.0, rowHeight), Qt::AlignVCenter | Qt::AlignLeft, text);

        y += rowHeight;
    }

    if (m_windows.empty()) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, tr("No contact data yet"));
    }
}

} // namespace qttutorial::space
