# 05 – Concurrency & Async

Module: `framework-tour/05-concurrency-async/`
Test: `tests/framework-tour/05-concurrency-async/`

This step is about running work off the GUI thread two different ways over the
same data, and being precise about *why* you'd pick one over the other. It
builds a small "sensor dashboard": a simulated stream of per-channel samples,
two buttons that each process that stream through a different concurrency
model, a progress bar and a samples/sec readout for each, and a cancel button
that demonstrates cooperative cancellation.

## 1. The simulated data

`SensorData.h`/`.cpp` defines:

```cpp
struct SensorSample {
    qint64 timestampMs = 0;
    int channel = 0;
    double value = 0.0;
};

std::vector<SensorSample> generateSamples(unsigned seed, std::size_t sampleCount, int channelCount);
```

`generateSamples` is a pure function: a fixed `seed` combined with a fixed
`sampleCount`/`channelCount` always produces byte-for-byte identical output
(it seeds a `std::mt19937` explicitly rather than reading global/system
entropy). That determinism is what makes the two pipelines directly
comparable and what makes the unit test able to assert exact equality instead
of "looks plausible."

## 2. The shared statistics logic

`StatsCompute.h`/`.cpp` holds the one piece of math both pipelines rely on:

```cpp
class ChannelStatsAccumulator {
public:
    void add(double value);
    ChannelStats finish(int channel) const;
    // running count/min/max/sum/sum-of-squares
};

ChannelStats computeChannelStats(const std::vector<SensorSample>& samples, int channel);
std::vector<ChannelStats> computeAllChannelStats(const std::vector<SensorSample>& samples, int channelCount);
```

`computeChannelStats` filters the sample vector down to one channel with
`std::ranges::views::filter`/`transform` and folds the values into an
accumulator. Because each channel's accumulator is independent local state,
calling `computeChannelStats` concurrently for different channels — from
different threads — needs no locking at all. That property is exactly what
the QtConcurrent pipeline exploits.

## 3. Pipeline A: QtConcurrent

`QtConcurrentPipeline` is a `QObject` wrapping a `QFutureWatcher<ChannelStats>`:

```cpp
QFuture<ChannelStats> future = QtConcurrent::mapped(channels, [samplesShared](int channel) {
    return computeChannelStats(*samplesShared, channel);
});
m_watcher.setFuture(future);
```

`QtConcurrent::mapped` schedules one `computeChannelStats` call per channel on
Qt's global thread pool. The `QFutureWatcher` is connected to
`progressValueChanged` and `finished` with ordinary `connect()` calls — no
threading code, no signal marshalling to write by hand, because the watcher
object lives on the GUI thread and Qt already guarantees its signals are
delivered there.

This is the "Qt-native" way to parallelize a map-reduce-shaped computation: you
describe *what* to compute per item, and the thread pool, scheduling, and
GUI-thread signal delivery are Qt's problem, not yours.

## 4. Pipeline B: std::jthread + std::stop_token

`JThreadWorker` runs the same kind of computation, but sample-by-sample in
batches on a raw `std::jthread`, checking a `std::stop_token` between batches:

```cpp
m_thread = std::jthread([this, samples = std::move(samples), channelCount](std::stop_token token) {
    run(token, std::move(samples), channelCount);
});
```

Inside `run()`, each batch of samples is filtered with `std::ranges::views`
and folded into per-channel `ChannelStatsAccumulator`s, a status string is
built with `std::format`, and — critically — the loop checks
`token.stop_requested()` before every batch:

```cpp
for (std::size_t offset = 0; offset < samples.size(); offset += batchSize) {
    if (token.stop_requested()) {
        cancelled = true;
        break;
    }
    // ... accumulate this batch, emit progress ...
}
```

Calling `worker.requestStop()` (wired to the "Cancel" button) sets the
`stop_token`'s associated state; the very next batch boundary sees it and the
thread unwinds cleanly, emitting `stopped()` instead of `finished()`. That's
cooperative cancellation: nothing is forcibly killed, the thread notices and
exits on its own.

### Crossing back into Qt safely

`JThreadWorker` is a `QObject`, but it is never `moveToThread`'d — only its
`run()` *function* executes on the worker thread; the object itself keeps the
GUI thread as its thread affinity. Emitting its signals (`progressChanged`,
`statusChanged`, `finished`, `stopped`) from inside `run()` is still safe,
because `Qt::AutoConnection` looks at the *receiver's* thread when a signal is
emitted and automatically queues delivery if it differs from the calling
thread — it does not matter which thread called `emit`. The only extra step
needed is registering the non-Qt result type for queued delivery:

```cpp
Q_DECLARE_METATYPE(qttutorial::concurrency_async::ChannelStats)
Q_DECLARE_METATYPE(std::vector<qttutorial::concurrency_async::ChannelStats>)
```

Without that, Qt has no way to copy a `std::vector<ChannelStats>` into the
queued event and would drop the signal with a runtime warning. Nothing on the
worker thread ever calls into a `QWidget` or reads/writes another `QObject`'s
state directly — only signal emission crosses the thread boundary, which is
the one operation Qt makes thread-safe by construction.

## 5. The UI

`MainWindow` wires two buttons ("Run via QtConcurrent", "Run via
std::jthread"), two progress bars, two result labels, a status label for the
jthread run, and a "Cancel std::jthread run" button that calls
`m_jthreadWorker->requestStop()`. Both pipelines run over the same
`m_samples` vector generated once at startup, so their reported elapsed
time/throughput (measured with `QElapsedTimer`, displayed as samples/sec) are
directly comparable.

## 6. Testing the pure logic

`tests/framework-tour/05-concurrency-async/test_concurrency_async.cpp` checks,
without touching the UI at all:

- `generateSamples` is deterministic for a fixed seed and differs across
  seeds.
- `computeChannelStats` matches hand-computed min/max/mean/stddev on a known
  input, and filters by channel correctly.
- `computeAllChannelStats` covers every channel and accounts for every sample.
- The `QtConcurrentPipeline` end to end, via `QSignalSpy::wait()` on its
  `finished` signal, produces the same results as the sequential reference
  computation.
- The `JThreadWorker` can be started, is observed running via
  `QTRY_VERIFY(worker.isRunning())`, and after `requestStop()` stops promptly
  (`QTRY_VERIFY_WITH_TIMEOUT(!worker.isRunning(), 3000)`), emitting `stopped()`
  and never `finished()` — proving the cancellation is both real (it
  interrupts a run that would otherwise take much longer) and prompt (it
  reacts within one batch, not "eventually").

## Takeaway

Both pipelines end up in the same place — a `std::vector<ChannelStats>` — but
getting there looks very different: QtConcurrent trades control for
convenience and Qt-event-loop integration; `std::jthread`/`std::stop_token`
trades a bit of manual thread-safety bookkeeping for precise, cooperative
control over when work starts, reports progress, and stops. Knowing both
means picking the right one instead of reaching for whichever you learned
first.
