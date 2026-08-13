# Stock Tracker — Trading Cockpit

The flagship showcase in `showcases/`: a "trading cockpit" that concretely
demonstrates a wide slice of Qt's competitive capabilities in one app, rather
than teaching a single concept (`framework-tour/`) or modeling a vertical
industry (`industries/`).

## Capability table — what implements what

| Capability | Implementation |
|---|---|
| **Qt Quick/QML UI** | The whole cockpit: `qml/Main.qml` (responsive `GridLayout` shell), `qml/TickerList.qml` (watchlist), `qml/DetailPanel.qml` (selected symbol + chart), `qml/OrderPanel.qml` (order entry + lifecycle badge). |
| **Qt Graphs** | `qml/PriceChartGraphs.qml` — `import QtGraphs`, a `GraphsView` with a `LineSeries` for the selected symbol's simulated price history, zoomable via a `Slider` (`windowSize`) and pannable by dragging across the chart (`panOffset`). Only built/loaded when `Qt6::Graphs` is found; see "Optional modules" below. |
| **Qt Quick Scene Graph / RHI** | Not something this app configures — Qt Quick's GPU-accelerated scene graph is already what renders the marquee-smooth chart animation, the Lottie confirmation and every other pixel here. Qt's RHI abstraction lets that same QML run on Direct3D 11/12, Metal, Vulkan or OpenGL depending on platform, with zero app code changes; which backend is active can be checked with `QSG_INFO=1` or `QSG_RHI_BACKEND=<name>` at run time. |
| **WebSockets / Qt Network** | `src/WebSocketMarketFeed.{h,cpp}` — a real local `QWebSocketServer` bound to `127.0.0.1` plus a `QWebSocket` client that connects to it *in the same process*. `StockTrackerBackend::advanceTick()` calls `publishTick()` on the server side; the client side's `tickReceived()` signal (only fired once the frame has actually round-tripped over the socket) drives every price update. See "WebSocket wiring" below. Guarded exactly like Qt Graphs; falls back to a direct in-process feed when `Qt6::WebSockets` isn't available. |
| **`QStateMachine`** | `src/OrderLifecycle.{h,cpp}` — `Draft -> Submitted -> {Filled, Cancelled, Rejected}`, guarded on `Draft -> Submitted` (`GuardedTransition`, `src/GuardedTransition.h`) and terminal on `Filled`/`Cancelled`/`Rejected`. Visualized live via `qml/OrderStateBadge.qml`. |
| **Responsive layouts** | `qml/Main.qml`'s `GridLayout.columns` is bound to `window.isWide` (`width >= 820`): three side-by-side panels above that width, one stacked column below it, re-evaluated continuously as the window is resized. |
| **SVG/Lottie animation** | Lottie: `qml/OrderPanel.qml` plays `resources/lottie/order-filled.json` (via `import Qt.labs.lottieqt 1.0`, `LottieAnimation`) the moment an order transitions into `Filled`. SVG: `qml/DetailPanel.qml`'s feed-connection indicator renders `resources/icons/feed-status.svg` through `Qt6::Svg`. |
| **C++/QML integration** | `src/PriceSimulator.{h,cpp}` (price generation) and `src/OrderLifecycle.{h,cpp}` (order lifecycle) are headless C++ in `stock_tracker_lib`, with no QML dependency at all. `StockTrackerBackend` and `TickerListModel` (both app-side, `QML_ELEMENT`-registered) are the only types QML imports/instantiates; they expose `OrderLifecycle*`/price data as ordinary bound properties, and QML binds to that `QObject`'s properties/invokable slots the normal way — no QML type registration is required for that, and no price/state logic is duplicated in QML/JS. |

## Architecture

```
stock_tracker_lib (static lib, headless, unit-tested)
  PriceSimulator        - pure random-walk price generator, one instance per symbol
  GuardedTransition     - QSignalTransition + predicate guard (same shape as
                          framework-tour/07-state-machine's GuardedSignalTransition,
                          reproduced locally: showcases/ doesn't depend on
                          framework-tour/ targets)
  OrderLifecycle        - QStateMachine-based order state machine (headless, no QML dependency)
  WebSocketMarketFeed   - QWebSocketServer + QWebSocket loopback pair
                          (only compiled when Qt6::WebSockets is found)

stock_tracker (app)
  TickerListModel        - QAbstractListModel adapting PriceSimulator state to the watchlist
  StockTrackerBackend    - QML_ELEMENT hub: owns every PriceSimulator, the
                           TickerListModel, the one OrderLifecycle, and (optionally)
                           the WebSocketMarketFeed
  qml/Main.qml            - responsive GridLayout shell
  qml/TickerList.qml       - watchlist
  qml/DetailPanel.qml      - selected symbol, price, chart Loader, feed status
  qml/PriceChartGraphs.qml  - Qt Graphs LineSeries chart (optional)
  qml/PriceChartFallback.qml - QML Shapes line chart (always available)
  qml/OrderPanel.qml        - order entry + OrderStateBadge + Lottie confirmation
  qml/OrderStateBadge.qml   - stepper/badge visualizing OrderLifecycle.currentState
```

