// SPDX-License-Identifier: MIT
#pragma once

#include <string_view>
#include <vector>

namespace qttutorial::quick_controls_styling {

struct StyleInfo {
    std::string_view name;
    std::string_view minimumQtVersion;
    std::string_view note;
};

// The styles this module's README documents as switchable via -style /
// QT_QUICK_CONTROLS_STYLE / QQuickStyle::setStyle. Kept as real, queryable
// data (rather than only prose) so it can be unit-tested and shown in the UI.
[[nodiscard]] const std::vector<StyleInfo>& styleCatalogue();

} // namespace qttutorial::quick_controls_styling
