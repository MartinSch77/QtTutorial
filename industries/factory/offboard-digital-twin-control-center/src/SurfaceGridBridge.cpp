// SPDX-License-Identifier: GPL-3.0-or-later
#include "SurfaceGridBridge.h"

#include <QVector3D>

namespace qttutorial::offboard_digital_twin {

SurfaceGridBridge::SurfaceGridBridge(QObject *parent)
    : QObject(parent)
    , m_proxy(new QSurfaceDataProxy(this))
{
}

QSurfaceDataProxy *SurfaceGridBridge::proxy() const
{
    return m_proxy;
}

void SurfaceGridBridge::setGrid(const QVariantList &rows)
{
    QSurfaceDataArray array;
    array.reserve(rows.size());
    for (qsizetype r = 0; r < rows.size(); ++r) {
        const QVariantList row = rows.at(r).toList();
        QSurfaceDataRow dataRow;
        dataRow.reserve(row.size());
        for (qsizetype c = 0; c < row.size(); ++c) {
            const float value = row.at(c).toFloat();
            dataRow.append(QSurfaceDataItem(QVector3D(static_cast<float>(c), value, static_cast<float>(r))));
        }
        array.append(dataRow);
    }
    m_proxy->resetArray(array);
}

} // namespace qttutorial::offboard_digital_twin
