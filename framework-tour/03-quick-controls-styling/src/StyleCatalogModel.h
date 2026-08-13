// SPDX-License-Identifier: MIT
#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

namespace qttutorial::quick_controls_styling {

class StyleCatalogModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
public:
    enum Role { NameRole = Qt::UserRole + 1, MinimumQtVersionRole, NoteRole };

    explicit StyleCatalogModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
};

} // namespace qttutorial::quick_controls_styling
