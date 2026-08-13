# News Ticker

A polished, animated scrolling news ticker — the kind of "constantly moving
band of headlines" widget you'd see on a broadcast news channel — built to
show off smooth Qt Quick animation and Model/View plumbing, not to teach a
single Qt concept (that's `framework-tour/`) or model a vertical industry
(that's `industries/`).

## What it demonstrates

- **Smooth, continuous scrolling, not a jerky timer jump.** The headline band
  (`qml/Main.qml`) renders the current headline set **twice**, back to back,
  in one `Row`, and animates that row's `x` from `0` to `-halfWidth` with a
  single looping `NumberAnimation`. Because both halves are identical, the
  animation's loop-restart snap from `-halfWidth` back to `0` is invisible —
  the classic seamless-marquee trick — so headlines glide past at a constant
  speed entirely under Qt Quick's scene graph, with no per-tick QML/JS
  position math.
- **Category filtering.** A `TabBar` (World/Tech/Business/Sports/All) drives
  `NewsFeedModel::activeCategory`; changing it re-queries the same
  `HeadlineGenerator::window()` used everywhere else, restricted to the
  selected category.
- **"Breaking news" treatment.** `qml/HeadlineChip.qml` gives any headline
  flagged `breaking` a small red pill with a continuously pulsing opacity
  (`SequentialAnimation on opacity`) plus bold, tinted headline text — a
  distinct, animated visual treatment rather than a static label.
- **C++ backend, QML display only.** `NewsFeedModel` (`QAbstractListModel`,
  `QML_ELEMENT`) is driven by a `QTimer` that periodically asks the pure
  `HeadlineGenerator` (in `news_ticker_lib`, no `QObject`, no timer, no
  randomness) for the next deterministic window of headlines. The generator
  itself is what's unit-tested in
  `tests/showcases/news-ticker/test_headline_generator.cpp` — headline
  rotation order, category filtering, and the breaking flag are all covered
  without touching a timer or the UI.

## Architecture

```
news_ticker_lib (static lib, headless, unit-tested)
  Headline            - {text, category, breaking}
  HeadlineGenerator    - fixed headline pool + pure rotation/filter/window logic

news_ticker (app)
  NewsFeedModel        - QAbstractListModel + QTimer, adapts the generator to QML roles
  qml/Main.qml          - TabBar + seamless marquee band
  qml/HeadlineChip.qml  - one headline, with the "breaking" pulse treatment
```

## Known cosmetic startup warning

On some Qt builds you may see a handful of `ReferenceError: ... is not
defined` lines on stderr right at startup, before the window is fully
realized (the two `Repeater`s in `qml/Main.qml` bind to `NewsFeedModel`'s
roles before the model's very first population signal has propagated through
the whole component tree). It is a one-time, non-repeating console warning,
not a crash and not a steady-state problem: `NewsFeedModel::refresh()`
updates rows in place (`dataChanged`) rather than resetting the model on
every timer tick specifically to avoid this class of warning recurring after
startup.

## Build & run

```sh
cmake -S . -B build
cmake --build build --target news_ticker
./build/showcases/news-ticker/news_ticker
```

## Tests

```sh
cmake --build build --target test_headline_generator
ctest --test-dir build -R test_headline_generator
```
