// SPDX-License-Identifier: MIT
#include "SceneRegistry.h"

namespace qttutorial::homeautomation {

namespace {

const std::vector<SceneDefinition>& sceneTable()
{
    static const std::vector<SceneDefinition> table = {
        SceneDefinition{
            .id = Scene::Home,
            .name = QStringLiteral("Home"),
            .description = QStringLiteral("Everyday comfort: lights on, doors as you left them, climate steady."),
            .lightsOn = true,
            .brightness = 70,
            .lockAllDoors = false,
            .armSecurity = false,
            .thermostatMode = ThermostatModel::Mode::Heat,
            .thermostatTarget = 21.0,
        },
        SceneDefinition{
            .id = Scene::Away,
            .name = QStringLiteral("Away"),
            .description = QStringLiteral("Nobody home: lights off, every door locked, security armed, heating set back to save energy."),
            .lightsOn = false,
            .brightness = 0,
            .lockAllDoors = true,
            .armSecurity = true,
            .thermostatMode = ThermostatModel::Mode::Heat,
            .thermostatTarget = 16.0,
        },
        SceneDefinition{
            .id = Scene::Night,
            .name = QStringLiteral("Night"),
            .description = QStringLiteral("Bedtime: lights dimmed low, doors locked, security armed, a few degrees cooler for sleeping."),
            .lightsOn = true,
            .brightness = 15,
            .lockAllDoors = true,
            .armSecurity = true,
            .thermostatMode = ThermostatModel::Mode::Heat,
            .thermostatTarget = 18.0,
        },
        SceneDefinition{
            .id = Scene::Morning,
            .name = QStringLiteral("Morning"),
            .description = QStringLiteral("Rise and shine: lights up bright, security stands down for the day, climate back to comfortable."),
            .lightsOn = true,
            .brightness = 90,
            .lockAllDoors = false,
            .armSecurity = false,
            .thermostatMode = ThermostatModel::Mode::Heat,
            .thermostatTarget = 21.0,
        },
    };
    return table;
}

} // namespace

const std::vector<SceneDefinition>& SceneRegistry::scenes()
{
    return sceneTable();
}

const SceneDefinition& SceneRegistry::definition(Scene id)
{
    for (const SceneDefinition& scene : sceneTable()) {
        if (scene.id == id) {
            return scene;
        }
    }
    return sceneTable().front();
}

void SceneRegistry::applyToRooms(Scene id, RoomRegistry& rooms)
{
    const SceneDefinition& scene = definition(id);
    for (int i = 0; i < rooms.count(); ++i) {
        rooms.setLightOn(i, scene.lightsOn);
        rooms.setBrightness(i, scene.brightness);
    }
}

void SceneRegistry::applyToLocks(Scene id, LockRegistry& locks, const QString& actor, qint64 timestampMs)
{
    const SceneDefinition& scene = definition(id);
    if (!scene.lockAllDoors) {
        return;
    }
    for (int i = 0; i < locks.count(); ++i) {
        locks.setLocked(i, true, actor, timestampMs);
    }
}

void SceneRegistry::applyToSecurity(Scene id, SecurityCenter& security)
{
    security.setArmed(definition(id).armSecurity);
}

void SceneRegistry::applyToThermostat(Scene id, ThermostatModel& thermostat)
{
    const SceneDefinition& scene = definition(id);
    thermostat.setMode(scene.thermostatMode);
    thermostat.setTargetTemperature(scene.thermostatTarget);
}

} // namespace qttutorial::homeautomation
