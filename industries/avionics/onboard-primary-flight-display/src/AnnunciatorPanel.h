// SPDX-License-Identifier: MIT
#pragma once

#include "AnnunciatorLogic.h"

#include <QWidget>

#include <vector>

namespace qttutorial::avionics {

// A row of caution/warning annunciator "lights", the small legend-lit panel
// found on most glass flight decks (style-inspired, not a copy of any specific
// vendor's exact legend layout or colour coding). Each active message is drawn
// as a lit rectangle with a procedurally-drawn caution triangle glyph (a plain
// geometric triangle with an exclamation mark, built from QPainterPath - no
// external icon asset) plus its short legend text. An unlit "master caution"
// summary lamp is filled solid whenever anything is active.
class AnnunciatorPanel : public QWidget {
    Q_OBJECT
public:
    explicit AnnunciatorPanel(QWidget* parent = nullptr);

    void setMessages(std::vector<CautionMessage> messages);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<CautionMessage> m_messages;
};

} // namespace qttutorial::avionics
