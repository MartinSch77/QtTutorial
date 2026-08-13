// SPDX-License-Identifier: MIT
#pragma once

#include "LockRegistry.h"
#include "SceneRegistry.h"

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QStringList>

namespace qttutorial::homeautomation {

// QML-facing list model over LockRegistry, plus a human-readable
// "recentActivity" log for the Locks panel. All state and audit-log logic
// lives in LockRegistry, which has no Qt GUI dependency.
class LockListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QStringList recentActivity READ recentActivity NOTIFY recentActivityChanged)
public:
    enum Role {
        NameRole = Qt::UserRole + 1,
        LockedRole,
    };

    explicit LockListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void setLocked(int row, bool locked);
    // Locks every door if the scene calls for it (Away/Night); a no-op for
    // scenes that leave locks alone (Home/Morning). Logged as "Scene: <name>"
    // in recentActivity, same as a manual toggle.
    Q_INVOKABLE void applyScene(int sceneId);

    [[nodiscard]] QStringList recentActivity() const;

signals:
    void recentActivityChanged();

private:
    LockRegistry m_registry;
};

} // namespace qttutorial::homeautomation
