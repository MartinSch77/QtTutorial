// SPDX-License-Identifier: MIT
#pragma once

#include <QStyledItemDelegate>

namespace qttutorial::automotive::fleet {

// Paints the Maintenance column as a small vector icon (a wrench when service is
// due soon, a check-mark badge otherwise) instead of plain text, hand-drawn with
// QPainter paths - the Qt Widgets equivalent of the onboard app's Canvas-painted
// icons, since this app has no QML/Canvas available. No external icon assets.
class FleetMaintenanceDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit FleetMaintenanceDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

} // namespace qttutorial::automotive::fleet
