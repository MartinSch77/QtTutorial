// SPDX-License-Identifier: MIT
#pragma once

#include <QHash>
#include <QWidget>

class QLabel;

namespace qttutorial::space {

// A grid of colour-coded badges, one per subsystem, updated from
// SubsystemHealthMachine::healthChanged signals.
class SubsystemHealthGrid : public QWidget {
    Q_OBJECT
public:
    explicit SubsystemHealthGrid(QWidget* parent = nullptr);

    void addSubsystem(const QString& subsystemName);

public slots:
    void onHealthChanged(const QString& subsystemName, const QString& stateName);

private:
    QHash<QString, QLabel*> m_labels;
};

} // namespace qttutorial::space
