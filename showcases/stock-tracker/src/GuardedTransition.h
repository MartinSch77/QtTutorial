// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QSignalTransition>

#include <functional>
#include <utility>

namespace qttutorial::stock_tracker {

// A QSignalTransition that only fires when both the signal fires AND a supplied
// guard predicate returns true; otherwise the signal is consumed with no
// transition. Same shape as framework-tour/07-state-machine's
// GuardedSignalTransition, reproduced locally here (showcases/ does not depend
// on framework-tour/ targets) to guard OrderLifecycle's Draft -> Submitted
// transition on "is this order valid?".
class GuardedTransition : public QSignalTransition {
public:
    using Guard = std::function<bool()>;

    template <typename Sender, typename PointerToMemberSignal>
    GuardedTransition(const Sender* sender, PointerToMemberSignal memberSignal, Guard guard,
                       QState* sourceState = nullptr)
        : QSignalTransition(sender, memberSignal, sourceState)
        , m_guard(std::move(guard))
    {
    }

protected:
    bool eventTest(QEvent* event) override
    {
        return QSignalTransition::eventTest(event) && m_guard();
    }

private:
    Guard m_guard;
};

} // namespace qttutorial::stock_tracker
