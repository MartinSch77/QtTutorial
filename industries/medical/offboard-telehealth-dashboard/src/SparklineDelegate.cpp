// SPDX-License-Identifier: MIT
#include "SparklineDelegate.h"

#include "PatientListModel.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace qttutorial::medical::telehealth {

SparklineDelegate::SparklineDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void SparklineDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    const int severity = index.data(PatientListModel::SeverityRole).toInt();
    const QColor background = severity == 2 ? QColor("#3a1e1e") : severity == 1 ? QColor("#3a331e")
                                                                                 : QColor("#1c212b");
    painter->fillRect(option.rect, background);

    const QRect textRect = option.rect.adjusted(10, 6, -140, -6);
    painter->setPen(Qt::white);
    const QString summary = QStringLiteral("%1  %2 bpm  SpO2 %3%%  BP %4")
                                 .arg(index.data(PatientListModel::NameRole).toString())
                                 .arg(qRound(index.data(PatientListModel::HeartRateRole).toDouble()))
                                 .arg(qRound(index.data(PatientListModel::Spo2Role).toDouble()))
                                 .arg(index.data(PatientListModel::BloodPressureRole).toString());
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, summary);

    const QRect sparkRect = option.rect.adjusted(option.rect.width() - 130, 10, -10, -10);
    const QVariantList trend = index.data(PatientListModel::TrendRole).toList();
    if (trend.size() >= 2) {
        double minValue = trend.first().toDouble();
        double maxValue = minValue;
        for (const QVariant& v : trend) {
            minValue = std::min(minValue, v.toDouble());
            maxValue = std::max(maxValue, v.toDouble());
        }
        const double range = std::max(1.0, maxValue - minValue);
        QPainterPath path;
        const double stepX = static_cast<double>(sparkRect.width()) / static_cast<double>(trend.size() - 1);
        for (int i = 0; i < trend.size(); ++i) {
            const double normalised = (trend[i].toDouble() - minValue) / range;
            const double x = sparkRect.left() + i * stepX;
            const double y = sparkRect.bottom() - normalised * sparkRect.height();
            if (i == 0) {
                path.moveTo(x, y);
            } else {
                path.lineTo(x, y);
            }
        }
        painter->setPen(QPen(QColor("#39c0ff"), 2));
        painter->drawPath(path);
    }

    painter->restore();
}

QSize SparklineDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(400, 56);
}

} // namespace qttutorial::medical::telehealth
