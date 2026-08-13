// SPDX-License-Identifier: GPL-3.0-or-later
#include "DemoConductor.h"

#include <QCoreApplication>

namespace qttutorial::offboard_digital_twin {

DemoConductor::DemoConductor(QObject *parent)
    : QObject(parent)
    , m_beats{
          {QStringLiteral("1/6 Explore the factory floor - orbit, zoom, select a machine."), 15000},
          {QStringLiteral("2/6 CNC Mill B2 is overheating - watch the material glow and listen for the spatial alarm."), 15000},
          {QStringLiteral("3/6 Inspecting CNC Mill B2 - exploded housing view, defective bearing highlighted."), 15000},
          {QStringLiteral("4/6 Analyzing telemetry - vibration waveform, thermal history, temperature surface."), 15000},
          {QStringLiteral("5/6 Running the maintenance workflow - diagnose, stop, replace, restart."), 20000},
          {QStringLiteral("6/6 Switching appearance - dark, light and high-contrast themes."), 10000},
      }
{
    // QStateMachine::start()/stop() only take effect asynchronously through
    // the event loop, so calling them back-to-back (e.g. stop() immediately
    // followed by play()) is racy - a start() issued before a preceding
    // stop() has actually been processed is silently ignored. To sidestep
    // that entirely, the machine is started exactly once, here in the
    // constructor, and never stopped again: there is a dedicated idleState
    // (the true initial state) that every beat can return to via the
    // stopRequested signal, and play() always re-enters beat 0 via the
    // playRequested signal rather than by restarting the machine itself.
    auto *idleState = new QState(&m_machine);
    std::vector<QState *> beatStates;
    for (std::size_t i = 0; i < m_beats.size(); ++i)
        beatStates.push_back(new QState(&m_machine));
    idleState->addTransition(this, &DemoConductor::playRequested, beatStates.front());
    for (std::size_t i = 0; i + 1 < beatStates.size(); ++i)
        beatStates[i]->addTransition(&m_beatTimer, &QTimer::timeout, beatStates[i + 1]);
    beatStates.back()->addTransition(&m_beatTimer, &QTimer::timeout, idleState);
    for (QState *beat : beatStates)
        beat->addTransition(this, &DemoConductor::stopRequested, idleState);

    for (std::size_t i = 0; i < beatStates.size(); ++i) {
        const int index = static_cast<int>(i);
        connect(beatStates[i], &QState::entered, this, [this, index]() {
            m_stepIndex = index;
            m_beatTimer.start(m_beats[static_cast<std::size_t>(index)].durationMs);
            emit stepIndexChanged();

            switch (index) {
            case 0: emit requestExplore(); break;
            case 1: emit requestOverheat(m_targetMachineId); break;
            case 2: emit requestInspect(m_targetMachineId); break;
            case 3: emit requestAnalyze(m_targetMachineId); break;
            case 4: emit requestMaintenance(m_targetMachineId); break;
            case 5: emit requestThemeSwitch(QStringLiteral("high-contrast")); break;
            default: break;
            }
        });
    }
    connect(idleState, &QState::entered, this, [this]() {
        m_beatTimer.stop();
        if (!m_playing) // the machine's *very first* entry into idleState, at construction time - not a real finish
            return;
        m_playing = false;
        m_stepIndex = -1;
        emit playingChanged();
        emit stepIndexChanged();
        emit finished();
    });

    m_machine.setInitialState(idleState);
    m_machine.start();
    // QStateMachine::start() only enters its initial state asynchronously,
    // through the event loop (see showcases/stock-tracker's
    // OrderLifecycle/test_order_lifecycle.cpp for the same caveat on a
    // simpler machine that doesn't process this internally). Processing
    // events once here means idleState is guaranteed active by the time the
    // constructor returns, so callers - unlike OrderLifecycle's callers -
    // never need their own "wait for the initial state" step before calling
    // play().
    QCoreApplication::processEvents();
}

QString DemoConductor::narration() const
{
    if (m_stepIndex < 0 || static_cast<std::size_t>(m_stepIndex) >= m_beats.size())
        return QStringLiteral("Idle - press Play demo to start the walkthrough.");
    return m_beats[static_cast<std::size_t>(m_stepIndex)].narration;
}

void DemoConductor::play()
{
    if (m_playing)
        return;
    m_playing = true;
    emit playingChanged();
    emit playRequested();
}

void DemoConductor::stop()
{
    if (!m_playing)
        return;
    m_playing = false;
    m_stepIndex = -1;
    m_beatTimer.stop();
    emit playingChanged();
    emit stepIndexChanged();
    emit stopRequested(); // drives the underlying QStateMachine back to idleState; see the constructor comment
}

} // namespace qttutorial::offboard_digital_twin
