// SPDX-License-Identifier: MIT
#include "SubsystemHealthGrid.h"

#include <QGridLayout>
#include <QLabel>

namespace qttutorial::space {

SubsystemHealthGrid::SubsystemHealthGrid(QWidget* parent)
    : QWidget(parent)
{
    new QGridLayout(this);
}

void SubsystemHealthGrid::addSubsystem(const QString& subsystemName)
{
    auto* layout = qobject_cast<QGridLayout*>(this->layout());
    auto* nameLabel = new QLabel(subsystemName, this);
    auto* stateLabel = new QLabel(QStringLiteral("Nominal"), this);
    stateLabel->setAlignment(Qt::AlignCenter);
    stateLabel->setStyleSheet(QStringLiteral("background-color: #2a8a2a; color: white; padding: 4px;"));

    const int row = layout->rowCount();
    layout->addWidget(nameLabel, row, 0);
    layout->addWidget(stateLabel, row, 1);
    m_labels.insert(subsystemName, stateLabel);
}

void SubsystemHealthGrid::onHealthChanged(const QString& subsystemName, const QString& stateName)
{
    QLabel* label = m_labels.value(subsystemName, nullptr);
    if (!label) {
        return;
    }
    label->setText(stateName);
    if (stateName == QStringLiteral("Nominal")) {
        label->setStyleSheet(QStringLiteral("background-color: #2a8a2a; color: white; padding: 4px;"));
    } else if (stateName == QStringLiteral("Caution")) {
        label->setStyleSheet(QStringLiteral("background-color: #b8860b; color: white; padding: 4px;"));
    } else {
        label->setStyleSheet(QStringLiteral("background-color: #a02020; color: white; padding: 4px;"));
    }
}

} // namespace qttutorial::space
