// SPDX-License-Identifier: MIT
#pragma once

#include <QWidget>

namespace qttutorial::space {

// A strip of hand-painted vector icons (see IconPainter) giving the kiosk
// panel the visual grammar of a real ops display: satellite, orbit, ground
// station, battery/charge state and an optional warning triangle. Purely
// decorative/status - all data still comes from the labels/gauges elsewhere
// in MainWindow; this widget just renders the icon set against the current
// charging/contact/warning state passed in via setState().
class StatusIconStrip : public QWidget {
    Q_OBJECT
public:
    explicit StatusIconStrip(QWidget* parent = nullptr);

    void setState(bool charging, bool groundStationInView, bool warning);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_charging = false;
    bool m_groundStationInView = false;
    bool m_warning = false;
};

} // namespace qttutorial::space
