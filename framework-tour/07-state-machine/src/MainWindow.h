// SPDX-License-Identifier: MIT
#pragma once

#include "DeviceController.h"

#include <QMainWindow>

class QCheckBox;
class QLabel;
class QListWidget;
class QPushButton;

namespace qttutorial::state_machine {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void appendLatestHistoryEntry();

    DeviceController* m_controller;
    QLabel* m_stateLabel;
    QLabel* m_statusLabel;
    QListWidget* m_historyList;
    QCheckBox* m_interlockCheckBox;
    QPushButton* m_openButton;
    QPushButton* m_closeButton;
    QPushButton* m_errorButton;
    QPushButton* m_resetButton;
};

} // namespace qttutorial::state_machine
