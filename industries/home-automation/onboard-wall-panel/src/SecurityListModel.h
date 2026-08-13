// SPDX-License-Identifier: MIT
#pragma once

#include "SecurityCenter.h"

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QTimer>

namespace qttutorial::homeautomation {

// QML-facing list model over SecurityCenter's sensors, plus armed/breach
// properties for the Security panel. Also owns a QTimer that cycles a
// simulated sensor "blip" (a door/window opening briefly, motion detected)
// through the sensor list so the panel has something live to show - the
// armed/breach decision itself lives in SecurityCenter, which has no Qt GUI
// dependency and is unit tested independent of this class.
class SecurityListModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool armed READ armed WRITE setArmed NOTIFY armedChanged)
    Q_PROPERTY(bool breach READ breach NOTIFY breachChanged)
public:
    enum Role {
        NameRole = Qt::UserRole + 1,
        TriggeredRole,
    };

    explicit SecurityListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] bool armed() const { return m_center.armed(); }
    void setArmed(bool armed);
    [[nodiscard]] bool breach() const { return m_center.isBreach(); }

signals:
    void armedChanged();
    void breachChanged();

private:
    void simulateSensorBlip();

    SecurityCenter m_center;
    QTimer m_timer;
    int m_tick = 0;
    int m_activeSensor = -1;
};

} // namespace qttutorial::homeautomation
