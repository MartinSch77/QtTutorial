// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QString>

class QState;
class QStateMachine;

namespace qttutorial::stock_tracker {

// Models one order's lifecycle as a QStateMachine:
//
//   Draft --submit()--> Submitted --fill()--> Filled
//                            |
//                            +--cancel()--> Cancelled
//                            +--reject()--> Rejected
//
// Draft -> Submitted is guarded: submit() is silently rejected (the machine
// stays in Draft) unless the order has a non-empty symbol and a positive
// quantity. Filled/Cancelled/Rejected are terminal: calling cancel() on an
// already-Filled order has no transition to take, so nothing happens - this is
// how "can't cancel an already-filled order" is enforced, the same way an
// invalid signal is simply ignored by a real QStateMachine. reset() returns to
// a fresh Draft from any terminal state so the same object can be reused for a
// new order in the demo UI.
//
// Deliberately has no QML_ELEMENT/QML_UNCREATABLE registration and no
// dependency on Qt6::Qml: this class lives in stock_tracker_lib and must stay
// headless and unit-testable without linking anything QML-related. It is
// still fully usable from QML - StockTrackerBackend::currentOrder() (which
// DOES live in the QML_ELEMENT-registered app layer) exposes a pointer to one,
// and QML binds to its Q_PROPERTYs/invokable slots the normal way any
// QObject-derived C++ instance is accessed from QML.
class OrderLifecycle : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentState READ currentState NOTIFY stateChanged)
    Q_PROPERTY(QString symbol READ symbol WRITE setSymbol NOTIFY symbolChanged)
    Q_PROPERTY(int quantity READ quantity WRITE setQuantity NOTIFY quantityChanged)
    Q_PROPERTY(double limitPrice READ limitPrice WRITE setLimitPrice NOTIFY limitPriceChanged)
    Q_PROPERTY(bool isBuy READ isBuy WRITE setIsBuy NOTIFY isBuyChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY validityChanged)
public:
    explicit OrderLifecycle(QObject* parent = nullptr);

    [[nodiscard]] QString currentState() const;
    [[nodiscard]] QString symbol() const;
    [[nodiscard]] int quantity() const;
    [[nodiscard]] double limitPrice() const;
    [[nodiscard]] bool isBuy() const;
    [[nodiscard]] bool isValid() const;

    void setSymbol(const QString& symbol);
    void setQuantity(int quantity);
    void setLimitPrice(double price);
    void setIsBuy(bool isBuy);

public slots:
    void submit();
    void fill();
    void cancel();
    void reject();
    void reset();

signals:
    void stateChanged(QString stateName);
    void symbolChanged();
    void quantityChanged();
    void limitPriceChanged();
    void isBuyChanged();
    void validityChanged();

    // Internal signals consumed only by the state machine's own transitions.
    void submitRequested();
    void fillRequested();
    void cancelRequested();
    void rejectRequested();
    void resetRequested();

private:
    void buildStateMachine();
    void registerState(QState* state, const QString& name);

    QStateMachine* m_machine;
    QState* m_draftState;
    QState* m_submittedState;
    QState* m_filledState;
    QState* m_cancelledState;
    QState* m_rejectedState;

    QString m_currentStateName;
    QString m_symbol;
    int m_quantity = 0;
    double m_limitPrice = 0.0;
    bool m_isBuy = true;
};

} // namespace qttutorial::stock_tracker
