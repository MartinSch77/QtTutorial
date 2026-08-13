// SPDX-License-Identifier: MIT
#pragma once

#include "AlertLog.h"
#include "AssetHistoryStore.h"
#include "FleetSimulator.h"

#include <QListWidget>
#include <QTableView>
#include <QTimer>
#include <QWidget>

#include <memory>

namespace qttutorial::defence {

class AssetTableModel;
class TacticalMapWidget;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onTick();
    void onAlertRaised(const Alert& alert);
    void refreshAlertList();

    FleetSimulator m_simulator;
    AlertLog m_alertLog;
    std::unique_ptr<AssetHistoryStore> m_history;

    AssetTableModel* m_assetModel;
    QTableView* m_assetView;
    TacticalMapWidget* m_mapWidget;
    QListWidget* m_alertList;
    QTimer* m_timer;
};

} // namespace qttutorial::defence
