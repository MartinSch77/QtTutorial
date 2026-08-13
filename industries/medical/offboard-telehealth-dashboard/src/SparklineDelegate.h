// SPDX-License-Identifier: MIT
#pragma once

#include <QStyledItemDelegate>

namespace qttutorial::medical::telehealth {

// Paints each patient row as a text summary plus a small QPainter-drawn
// heart-rate sparkline (no charting library) using PatientListModel::TrendRole.
class SparklineDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit SparklineDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

} // namespace qttutorial::medical::telehealth
