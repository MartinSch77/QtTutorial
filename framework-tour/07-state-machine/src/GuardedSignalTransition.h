// SPDX-License-Identifier: MIT
#pragma once

#include <QSignalTransition>

#include <functional>
#include <utility>

namespace qttutorial::state_machine {

// A QSignalTransition that is only taken when both the signal fires AND a supplied
// predicate ("guard") returns true. If the guard returns false, the signal is
// consumed but no transition happens and the state machine stays put.
//
// This is the reusable "guard concept" other examples (e.g. an industrial-automation
// showcase with interlocks) can point to: subclass QSignalTransition, override
// eventTest() to AND the base class's signal check with the guard.
class GuardedSignalTransition : public QSignalTransition {
public:
    using Guard = std::function<bool()>;

    template <typename Sender, typename PointerToMemberSignal>
    GuardedSignalTransition(const Sender* sender, PointerToMemberSignal memberSignal, Guard guard,
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

} // namespace qttutorial::state_machine
