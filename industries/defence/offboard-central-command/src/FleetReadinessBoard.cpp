// SPDX-License-Identifier: MIT
#include "FleetReadinessBoard.h"

#include <QFont>
#include <QPainter>
#include <QPainterPath>

namespace qttutorial::defence {

namespace {
void drawShield(QPainter& painter, QPointF center, double size, const QColor& color, bool ok)
{
    QPainterPath path;
    path.moveTo(center.x(), center.y() - size);
    path.lineTo(center.x() + size * 0.8, center.y() - size * 0.6);
    path.lineTo(center.x() + size * 0.8, center.y() + size * 0.3);
    path.quadTo(center.x() + size * 0.8, center.y() + size * 1.1, center.x(), center.y() + size * 1.3);
    path.quadTo(center.x() - size * 0.8, center.y() + size * 1.1, center.x() - size * 0.8, center.y() + size * 0.3);
    path.lineTo(center.x() - size * 0.8, center.y() - size * 0.6);
    path.closeSubpath();

    painter.setPen(QPen(QColor(0x0c, 0x0f, 0x14), 1.5));
    painter.setBrush(color);
    painter.drawPath(path);

    painter.setPen(QPen(Qt::white, 2));
    if (ok) {
        painter.drawLine(QPointF(center.x() - size * 0.35, center.y()), QPointF(center.x() - size * 0.1, center.y() + size * 0.3));
        painter.drawLine(QPointF(center.x() - size * 0.1, center.y() + size * 0.3), QPointF(center.x() + size * 0.4, center.y() - size * 0.35));
    } else {
        painter.drawLine(QPointF(center.x() - size * 0.3, center.y() - size * 0.3), QPointF(center.x() + size * 0.3, center.y() + size * 0.3));
        painter.drawLine(QPointF(center.x() + size * 0.3, center.y() - size * 0.3), QPointF(center.x() - size * 0.3, center.y() + size * 0.3));
    }
}
}

FleetReadinessBoard::FleetReadinessBoard(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(96);
}

void FleetReadinessBoard::setSummary(FleetReadinessSummary summary)
{
    m_summary = summary;
    update();
}

void FleetReadinessBoard::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(0x12, 0x15, 0x1b));

    const struct { const char* label; int count; QColor color; bool ok; } tiles[] = {
        {"Nominal", m_summary.nominal, QColor(0x2f, 0xa8, 0x4f), true},
        {"Caution", m_summary.caution, QColor(0xe0, 0xa3, 0x00), false},
        {"Critical", m_summary.critical, QColor(0xc0, 0x39, 0x2b), false},
    };

    const double tileWidth = width() / 3.0;
    QFont countFont = painter.font();
    countFont.setBold(true);
    countFont.setPointSize(20);

    for (int i = 0; i < 3; ++i) {
        const QRectF tileRect(i * tileWidth, 0, tileWidth, height());
        drawShield(painter, QPointF(tileRect.x() + 26, tileRect.center().y() - 6), 16, tiles[i].color, tiles[i].ok);

        painter.setFont(countFont);
        painter.setPen(Qt::white);
        painter.drawText(QRectF(tileRect.x() + 48, tileRect.y(), tileWidth - 56, height() * 0.6),
                          Qt::AlignLeft | Qt::AlignBottom, QString::number(tiles[i].count));

        QFont labelFont = painter.font();
        labelFont.setBold(false);
        labelFont.setPointSize(10);
        painter.setFont(labelFont);
        painter.setPen(QColor(0x9a, 0xa4, 0xb2));
        painter.drawText(QRectF(tileRect.x() + 48, tileRect.center().y(), tileWidth - 56, height() * 0.4),
                          Qt::AlignLeft | Qt::AlignTop, QString::fromLatin1(tiles[i].label));
    }
}

} // namespace qttutorial::defence
