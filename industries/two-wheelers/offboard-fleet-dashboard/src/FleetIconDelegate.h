// SPDX-License-Identifier: MIT
#pragma once

#include <QStyledItemDelegate>

namespace qttutorial::two_wheelers::fleet {

// Paints a small procedurally-drawn vector icon (see FleetIcons.h) alongside the
// existing text for a handful of FleetModel columns, so the table reads as a
// vehicle fleet rather than a generic spreadsheet: a motorcycle silhouette next
// to the vehicle id, an analogue-style gauge next to the battery percentage, and
// a cog/service badge next to the maintenance-due flag.
class FleetIconDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit FleetIconDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

} // namespace qttutorial::two_wheelers::fleet
