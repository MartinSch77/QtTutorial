// SPDX-License-Identifier: MIT
#pragma once

#include "IndicatorListModel.h"

#include <QObject>
#include <QQmlEngine>
#include <QTimer>

namespace qttutorial::qml_basics {

// The QObject exposed to QML as the "backend": a live system-status source
// driven by a QTimer, demonstrating Q_PROPERTY with NOTIFY and QML_ELEMENT
// registration (the modern, singleton-free way to expose C++ types to QML).
class SystemStatusBackend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(qttutorial::qml_basics::IndicatorListModel* indicators READ indicators CONSTANT)
    Q_PROPERTY(QString overallStatus READ overallStatus NOTIFY overallStatusChanged)
    Q_PROPERTY(int uptimeSeconds READ uptimeSeconds NOTIFY uptimeSecondsChanged)
public:
    explicit SystemStatusBackend(QObject* parent = nullptr);

    [[nodiscard]] IndicatorListModel* indicators() const;
    [[nodiscard]] QString overallStatus() const;
    [[nodiscard]] int uptimeSeconds() const;

signals:
    void overallStatusChanged();
    void uptimeSecondsChanged();

private slots:
    void advance();

private:
    IndicatorListModel* m_indicators;
    QTimer m_timer;
    int m_tick = 0;
    QString m_overallStatus = QStringLiteral("nominal");
};

} // namespace qttutorial::qml_basics
