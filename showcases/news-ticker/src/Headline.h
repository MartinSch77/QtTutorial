// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::news_ticker {

struct Headline {
    QString text;
    QString category;
    bool breaking = false;
};

} // namespace qttutorial::news_ticker
