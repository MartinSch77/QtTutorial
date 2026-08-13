// SPDX-License-Identifier: MIT
#pragma once

#include "IndicatorGenerator.h"

#include <QAbstractListModel>
#include <QQmlEngine>

namespace qttutorial::qml_basics {

// QAbstractListModel feeding the QML ListView. Data generation itself lives in
// IndicatorGenerator (qml_basics_lib); this class only adapts it to the
// Qt Model/View roles that QML delegates bind against.
class IndicatorListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Created by SystemStatusBackend")
public:
    enum Role { NameRole = Qt::UserRole + 1, ValueRole, StatusRole };

    explicit IndicatorListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void refresh(int tick);

private:
    std::vector<IndicatorReading> m_readings;
};

} // namespace qttutorial::qml_basics
