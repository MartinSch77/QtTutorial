// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::state_machine {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_controller(new DeviceController(this))
    , m_stateLabel(new QLabel(this))
    , m_statusLabel(new QLabel(this))
    , m_historyList(new QListWidget(this))
    , m_interlockCheckBox(new QCheckBox(tr("Interlock cleared"), this))
    , m_openButton(new QPushButton(tr("Open"), this))
    , m_closeButton(new QPushButton(tr("Close"), this))
    , m_errorButton(new QPushButton(tr("Trigger Error"), this))
    , m_resetButton(new QPushButton(tr("Reset"), this))
{
    setWindowTitle(tr("Qt State Machine – Device Controller"));

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    auto stateFont = m_stateLabel->font();
    stateFont.setPointSize(stateFont.pointSize() + 4);
    stateFont.setBold(true);
    m_stateLabel->setFont(stateFont);
    layout->addWidget(m_stateLabel);

    auto* buttonRow = new QHBoxLayout;
    buttonRow->addWidget(m_openButton);
    buttonRow->addWidget(m_closeButton);
    buttonRow->addWidget(m_errorButton);
    buttonRow->addWidget(m_resetButton);
    buttonRow->addWidget(m_interlockCheckBox);
    layout->addLayout(buttonRow);

    layout->addWidget(m_statusLabel);
    layout->addWidget(new QLabel(tr("Transition history:"), central));
    layout->addWidget(m_historyList);

    setCentralWidget(central);

    connect(m_openButton, &QPushButton::clicked, m_controller, &DeviceController::requestOpen);
    connect(m_closeButton, &QPushButton::clicked, m_controller, &DeviceController::requestClose);
    connect(m_errorButton, &QPushButton::clicked, m_controller, &DeviceController::error);
    connect(m_resetButton, &QPushButton::clicked, m_controller, &DeviceController::reset);
    connect(m_interlockCheckBox, &QCheckBox::toggled, m_controller, &DeviceController::setInterlockCleared);

    connect(m_controller, &DeviceController::stateChanged, this, [this](const QString& stateName) {
        m_stateLabel->setText(tr("Current state: %1").arg(stateName));
        m_statusLabel->clear();
        appendLatestHistoryEntry();
    });

    connect(m_controller, &DeviceController::openRequestRejected, this, [this] {
        m_statusLabel->setText(tr("Open request rejected: interlock is not cleared."));
    });

    m_stateLabel->setText(tr("Current state: %1").arg(m_controller->currentState()));

    resize(520, 420);
}

void MainWindow::appendLatestHistoryEntry()
{
    const auto& history = m_controller->transitionHistory();
    if (history.empty()) {
        return;
    }
    const auto& record = history.back();
    const QString from = record.fromState.isEmpty() ? QStringLiteral("(start)") : record.fromState;
    const QString entry = QStringLiteral("%1  %2 -> %3")
                               .arg(record.timestamp.toString(QStringLiteral("HH:mm:ss.zzz")))
                               .arg(from, record.toState);
    m_historyList->addItem(entry);
    m_historyList->scrollToBottom();
}

} // namespace qttutorial::state_machine
