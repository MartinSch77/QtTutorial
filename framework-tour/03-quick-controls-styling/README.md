# 03 – Qt Quick Controls Styling

A realistic two-tab settings/dashboard screen — a "Dashboard" page (fan-speed
`Dial`, brightness `Slider`, refresh-rate `ComboBox`, an "Apply" `Button`) and
a "Settings" page (`SwitchDelegate` toggles) — built entirely with the
**Basic** style so it compiles and runs unmodified on every Qt 6 installation,
including the Qt 6.4.2 baseline this repository targets.

`qml/Main.qml` imports `QtQuick.Controls.Basic` directly instead of the
style-agnostic `QtQuick.Controls`, which pins the style at compile time
regardless of environment/platform defaults — the simplest way to guarantee
a consistent look for a tutorial screenshot or CI run.

## Controls demonstrated

`TabBar` + `SwipeView` (page navigation), `Dial`, `Slider`, `ComboBox`,
`GroupBox`, `Button`, `SwitchDelegate`, `Label`, `Page`.

## Build & run

```sh
cmake -S . -B build
cmake --build build --target quick_controls_styling
./build/framework-tour/03-quick-controls-styling/quick_controls_styling
```

## Switching styles

Qt Quick Controls supports several interchangeable styles; which ones are
*available* depends on the Qt version and platform, but which one is *active*
can always be chosen without touching the QML that uses `QtQuick.Controls`
(style-agnostic) imports — this module intentionally imports
`QtQuick.Controls.Basic` directly instead, so switching styles requires
changing that one import line back to `QtQuick.Controls`. Once you do that,
you can pick a style in any of the following ways, in order of precedence:

1. **Command line**, at run time: `./quick_controls_styling -style Material`
2. **Environment variable**, at run time:
   `QT_QUICK_CONTROLS_STYLE=Material ./quick_controls_styling`
3. **In code**, before the `QQmlApplicationEngine` is constructed:
   ```cpp
   #include <QQuickStyle>
   QQuickStyle::setStyle("Material");
   ```
4. **As a packaged default**, via a `qtquickcontrols2.conf` file placed next
   to the executable (or pointed at with the `QT_QUICK_CONTROLS_CONF`
   environment variable), which can set the preferred style once for a
   deployed build without touching the QML or recompiling — see
   "Qt Quick Controls Configuration File" in the Qt docs.

Built into Qt 6.4.2 (this repository's baseline) are the **Basic**,
**Fusion**, **Material**, **Universal**, **Imagine**, and (on macOS/iOS) the
**macOS**/**iOS** native styles. If you build against a newer Qt on your own
machine, later releases add more current options worth trying:

- **Material** has received visual refinements across Qt 6.6–6.9 that bring
  it closer to Material 3 (updated elevation, color roles and ripple/state
  layers) without changing the import name — just `import QtQuick.Controls.Material`.
- **FluentWinUI3** (Qt 6.8+) is a native-looking style for Windows 11,
  drawing on Fluent/WinUI3 visuals and the system theme/accent color; only
  meaningful on Windows.
- The native **macOS** and **iOS** styles have continued to track newer
  platform control appearances on recent Qt releases.

None of the above are required to build or run this module — it only needs
the Basic style, which ships with every Qt 6 installation.
