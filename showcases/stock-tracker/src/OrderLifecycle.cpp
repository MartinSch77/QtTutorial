// SPDX-License-Identifier: GPL-3.0-or-later
#include "OrderLifecycle.h"

#include "GuardedTransition.h"

#include <QState>
#include <QStateMachine>

namespace qttutorial::stock_tracker {

OrderLifecycle::OrderLifecycle(QObject* parent)
    : QObject(parent)
    , m_machine(nullptr)
    , m_draftState(nullptr)
    , m_submittedState(nullptr)
    , m_filledState(nullptr)
    , m_cancelledState(nullptr)
    , m_rejectedState(nullptr)
{
    buildStateMachine();
}

QString OrderLifecycle::currentState() const
{
    return m_currentStateName;
}

QString OrderLifecycle::symbol() const
{
    return m_symbol;
}

int OrderLifecycle::quantity() const
{
    return m_quantity;
}

double OrderLifecycle::limitPrice() const
{
    return m_limitPrice;
}

bool OrderLifecycle::isBuy() const
{
    return m_isBuy;
}

bool OrderLifecycle::isValid() const
{
    return !m_symbol.isEmpty() && m_quantity > 0;
}

void OrderLifecycle::setSymbol(const QString& symbol)
{
    if (m_symbol == symbol) {
        return;
    }
    m_symbol = symbol;
    emit symbolChanged();
    emit validityChanged();
}

void OrderLifecycle::setQuantity(int quantity)
{
    if (m_quantity == quantity) {
        return;
    }
    m_quantity = quantity;
    emit quantityChanged();
    emit validityChanged();
}

void OrderLifecycle::setLimitPrice(double price)
{
    if (qFuzzyCompare(m_limitPrice + 1.0, price + 1.0)) {
        return;
    }
    m_limitPrice = price;
    emit limitPriceChanged();
}

void OrderLifecycle::setIsBuy(bool isBuy)
{
    if (m_isBuy == isBuy) {
        return;
    }
    m_isBuy = isBuy;
    emit isBuyChanged();
}

void OrderLifecycle::submit()
{
    emit submitRequested();
}

void OrderLifecycle::fill()
{
    emit fillRequested();
}

void OrderLifecycle::cancel()
{
    emit cancelRequested();
}

void OrderLifecycle::reject()
{
    emit rejectRequested();
}

void OrderLifecycle::reset()
{
    m_symbol.clear();
    m_quantity = 0;
    m_limitPrice = 0.0;
    emit symbolChanged();
    emit quantityChanged();
    emit limitPriceChanged();
    emit validityChanged();
    emit resetRequested();
}

void OrderLifecycle::buildStateMachine()
{
    m_machine = new QStateMachine(this);

    m_draftState = new QState(m_machine);
    m_submittedState = new QState(m_machine);
    m_filledState = new QState(m_machine);
    m_cancelledState = new QState(m_machine);
    m_rejectedState = new QState(m_machine);

    m_machine->setInitialState(m_draftState);

    registerState(m_draftState, QStringLiteral("Draft"));
    registerState(m_submittedState, QStringLiteral("Submitted"));
    registerState(m_filledState, QStringLiteral("Filled"));
    registerState(m_cancelledState, QStringLiteral("Cancelled"));
    registerState(m_rejectedState, QStringLiteral("Rejected"));

    // Draft -> Submitted is the guarded transition: rejected (machine stays in
    // Draft) unless isValid() is true at the moment submit() is called.
    auto* guardedSubmit = new GuardedTransition(
        this, &OrderLifecycle::submitRequested, [this] { return isValid(); }, m_draftState);
    guardedSubmit->setTargetState(m_submittedState);

    m_submittedState->addTransition(this, &OrderLifecycle::fillRequested, m_filledState);
    m_submittedState->addTransition(this, &OrderLifecycle::cancelRequested, m_cancelledState);
    m_submittedState->addTransition(this, &OrderLifecycle::rejectRequested, m_rejectedState);

    // Filled/Cancelled/Rejected have no cancel()/fill()/submit() transitions of
    // their own: emitting those signals while in a terminal state is simply
    // ignored by the state machine, which is exactly how "can't cancel an
    // already-filled order" is enforced without extra guard logic.
    m_filledState->addTransition(this, &OrderLifecycle::resetRequested, m_draftState);
    m_cancelledState->addTransition(this, &OrderLifecycle::resetRequested, m_draftState);
    m_rejectedState->addTransition(this, &OrderLifecycle::resetRequested, m_draftState);

    m_machine->start();
}

void OrderLifecycle::registerState(QState* state, const QString& name)
{
    connect(state, &QState::entered, this, [this, name] {
        m_currentStateName = name;
        emit stateChanged(name);
    });
}

} // namespace qttutorial::stock_tracker
