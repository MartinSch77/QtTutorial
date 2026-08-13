# Tutorial: Qt Quick Controls Styling

This walks through `framework-tour/03-quick-controls-styling/`: a two-tab
settings/dashboard screen built with Qt Quick Controls 2, and how Qt's
control styling system works.

## 1. What "styling" means in Qt Quick Controls

Unlike Qt Widgets, where one native `QStyle` draws every control, Qt Quick
Controls 2 ships several complete, independent QML implementations of the
same control set — Basic, Fusion, Material, Universal, Imagine, plus
platform-native ones (macOS, iOS, and, on recent Qt, FluentWinUI3) — and you
pick one. Switching styles does not change your QML's structure (`Button`,
`Slider`, `ComboBox`, ... keep the same API); it only changes which visual
implementation backs those names.

## 2. Pinning a style at import time

`qml/Main.qml` writes:

```qml
import QtQuick.Controls.Basic
```

instead of the more commonly-seen:

```qml
import QtQuick.Controls
```

The style-agnostic import resolves to *whatever style is active* (chosen via
`-style`, an environment variable, or a config file — see the module's
README). Importing a specific style module, as done here, pins that choice
at compile time. This module deliberately uses Basic because it is the one
style guaranteed to be present and to look/behave identically on every Qt 6
installation, including this repository's Qt 6.4.2 baseline — no theming
engine, native rendering assets, or platform integration required.

## 3. Layout with `SwipeView` + `TabBar`

`SwipeView` holds the pages; each page is an independent `Page` item.
Binding `tabBar.currentIndex` to `swipeView.currentIndex` (and vice versa)
keeps the tab selection and the swipeable page in sync — a two-line pattern
that appears in almost every Qt Quick Controls example that mixes tabs with
swipeable content.

## 4. The controls on display

- **Dashboard page:** `Dial` (fan speed), `Slider` (brightness), `ComboBox`
  (refresh rate), `GroupBox` (visual grouping/labeling), `Button` with
  `highlighted: true` (the primary action).
- **Settings page:** `SwitchDelegate`, a list-friendly on/off row control
  (as opposed to a bare `Switch`, which has no label/row chrome).

## 5. Switching to a different style yourself

1. Change the import in `qml/Main.qml` from `QtQuick.Controls.Basic` back to
   `QtQuick.Controls`.
2. Rebuild.
3. Run with, e.g., `QT_QUICK_CONTROLS_STYLE=Material ./quick_controls_styling`
   or `./quick_controls_styling -style Universal`.

If you have Qt 6.7+ installed, also try `Material` (visually refreshed in
recent releases) or, on Windows 11 with Qt 6.8+, `FluentWinUI3`. Neither is
required to build this module — see the README for the full list and how
each is selected.

## 6. Try it yourself

- Add a `RadioButton` group for a "theme" choice on the Settings page.
- Bind the `Dial`'s `value` to a `Label` on the Dashboard page that changes
  color past 80% (a plain-QML analogue to the `states`/`transitions` pattern
  used in `framework-tour/02-qml-quick-basics/qml/IndicatorTile.qml`).
