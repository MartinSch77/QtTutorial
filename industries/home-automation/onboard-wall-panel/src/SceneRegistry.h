// SPDX-License-Identifier: MIT
#pragma once

#include "LockRegistry.h"
#include "RoomRegistry.h"
#include "SecurityCenter.h"
#include "ThermostatModel.h"

#include <QString>

#include <vector>

namespace qttutorial::homeautomation {

// The four "one tap changes everything" scenes a Google Home/Apple
// Home/Control4-style app offers from its dashboard, standing in for a
// household's actual daily routine rather than one more device toggle.
enum class Scene {
    Home,
    Away,
    Night,
    Morning,
};

// Static description of what a scene asks every device category to do. Kept
// separate from the *_Registry classes themselves since a scene is a policy
// that spans all of them, not a property of any single device.
struct SceneDefinition {
    Scene id;
    QString name;
    QString description;
    bool lightsOn = false;
    int brightness = 70;
    bool lockAllDoors = false;
    bool armSecurity = false;
    ThermostatModel::Mode thermostatMode = ThermostatModel::Mode::Heat;
    double thermostatTarget = 21.0;
};

// Applies a named scene to every device registry at once - the thing that
// makes a scene selector feel like more than a bookmark for one control.
// Pure C++, no Qt GUI dependency, so each scene's cross-device effect is
// unit testable without a QML engine.
class SceneRegistry {
public:
    [[nodiscard]] static const std::vector<SceneDefinition>& scenes();
    [[nodiscard]] static const SceneDefinition& definition(Scene id);

    // Every apply* function only ever *sets* device state (never reads it
    // back into the definition), so calling the same scene twice in a row is
    // always idempotent.
    static void applyToRooms(Scene id, RoomRegistry& rooms);
    // `actor` and `timestampMs` feed LockRegistry's own change log, so
    // "Scene: Away" shows up in Recent Activity exactly like a manual toggle
    // would.
    static void applyToLocks(Scene id, LockRegistry& locks, const QString& actor, qint64 timestampMs);
    static void applyToSecurity(Scene id, SecurityCenter& security);
    // Only moves the thermostat's mode/target - ThermostatModel::advance()
    // still drifts the *current* temperature toward that target at its own
    // realistic rate, so a scene change is felt gradually, not instantly.
    static void applyToThermostat(Scene id, ThermostatModel& thermostat);
};

} // namespace qttutorial::homeautomation
