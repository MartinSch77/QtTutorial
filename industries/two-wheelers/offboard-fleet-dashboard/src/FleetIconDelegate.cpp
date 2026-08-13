// SPDX-License-Identifier: MIT
#include "FleetIconDelegate.h"

#include "FleetIcons.h"
#include "FleetModel.h"

#include <QColor>
#include <QPainter>
#include <QStyleOptionViewItem>

namespace qttutorial::two_wheelers::fleet {

FleetIconDelegate::FleetIconDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void FleetIconDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    const int column = index.column();
    if (column != FleetModel::IdColumn && column != FleetModel::BatteryColumn
        && column != FleetModel::MaintenanceColumn) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    const qreal iconSize = static_cast<qreal>(option.rect.height()) - 8;

    // Shrink the text area so the icon has its own space on the left rather than
    // overlapping the existing text-based readout.
    QStyleOptionViewItem textOption(option);
    textOption.rect.setLeft(option.rect.left() + static_cast<int>(iconSize) + 10);
    QStyledItemDelegate::paint(painter, textOption, index);

    painter->save();
    const QRectF iconRect(option.rect.left() + 4, option.rect.top() + 4, iconSize, iconSize);

    switch (column) {
    case FleetModel::IdColumn:
        icons::paintMotorcycleIcon(*painter, iconRect, option.palette.text().color());
        break;
    case FleetModel::BatteryColumn: {
        const double percent = index.data(Qt::UserRole).toDouble();
        icons::paintBatteryGaugeIcon(*painter, iconRect, percent);
        break;
    }
    case FleetModel::MaintenanceColumn: {
        const bool due = index.data(Qt::UserRole).toBool();
        icons::paintMaintenanceBadgeIcon(*painter, iconRect, due ? QColor("#ff9f43") : QColor("#5ad46a"), due);
        break;
    }
    default:
        break;
    }

    painter->restore();
}

} // namespace qttutorial::two_wheelers::fleet
