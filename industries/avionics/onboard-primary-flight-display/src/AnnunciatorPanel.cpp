// SPDX-License-Identifier: MIT
#include "AnnunciatorPanel.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace qttutorial::avionics {

namespace {

// Draws a plain geometric caution triangle (outline + exclamation mark) into
// `rect`, entirely from primitive path segments - no external SVG/image asset.
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

    const double stemTop = rect.top() + rect.height() * 0.38;
    const double stemBottom = rect.top() + rect.height() * 0.68;
    const double midX = rect.center().x();
    painter.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(midX, stemTop), QPointF(midX, stemBottom));
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(midX, rect.top() + rect.height() * 0.8), 1.6, 1.6);
}

} // namespace

AnnunciatorPanel::AnnunciatorPanel(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(220, 70);
}

void AnnunciatorPanel::setMessages(std::vector<CautionMessage> messages)
{
    m_messages = std::move(messages);
    update();
}

void AnnunciatorPanel::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x10, 0x10, 0x10));

    const double masterWidth = 56.0;
    const QRectF masterRect(4.0, 4.0, masterWidth, height() - 8.0);
    const bool anyActive = !m_messages.empty();
    painter.setPen(QPen(Qt::white, 1.0));
    painter.setBrush(anyActive ? QColor(0xd0, 0x30, 0x30) : QColor(0x20, 0x20, 0x20));
    painter.drawRect(masterRect);
    painter.setPen(anyActive ? Qt::white : QColor(0x60, 0x60, 0x60));
    QFont masterFont = font();
    masterFont.setBold(true);
    masterFont.setPointSize(9);
    painter.setFont(masterFont);
    painter.drawText(masterRect, Qt::AlignCenter, tr("MASTER\nCAUTION"));

    const double slotX = masterRect.right() + 8.0;
    const double slotWidth = std::max(40.0, (width() - slotX - 8.0) / 4.0);
    QFont legendFont = font();
    legendFont.setPointSize(8);
    legendFont.setBold(true);
    painter.setFont(legendFont);

    for (int i = 0; i < 4; ++i) {
        const QRectF slotRect(slotX + i * (slotWidth + 4.0), 4.0, slotWidth, height() - 8.0);
        const bool active = i < static_cast<int>(m_messages.size());
        const QColor color = active && m_messages[static_cast<std::size_t>(i)].severity == CautionSeverity::Warning
            ? QColor(0xe0, 0x30, 0x30)
            : QColor(0xd0, 0xa0, 0x20);

        painter.setPen(QPen(Qt::white, 1.0));
        painter.setBrush(active ? color.darker(140) : QColor(0x18, 0x18, 0x18));
        painter.drawRect(slotRect);

        if (!active) {
            continue;
        }

        const QRectF iconRect(slotRect.left() + 4.0, slotRect.top() + 4.0, 16.0, 16.0);
        drawCautionTriangle(painter, iconRect, Qt::black);

        const QRectF textRect(iconRect.right() + 2.0, slotRect.top(), slotRect.right() - iconRect.right() - 4.0,
                               slotRect.height());
        painter.setPen(Qt::black);
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWordWrap,
                          QString::fromStdString(m_messages[static_cast<std::size_t>(i)].id));
    }
}

} // namespace qttutorial::avionics
