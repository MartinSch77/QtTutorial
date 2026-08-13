// SPDX-License-Identifier: MIT
#include <MoveValidator.h>

#include <QTest>

using namespace qttutorial::maumau;

class TestMoveValidator : public QObject {
    Q_OBJECT
private slots:
    void matchingSuitIsLegal();
    void matchingRankIsLegal();
    void mismatchedCardIsIllegal();
    void jackIsAlwaysLegal();
    void wishedSuitOverridesTopCard();
    void wishedSuitStillAllowsAnotherJack();
    void effectsAreMappedCorrectly();
};

void TestMoveValidator::matchingSuitIsLegal()
{
    const DiscardState state{Card{Suit::Hearts, Rank::King}, std::nullopt};
    QVERIFY(isLegalPlay(Card{Suit::Hearts, Rank::Nine}, state));
}

void TestMoveValidator::matchingRankIsLegal()
{
    const DiscardState state{Card{Suit::Hearts, Rank::King}, std::nullopt};
    QVERIFY(isLegalPlay(Card{Suit::Clubs, Rank::King}, state));
}

void TestMoveValidator::mismatchedCardIsIllegal()
{
    const DiscardState state{Card{Suit::Hearts, Rank::King}, std::nullopt};
    QVERIFY(!isLegalPlay(Card{Suit::Clubs, Rank::Nine}, state));
}

void TestMoveValidator::jackIsAlwaysLegal()
{
    const DiscardState state{Card{Suit::Hearts, Rank::King}, std::nullopt};
    QVERIFY(isLegalPlay(Card{Suit::Clubs, Rank::Jack}, state));

    const DiscardState wishedState{Card{Suit::Hearts, Rank::King}, Suit::Diamonds};
    QVERIFY(isLegalPlay(Card{Suit::Clubs, Rank::Jack}, wishedState));
}

void TestMoveValidator::wishedSuitOverridesTopCard()
{
    const DiscardState state{Card{Suit::Hearts, Rank::King}, Suit::Diamonds};

    QVERIFY(isLegalPlay(Card{Suit::Diamonds, Rank::Nine}, state));
    QVERIFY(!isLegalPlay(Card{Suit::Hearts, Rank::Queen}, state));
    QVERIFY(!isLegalPlay(Card{Suit::Clubs, Rank::King}, state));
}

void TestMoveValidator::wishedSuitStillAllowsAnotherJack()
{
    const DiscardState state{Card{Suit::Hearts, Rank::King}, Suit::Diamonds};
    QVERIFY(isLegalPlay(Card{Suit::Spades, Rank::Jack}, state));
}

void TestMoveValidator::effectsAreMappedCorrectly()
{
    QCOMPARE(effectOf(Rank::Seven), SpecialEffect::DrawTwo);
    QCOMPARE(effectOf(Rank::Eight), SpecialEffect::Skip);
    QCOMPARE(effectOf(Rank::Jack), SpecialEffect::None);
    QCOMPARE(effectOf(Rank::Ace), SpecialEffect::None);
}

QTEST_APPLESS_MAIN(TestMoveValidator)
#include "test_move_validator.moc"
