// SPDX-License-Identifier: MIT
#include "AlertLevel.h"

namespace qttutorial::startrek_bridge {

QString alertLevelName(AlertLevel level)
{
    switch (level) {
    case AlertLevel::Green:
        return QStringLiteral("Green");
    case AlertLevel::Yellow:
        return QStringLiteral("Yellow");
    case AlertLevel::Red:
        return QStringLiteral("Red");
    }
    return QStringLiteral("Green");
}

} // namespace qttutorial::startrek_bridge
