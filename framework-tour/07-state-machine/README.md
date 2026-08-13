# 07 – State Machine

A device controller (think: a valve or a door actuator) modeled as a hierarchical
`QStateMachine` with six named states:

```
Idle -> Opening -> Open -> Closing -> Closed -> (back to Opening) ...
  ^                                                |
  +---------------------- reset() <---- Fault <----+
```

- `Idle` – the initial state, nothing has been commanded yet.
- `Opening` – the actuator is in motion, moving towards the open position.
- `Open` – fully open.
- `Closing` – the actuator is in motion, moving towards the closed position.
- `Closed` – fully closed.
- `Fault` – reachable from any of the states above; cleared with `reset()`.

`Idle`, `Opening`, `Open`, `Closing` and `Closed` are grouped as children of one
shared parent `QState` ("the operational group"). That is a deliberate choice, not
an accident: it lets the `error() -> Fault` transition be added **once**, on the
parent, instead of being repeated on every child state. `Fault` sits outside that
group as its own top-level state. `reset()` transitions from `Fault` back to the
operational group, which re-enters its initial child, `Idle`.

Actuation is not instantaneous: entering `Opening` or `Closing` starts a
`QTimer::singleShot` that simulates the time a real actuator would take, then
emits an internal `actuationFinished()` signal that drives the machine on into
`Open` or `Closed` respectively.

## The guard pattern

`Closed -> Opening` is not a plain signal transition: it must only happen if an
interlock has been cleared (`DeviceController::setInterlockCleared(bool)`). This is
implemented with a small, reusable transition class,
[`GuardedSignalTransition`](src/GuardedSignalTransition.h):

```cpp
class GuardedSignalTransition : public QSignalTransition {
public:
    using Guard = std::function<bool()>;
    // ... constructor forwards to QSignalTransition's (sender, signal, sourceState) ctor
protected:
    bool eventTest(QEvent* event) override
    {
        return QSignalTransition::eventTest(event) && m_guard();
    }
private:
    Guard m_guard;
};
```

`eventTest()` is the hook `QAbstractTransition` calls to decide whether an event
should trigger the transition. `GuardedSignalTransition` ANDs the base class's own
signal check with an arbitrary `std::function<bool()>` predicate. If the guard
returns `false`, the signal is consumed but no transition happens and the machine
stays in `Closed` — silently, from the state machine's point of view.

`DeviceController::requestOpen()` additionally emits `openRequestRejected()` when it
can tell, from the very same `m_interlockCleared` flag the guard checks, that the
request is about to be rejected. That signal exists purely so the UI has something
to react to; the state machine's own guarded transition remains the single source of
truth for whether the transition actually happens.

**This is meant to be a reusable pattern.** Any other example that needs a
guarded state transition — for instance an industrial-automation showcase with real
interlocks — can point at `GuardedSignalTransition` and reuse it as-is: construct it
with a sender, a signal, a `std::function<bool()>` guard, and a source state, then
`setTargetState()`.

## Transition history

`DeviceController` keeps a `std::vector<TransitionRecord>` (`fromState`, `toState`,
`timestamp`) built by connecting to every state's `QState::entered` signal via
`registerState()`. It emits `stateChanged(QString)` on every entry and exposes the
full history via `transitionHistory()`. The UI uses this to render a log.

## UI

`MainWindow` provides buttons for Open, Close, Trigger Error and Reset, an
"Interlock cleared" checkbox, a label with the current state, and a list widget
showing the transition history with timestamps. Clicking Open while `Closed` and the
interlock is not cleared visibly does nothing to the state (it stays `Closed`) and
shows a status message explaining why.
