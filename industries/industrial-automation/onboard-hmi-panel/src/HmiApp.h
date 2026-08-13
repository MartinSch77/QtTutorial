// SPDX-License-Identifier: MIT
#pragma once

#include "AlarmLogModel.h"
#include "ProcessSimulator.h"
#include "PumpController.h"

#include <QObject>

namespace qttutorial::industrial_hmi {

// Wires the testable logic classes together for the running application:
// pump faults raise alarms, tank alarms trip the pump into Fault, and the
// pump refuses to start while the tank is already critically full. Exposed
// to QML as context properties by main.cpp.
class HmiApp : public QObject {
    Q_OBJECT
public:
    explicit HmiApp(QObject* parent = nullptr);

    [[nodiscard]] ProcessSimulator* process() { return &m_process; }
    [[nodiscard]] PumpController* pump() { return &m_pump; }
    [[nodiscard]] AlarmLogModel* alarms() { return &m_alarms; }

public slots:
    void togglePump();
    void toggleValve();

private:
    ProcessSimulator m_process;
    PumpController m_pump;
    AlarmLogModel m_alarms;
};

} // namespace qttutorial::industrial_hmi
