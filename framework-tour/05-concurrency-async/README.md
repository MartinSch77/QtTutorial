# 05 – Concurrency & Async: QtConcurrent vs. std::jthread

This module runs the *same* per-channel statistics computation over a simulated
stream of sensor samples through two deliberately contrasted concurrency models,
side by side in one window:

1. **QtConcurrent** (`QtConcurrent::mapped` + `QFutureWatcher<ChannelStats>`) —
   `QtConcurrentPipeline`.
2. **std::jthread + std::stop_token** — `JThreadWorker`.

Both pipelines consume `std::vector<SensorSample>` produced by
`generateSamples()` and both reduce to the same `ChannelStats` result type, so
the two runs are comparable and the underlying math (`ChannelStatsAccumulator`,
`computeChannelStats`) is shared and unit-tested once.

## Why QtConcurrent

- You never touch a `QThread` or a raw thread directly. `QtConcurrent::mapped`
  submits work to Qt's global thread pool and hands back a `QFuture`.
- `QFutureWatcher` turns that future into Qt signals — `progressValueChanged`,
  `finished`, `resultReadyAt` — that you connect like any other signal. The
  watcher lives on the GUI thread, so its signals are delivered safely without
  any extra plumbing.
- This is the natural choice when the work is a finite, "run once, get a
  result" computation that a Widgets or QML UI wants to kick off and observe,
  and when you don't need fine-grained, cooperative cancellation mid-computation
  (QFuture::cancel() only takes effect at iteration boundaries QtConcurrent
  controls, not at arbitrary points you choose).
- Trade-off: less control over exactly how work is chunked, scheduled, or
  interrupted, and it pulls in the Qt Concurrent module.

## Why std::jthread + std::stop_token

- `std::jthread` is a plain C++23 standard-library thread: no Qt dependency at
  all in the worker logic itself, no thread pool, exactly the thread you
  asked for, joined automatically in the destructor.
- `std::stop_token` gives you *cooperative* cancellation you check exactly
  where you want to — every batch, in this module — so a "Cancel" button can
  stop a long-running scan promptly, not just at the end.
- `std::ranges` views (`filter`/`transform`) and `std::format` do the
  data-shaping and status-text formatting without pulling in any Qt string or
  container types on the worker side.
- Trade-off: you own the thread-safety of crossing back into Qt. Here,
  `JThreadWorker` is a `QObject` that is *never* moved to another thread
  (`moveToThread` is not called on it) — only its `run()` member function
  executes on the worker thread. Emitting its signals from that thread is
  still safe: Qt's default `Qt::AutoConnection` resolves to
  `Qt::QueuedConnection` because the connecting code compares the *receiver's*
  thread affinity against the thread the signal is emitted from, not the
  emitter object's affinity. The queued event copies the signal arguments, so
  `std::vector<ChannelStats>` and `ChannelStats` are registered with
  `Q_DECLARE_METATYPE` in `SensorData.h` — without that, Qt would print a
  runtime warning and silently drop the cross-thread signal. Nothing on the
  worker thread ever calls a method on a widget or touches Qt containers
  directly; only signal emission crosses the thread boundary.

## When to reach for which

- Reach for **QtConcurrent** when the work is naturally expressed as "map/filter
  over a Qt container/range and reduce", when you want Qt's thread pool to
  manage scheduling for you, and when the UI just needs to react to
  "progress" and "done".
- Reach for **std::jthread/stop_token** when you need a dedicated, long-lived
  worker with fine-grained, responsive cancellation, when you want the core
  algorithm to have zero Qt dependency (e.g. it might be reused in a non-Qt
  context), or when you're already reaching for other modern-C++ tools
  (`std::ranges`, `std::format`) and don't want a Qt-flavoured API in the
  middle of that code.

## Toolchain note

Both `std::jthread`/`std::stop_token` and `std::format` compiled cleanly with
the locally installed GCC 13.3 / libstdc++ (C++23 mode), so no fallback to
`QString::arg`/`asprintf`-style formatting was needed. `std::format` is used
only for the jthread worker's human-readable status string; everything else
that flows into Qt widgets goes through `QString`.
