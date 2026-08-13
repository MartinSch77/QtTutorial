// SPDX-License-Identifier: MIT
#include "FleetMaintenanceDelegate.h"
#include "FleetModel.h"

#include <QPainter>
#include <QPainterPath>
#include <QRectF>

namespace qttutorial::automotive::fleet {

FleetMaintenanceDelegate::FleetMaintenanceDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void FleetMaintenanceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }

    const bool due = index.data(FleetModel::MaintenanceDueRole).toBool();
    const QString text = index.data(Qt::DisplayRole).toString();

    const int iconSize = qMin(18, option.rect.height() - 4);
    const QRect iconRect(option.rect.left() + 6, option.rect.top() + (option.rect.height() - iconSize) / 2,
                          iconSize, iconSize);

    if (due) {
        // A simple vector wrench: two rounded "jaw" circles joined by a diagonal
        // handle, drawn as paths rather than an imported glyph or SVG asset.
        painter->setPen(QPen(QColor("#ff9f43"), 2));
        painter->setBrush(Qt::NoBrush);
        QPainterPath handle;
        handle.moveTo(iconRect.left() + iconRect.width() * 0.2, iconRect.bottom() - iconRect.height() * 0.1);
        handle.lineTo(iconRect.left() + iconRect.width() * 0.75, iconRect.top() + iconRect.height() * 0.15);
        painter->drawPath(handle);
        painter->drawEllipse(QRectF(iconRect.left(), iconRect.bottom() - iconRect.height() * 0.35,
                                     iconRect.width() * 0.35, iconRect.width() * 0.35));
        painter->drawEllipse(QRectF(iconRect.left() + iconRect.width() * 0.6, iconRect.top(),
                                     iconRect.width() * 0.4, iconRect.width() * 0.4));
    } else {
        // A simple vector check-mark badge: a circle outline with a check stroke.
        painter->setPen(QPen(QColor("#3ddc6f"), 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(iconRect);
        QPainterPath check;
        check.moveTo(iconRect.left() + iconRect.width() * 0.25, iconRect.top() + iconRect.height() * 0.55);
        check.lineTo(iconRect.left() + iconRect.width() * 0.45, iconRect.top() + iconRect.height() * 0.75);
        check.lineTo(iconRect.left() + iconRect.width() * 0.78, iconRect.top() + iconRect.height() * 0.3);
        painter->drawPath(check);
    }

    painter->setPen(option.palette.text().color());
    const QRect textRect = option.rect.adjusted(iconRect.width() + 14, 0, 0, 0);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);

    painter->restore();
}

QSize FleetMaintenanceDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize base = QStyledItemDelegate::sizeHint(option, index);
    base.setWidth(base.width() + 24);
    return base;
}

} // namespace qttutorial::automotive::fleet
