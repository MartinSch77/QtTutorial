// SPDX-License-Identifier: MIT
#include "KickerProtocol.h"

namespace qttutorial::games::kicker {

namespace {

QJsonObject encodeBall(const BallState& ball)
{
    return QJsonObject{
        {QStringLiteral("x"), ball.x},
        {QStringLiteral("z"), ball.z},
        {QStringLiteral("vx"), ball.vx},
        {QStringLiteral("vz"), ball.vz},
    };
}

BallState decodeBall(const QJsonObject& obj)
{
    BallState ball;
    ball.x = obj.value(QStringLiteral("x")).toDouble();
    ball.z = obj.value(QStringLiteral("z")).toDouble();
    ball.vx = obj.value(QStringLiteral("vx")).toDouble();
    ball.vz = obj.value(QStringLiteral("vz")).toDouble();
    return ball;
}

QJsonObject encodeRod(const RodState& rod)
{
    return QJsonObject{
        {QStringLiteral("slide"), rod.slide},
        {QStringLiteral("rotation"), rod.rotation},
        {QStringLiteral("angularVelocity"), rod.angularVelocity},
    };
}

RodState decodeRod(const QJsonObject& obj)
{
    RodState rod;
    rod.slide = obj.value(QStringLiteral("slide")).toDouble();
    rod.rotation = obj.value(QStringLiteral("rotation")).toDouble();
    rod.angularVelocity = obj.value(QStringLiteral("angularVelocity")).toDouble();
    return rod;
}

} // namespace

QJsonObject encodeMatchState(const MatchState& state)
{
    QJsonArray rods;
    for (const RodState& rod : state.rods) {
        rods.append(encodeRod(rod));
    }
    return QJsonObject{
        {QStringLiteral("ball"), encodeBall(state.ball)},
        {QStringLiteral("rods"), rods},
        {QStringLiteral("score"), QJsonArray{state.score[0], state.score[1]}},
    };
}

MatchState decodeMatchState(const QJsonObject& obj)
{
    MatchState state;
    state.ball = decodeBall(obj.value(QStringLiteral("ball")).toObject());

    const QJsonArray rods = obj.value(QStringLiteral("rods")).toArray();
    for (int i = 0; i < 4 && i < rods.size(); ++i) {
        state.rods[static_cast<std::size_t>(i)] = decodeRod(rods[i].toObject());
    }

    const QJsonArray score = obj.value(QStringLiteral("score")).toArray();
    if (score.size() == 2) {
        state.score = {score[0].toInt(), score[1].toInt()};
    }
    return state;
}

QJsonObject encodeInputPayload(const QList<RodInputEntry>& entries)
{
    QJsonArray rods;
    for (const RodInputEntry& entry : entries) {
        rods.append(QJsonObject{
            {QStringLiteral("rod"), entry.rod},
            {QStringLiteral("slideTarget"), entry.input.slideTarget},
            {QStringLiteral("spin"), entry.input.spin},
        });
    }
    return QJsonObject{{QStringLiteral("rods"), rods}};
}

QList<RodInputEntry> decodeInputPayload(const QJsonObject& obj)
{
    QList<RodInputEntry> entries;
    const QJsonArray rods = obj.value(QStringLiteral("rods")).toArray();
    for (const QJsonValue& value : rods) {
        const QJsonObject rodObj = value.toObject();
        RodInputEntry entry;
        entry.rod = rodObj.value(QStringLiteral("rod")).toInt(-1);
        entry.input.slideTarget = rodObj.value(QStringLiteral("slideTarget")).toDouble();
        entry.input.spin = rodObj.value(QStringLiteral("spin")).toDouble();
        entries.append(entry);
    }
    return entries;
}

} // namespace qttutorial::games::kicker
