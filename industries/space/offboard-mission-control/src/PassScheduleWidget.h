// SPDX-License-Identifier: MIT
#pragma once

#include "GroundStationTracker.h"

#include <QWidget>

#include <vector>

namespace qttutorial::space {

// A textual "next contact window" schedule, one row per fleet satellite:
// currently in contact, or the station and time-to-contact returned by
// GroundStationTracker::nextContact(). Hand-painted (QPainter), in keeping
// with the rest of this app's no-Charts/no-Graphs house style.
class PassScheduleWidget : public QWidget {
    Q_OBJECT
public:
    explicit PassScheduleWidget(QWidget* parent = nullptr);

    void setContactWindows(std::vector<ContactWindow> windows);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<ContactWindow> m_windows;
};

} // namespace qttutorial::space
