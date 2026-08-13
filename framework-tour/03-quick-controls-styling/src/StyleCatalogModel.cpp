// SPDX-License-Identifier: MIT
#include "StyleCatalogModel.h"

#include "StyleCatalog.h"

namespace qttutorial::quick_controls_styling {

StyleCatalogModel::StyleCatalogModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int StyleCatalogModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(styleCatalogue().size());
}

QVariant StyleCatalogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(styleCatalogue().size())) {
        return {};
    }
    const StyleInfo& info = styleCatalogue()[static_cast<std::size_t>(index.row())];
    switch (role) {
    case NameRole:
        return QString::fromUtf8(info.name.data(), static_cast<qsizetype>(info.name.size()));
    case MinimumQtVersionRole:
        return QString::fromUtf8(info.minimumQtVersion.data(),
                                  static_cast<qsizetype>(info.minimumQtVersion.size()));
    case NoteRole:
        return QString::fromUtf8(info.note.data(), static_cast<qsizetype>(info.note.size()));
    default:
        return {};
    }
}

QHash<int, QByteArray> StyleCatalogModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {MinimumQtVersionRole, "minimumQtVersion"},
        {NoteRole, "note"},
    };
}

} // namespace qttutorial::quick_controls_styling