## WebSocket wiring

`WebSocketMarketFeed::start()` calls `QWebSocketServer::listen(QHostAddress::LocalHost)`
on an ephemeral port, then immediately opens a `QWebSocket` client connection to
`ws://127.0.0.1:<that port>`. `StockTrackerBackend` calls `publishTick(symbol,
price, sequence)` on the feed every tick, which sends a small JSON text frame
from the **server**'s accepted socket (`QWebSocketServer::nextPendingConnection()`).
The **client** socket's `textMessageReceived` signal decodes that frame and
re-emits it as `tickReceived(...)`, which is what `StockTrackerBackend`
actually listens to when updating prices. This is a genuine WebSocket
round-trip over a real (loopback) socket, not a timer relabeled as
"streaming" — you could point a separate WebSocket client at
`ws://127.0.0.1:<serverPort()>` and see the same ticks.

## Optional modules and graceful degradation

This repository's baseline local Qt (6.4.2) has neither Qt Graphs (needs
>= 6.9) nor Qt WebSockets (needs >= 6.5) installed; CI builds against Qt
6.11.1, which has both. `showcases/stock-tracker/CMakeLists.txt` guards each
one individually, the same way
`framework-tour/09-latest-qt-release-features/CMakeLists.txt` guards Qt
Graphs — `find_package(Qt6 COMPONENTS <Module> QUIET)` then
`if(TARGET Qt6::<Module>)` — except here only the one dependent piece is
skipped, not the whole module, so `stock_tracker` still builds and runs on
Qt 6.4.2:

- **Without Qt6::WebSockets**: `WebSocketMarketFeed.{h,cpp}` is excluded from
  `stock_tracker_lib` entirely (no `#ifdef` inside the file needed - it's
  simply never compiled), `STOCK_TRACKER_HAVE_WEBSOCKETS` is not defined, and
  `StockTrackerBackend` applies every simulated tick directly instead of
  round-tripping it over a socket. `StockTrackerBackend::feedMode()` reports
  this honestly in the UI (bottom of the detail panel) instead of pretending
  a stream exists.
- **Without Qt6::Graphs**: `qml/PriceChartGraphs.qml` is excluded from the
  QML module's sources entirely (so an old Qt with no `QtGraphs` QML module
  never even sees the `import QtGraphs` line), `STOCK_TRACKER_HAVE_GRAPHS` is
  not defined, `StockTrackerBackend::graphsAvailable()` returns `false`, and
  `qml/DetailPanel.qml`'s `Loader` always resolves to
  `qml/PriceChartFallback.qml` — a plain `QtQuick.Shapes`-based line chart
  driven by the exact same `selectedPriceHistory` data Qt Graphs would have
  used, so the stock tracker always shows *a* chart, just a less fancy one.

## Lottie and SVG assets

- `resources/lottie/order-filled.json` — a small hand-written Bodymovin/Lottie
  file (a filling green circle followed by a stroked checkmark, both plain
  Shape layers, which is all `Qt.labs.lottieqt`'s `LottieAnimation` supports)
  played once when an order reaches `Filled`.
- `resources/icons/feed-status.svg` — the feed-connection indicator in the
  detail panel, rendered via `Qt6::Svg`.

Both are bundled into the app's Qt resource system via
`resources/assets.qrc` + `qt6_add_resources()` in
`showcases/stock-tracker/CMakeLists.txt`.

The exact Lottie import, verified against the Qt 6.11 documentation before
use (`qt_documentation_read` on `qml-qt-labs-lottieqt-lottieanimation.html`):

```qml
import Qt.labs.lottieqt 1.0
```

## Build & run

```sh
cmake -S . -B build
cmake --build build --target stock_tracker
./build/showcases/stock-tracker/stock_tracker
```

On Qt 6.4.2 you'll see, in the CMake configure output, that both optional
modules are reported not found and the module still configures successfully:

```
-- showcases/stock-tracker: Qt6::WebSockets not found (needs Qt >= 6.5) - falling back to a direct in-process feed, ...
-- showcases/stock-tracker: Qt6::Graphs not found (needs Qt >= 6.9) - using the QML Shapes fallback chart ...
```

## Tests

```sh
cmake --build build --target test_price_simulator test_order_lifecycle
ctest --test-dir build -R "test_price_simulator|test_order_lifecycle"
```

- `test_price_simulator.cpp` — bounds (every generated price stays within
  `[minPrice, maxPrice]` over thousands of steps), determinism (same seed ->
  identical sequence), and that different seeds diverge.
- `test_order_lifecycle.cpp` — guarded `Draft -> Submitted` (rejected with an
  empty symbol or zero quantity, accepted once both are set), the three
  `Submitted ->` outcomes, and specifically that `cancel()` on an
  already-`Filled` order is a no-op (`currentState()` stays `Filled`, no
  `stateChanged` signal fires). Every test waits for the state machine's
  asynchronous `start()` to actually enter `Draft` with `QTRY_COMPARE` before
  sending any signal — `QStateMachine::start()` enters its initial state
  through the event loop, not synchronously at construction, and sending a
  signal before that entry gets silently dropped.
