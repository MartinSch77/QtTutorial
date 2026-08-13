# Tutorial: Getting Started

This repository is organized as a sequence of small, buildable Qt 6 / C++23
examples. This page covers the parts common to all of them: what you need
installed, how the build is structured, and the conventions every module
follows so that once you've read one, you can navigate the rest quickly.

## 1. Prerequisites

- **Qt 6.4 or later** (the repo's declared baseline — see the root
  `CMakeLists.txt`'s `find_package(Qt6 6.4 REQUIRED COMPONENTS ...)`).
  `framework-tour/09-latest-qt-release-features/` additionally needs the
  **Qt Graphs** module (Qt 6.9+); if CMake can't find it, that one module is
  skipped and everything else still builds.
- **CMake 3.21+**.
- **A C++23 compiler**: GCC 13+, Clang 16+, or MSVC 2022 17.6+.
- **Ninja or Make** (whatever CMake's default generator picks on your
  platform).

## 2. Build everything

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Or build a subset:

```bash
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=OFF   # framework-tour only
cmake -S . -B build -DQTTUTORIAL_BUILD_TESTS=OFF         # skip tests
```

## 3. The convention every module follows

Every `framework-tour/<NN-name>/` module and every
`industries/<industry>/{onboard,offboard}-<name>/` app is structured the
same way — read `framework-tour/01-widgets-basics/` once and the rest will
look familiar:

```
<module>/
  CMakeLists.txt     # find_package + a static "*_lib" target + qttutorial_add_app()
  README.md          # what it demonstrates, which Qt APIs, how to run it
  src/
    <Logic classes>  # plain C++ (or QObject-derived), no UI, goes in *_lib
    MainWindow.*      # or qml/Main.qml — the UI, links against *_lib
    main.cpp
tests/<same path>/
  CMakeLists.txt     # qttutorial_add_test(), links Qt6::Test + the module's *_lib
  test_*.cpp
```

The point of always splitting a `*_lib` static library out from the
application binary is that **the logic worth testing never depends on
`QApplication`/`QGuiApplication` being constructed**, so
`tests/<module>/test_*.cpp` can link straight against it and run headless,
fast, under `ctest`, in CI, without a display.

`cmake/QtTutorialTargets.cmake` defines the two helper functions every
module's `CMakeLists.txt` calls:

- `qttutorial_add_app(<target> SOURCES ... QT_LIBS ... [QML_MODULE_URI ... QML_SOURCES ...])`
  — declares the executable, applies the repo's shared warning flags, and
  (for QML modules) wires up `qt_add_qml_module()`.
- `qttutorial_add_test(<target> SOURCES ... LIB <the module's *_lib> [QT_LIBS ...])`
  — declares a QTest executable and registers it with `ctest`.

## 4. Where to go next

- Work through `docs/tutorial/01` onward in order if you want the framework
  tour as a structured course — each doc builds on concepts from the
  previous one (e.g. `02-qml-quick-basics` assumes you've seen `01`'s
  Model/View discussion).
- Jump straight to `docs/industries/<industry>.md` if you're evaluating Qt
  for a specific domain — each one is self-contained and links back to the
  framework-tour concepts it uses.
- See the top-level `README.md`'s [Quality](../../README.md#quality)
  section and `docs/qa/` for how this repo is tested, analyzed, and traced.
