# 07 – State Machine

This step builds a `DeviceController` that models the lifecycle of an actuated
device (think: a valve, or a door actuator) using Qt's State Machine framework
(`Qt6::StateMachine`, `QStateMachine`/`QState`/`QSignalTransition`). The module lives
in [`framework-tour/07-state-machine`](../../framework-tour/07-state-machine) and its
test in
[`tests/framework-tour/07-state-machine`](../../tests/framework-tour/07-state-machine).

By the end of this step you will have:

- A hierarchical state machine with six named states: `Idle`, `Opening`, `Open`,
  `Closing`, `Closed`, `Fault`.
- A guarded transition, implemented as a small reusable class, that only fires when
  a predicate holds.
- A transition history the UI can display.
- A widgets UI to drive and observe all of the above.
- A QTest suite that exercises the controller directly, without any UI.

## 1. The states and the shape of the machine

`DeviceController` (in `src/DeviceController.h`/`.cpp`) owns a `QStateMachine` with
this layout:

```
QStateMachine
├── "operational" QState (ExclusiveStates, initial = Idle)
│   ├── Idle
│   ├── Opening
│   ├── Open
│   ├── Closing
│   └── Closed
└── Fault
```

Grouping the five "normal operation" states under one parent `QState` is what makes
step 3 below (the any-state -> `Fault` transition) simple: Qt's state machine lets
you add a transition on a parent state, and it applies no matter which child state
is currently active. `Fault` itself lives outside that group, as a sibling
top-level state.

The wiring, state by state:

- `Idle --requestOpen()--> Opening` — unconditional. There is nothing to interlock
  against before the device has ever been closed.
- `Closed --requestOpen()--> Opening` — **guarded** (see step 2).
- `Opening --actuationFinished()--> Open` — `actuationFinished()` is an internal
  signal, emitted after a `QTimer::singleShot` fired from `Opening`'s `entered`
  signal. This simulates the time a real actuator takes to move.
- `Open --requestClose()--> Closing`
- `Closing --actuationFinished()--> Closed` — same timer-driven signal as above,
  now consumed by the `Closing` state instead. Reusing a single signal name across
  two different source states is fine: `QStateMachine` only evaluates the
  transitions attached to whichever state is currently active.
- `"operational" --error()--> Fault` — added once on the parent state, so it
  applies from any of `Idle`, `Opening`, `Open`, `Closing`, `Closed`.
- `Fault --reset()--> "operational"` — re-enters the parent's initial child,
  i.e. back to `Idle`.

## 2. The guard: `GuardedSignalTransition`

The `Closed -> Opening` transition must only be taken if an interlock has been
cleared. Qt's `QSignalTransition` doesn't have a built-in notion of "and also check
this condition," but it is designed to be subclassed: it calls a virtual
`eventTest(QEvent*)` to decide whether to fire. `GuardedSignalTransition` (in
`src/GuardedSignalTransition.h`) overrides that hook:

```cpp
class GuardedSignalTransition : public QSignalTransition {
public:
    using Guard = std::function<bool()>;

    template <typename PointerToMemberSignal>
    GuardedSignalTransition(const QObject* sender, PointerToMemberSignal signal, Guard guard,
                             QState* sourceState = nullptr)
        : QSignalTransition(sender, signal, sourceState)
        , m_guard(std::move(guard))
    {
    }

protected:
    bool eventTest(QEvent* event) override
    {
        return QSignalTransition::eventTest(event) && m_guard();
    }

private:
    Guard m_guard;
};
```

`DeviceController` wires it up like this:

```cpp
auto* guardedOpen = new GuardedSignalTransition(
    this, &DeviceController::requestOpenTriggered, [this] { return m_interlockCleared; },
    m_closedState);
guardedOpen->setTargetState(m_openingState);
```

If `m_interlockCleared` is `false` when `requestOpenTriggered()` fires while the
machine is in `Closed`, `eventTest()` returns `false`, no transition happens, and the
machine simply stays in `Closed`. From the state machine's perspective this is a
*silent* rejection — there is no separate "rejected" event built into the framework.

`DeviceController::requestOpen()` additionally emits `openRequestRejected()`
whenever it can already tell — from the very same `m_interlockCleared` flag the
guard itself reads — that the request is about to be silently dropped. This extra
signal exists purely to give the UI (or a test) something to hook into; it does not
duplicate the guard's authority over whether the transition actually happens. That
authority stays entirely inside `GuardedSignalTransition::eventTest()`.

This class and pattern are intentionally generic: it takes a `std::function<bool()>`
guard, so any predicate works. Other examples in this repository — including an
industrial-automation showcase that needs real interlocks — can reuse
`GuardedSignalTransition` exactly as written here.

## 3. Observing the machine: `stateChanged` and history

Every state is registered with:

```cpp
connect(state, &QState::entered, this, [this, name] { recordTransition(name); });
```

`recordTransition()` appends a `{fromState, toState, timestamp}` record to an
internal `std::vector<TransitionRecord>` and emits `stateChanged(QString)`. The UI
uses `transitionHistory()` to render a full log, and `stateChanged` to update the
current-state label live.

## 4. The UI

`MainWindow` (`src/MainWindow.h`/`.cpp`) wires four buttons (Open, Close, Trigger
Error, Reset) and an "Interlock cleared" checkbox directly to the corresponding
`DeviceController` slots, shows the current state in a bold label, and appends every
`transitionHistory()` entry to a `QListWidget` as it arrives via `stateChanged`.
When `openRequestRejected()` fires, a status label explains why nothing happened.

Try it: leave "Interlock cleared" unchecked, get the device to `Closed` (Open, then
Close), and click Open again — the state stays `Closed` and the status label
explains the rejection. Check the box first and the same click now proceeds through
`Opening` into `Open`.

## 5. Testing an event-loop-driven state machine

`QStateMachine` processes transitions asynchronously, through the event loop — never
synchronously with the signal that triggered them. The test suite
(`tests/framework-tour/07-state-machine/test_device_controller.cpp`) accounts for
this by using `QTRY_COMPARE` (which polls with a timeout) instead of asserting on
`currentState()` immediately after calling a slot:

```cpp
controller.requestOpen();
QTRY_COMPARE(controller.currentState(), QStringLiteral("Opening"));
QTRY_COMPARE(controller.currentState(), QStringLiteral("Open"));
```

The suite covers:

- The machine starts in `Idle`.
- `Idle -> Opening -> Open` via `requestOpen()`.
- `error()` from two different states (`Open` and `Opening`) both land in `Fault`.
- `reset()` from `Fault` returns to `Idle`.
- The interlock guard: `requestOpen()` from `Closed` does **not** transition when
  the interlock is not cleared (confirmed by waiting past the actuation delay and
  checking the state is still `Closed`, plus the `openRequestRejected` signal
  firing exactly once), and **does** transition when it is cleared.

## Building and running

This module is wired into the top-level build by the repository's central
`CMakeLists.txt` files, the same way every other `framework-tour` step is. From a
normal build directory:

```sh
cmake --build build --target state_machine
./build/framework-tour/07-state-machine/state_machine

cmake --build build --target test_device_controller
ctest --test-dir build -R test_device_controller
```
