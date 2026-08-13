// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::avionics {

struct EngineParameters {
    double egtMarginC = 40.0;    // margin below redline, lower is worse
    double oilPressurePsi = 65.0;
    double vibrationIps = 0.15;  // inches/second
};

struct Aircraft {
    QString tailNumber;
    QString type;
    EngineParameters engine1;
    EngineParameters engine2;
    bool maintenanceFlag = false;
};

struct MaintenanceTask {
    QString id;
    QString aircraftTail;
    QString description;
    QString priority; // "Routine", "Elevated", "Urgent"
    bool completed = false;
};

} // namespace qttutorial::avionics
