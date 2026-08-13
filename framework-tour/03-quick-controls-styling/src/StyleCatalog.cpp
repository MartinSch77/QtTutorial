// SPDX-License-Identifier: MIT
#include "StyleCatalog.h"

namespace qttutorial::quick_controls_styling {

const std::vector<StyleInfo>& styleCatalogue()
{
    static const std::vector<StyleInfo> styles{
        {"Basic", "6.0", "Works everywhere; used by this module's build so it runs on any Qt 6."},
        {"Fusion", "6.0", "Desktop-oriented, cross-platform look."},
        {"Material", "6.0", "Refreshed in later 6.x releases; configurable accent/theme."},
        {"Universal", "6.0", "Windows UWP-style look."},
        {"FluentWinUI3", "6.8", "Native-feeling look on Windows 11."},
        {"macOS", "6.6", "Native look on macOS, via QtQuick.Controls.macOS."},
        {"iOS", "6.7", "Native look on iOS."},
    };
    return styles;
}

} // namespace qttutorial::quick_controls_styling
