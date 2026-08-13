// SPDX-License-Identifier: MIT
#pragma once

#include "UnitConverterModel.h"

#include <QMainWindow>

class QTableView;

namespace qttutorial::widgets_basics {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    UnitConverterModel* m_model;
    QTableView* m_view;
};

} // namespace qttutorial::widgets_basics
