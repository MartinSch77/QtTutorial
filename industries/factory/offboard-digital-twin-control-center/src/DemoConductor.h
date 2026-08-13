// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QTimer>
#include <vector>

namespace qttutorial::offboard_digital_twin {

// Drives the automated "Play demo" button: a QStateMachine that walks
// through all six acceptance-test steps on its own in ~90 seconds -
// (1) explore, (2) overheat + spatial alarm, (3) inspect, (4) analyze,
// (5) maintenance workflow, (6) theme/layout switch - each step announced
// via stepIndexChanged()/narration() so the QML UI can react (fly the
// camera, trigger the overheat, open panels, switch themes) without the C++
// side reaching into QML directly.
class DemoConductor : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(int stepIndex READ stepIndex NOTIFY stepIndexChanged)
    Q_PROPERTY(QString narration READ narration NOTIFY stepIndexChanged)

public:
    explicit DemoConductor(QObject *parent = nullptr);

    [[nodiscard]] bool isPlaying() const { return m_playing; }
    [[nodiscard]] int stepIndex() const { return m_stepIndex; }
    [[nodiscard]] QString narration() const;

public slots:
    void play();
    void stop();

signals:
    void playingChanged();
    void stepIndexChanged();
    // Internal transition trigger only (see the constructor): lets play()
    // restart the walkthrough from the first beat after a previous run
    // reached the final state, without ever calling QStateMachine::stop()/
    // start() back-to-back (QStateMachine::stop() takes effect
    // asynchronously through the event loop, so an immediate start() right
    // after it logs "already running" and is silently ignored).
    void restartRequested();
    // One signal per demo beat; QML connects to these to actually perform
    // the visible action (camera fly-to, overheat trigger, workflow start...).
    void requestExplore();
    void requestOverheat(int machineId);
    void requestInspect(int machineId);
    void requestAnalyze(int machineId);
    void requestMaintenance(int machineId);
    void requestThemeSwitch(const QString &themeName);
    void finished();
    // Internal transition triggers only (see the constructor comment for
    // why the machine is started exactly once, in the constructor, and
    // never stopped/restarted - QStateMachine::stop()/start() take effect
    // asynchronously through the event loop, so calling them back-to-back
    // from play()/stop() is racy).
    void playRequested();
    void stopRequested();

private:
    struct Beat {
        QString narration;
        int durationMs;
    };

    void advance();

    QStateMachine m_machine;
    QTimer m_beatTimer;
    std::vector<Beat> m_beats;
    int m_stepIndex = -1;
    bool m_playing = false;
    int m_targetMachineId = 1; // CNC Mill B2 - the machine that overheats in the demo
};

} // namespace qttutorial::offboard_digital_twin
