// SPDX-License-Identifier: GPL-3.0-or-later
#include "AlarmAudioController.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::offboard_digital_twin {

AlarmAudioController::AlarmAudioController(QObject *parent)
    : QObject(parent)
#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO
    , m_engine(std::make_unique<QAudioEngine>())
#endif
{
#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO
    m_engine->setOutputMode(QAudioEngine::Surround);
    m_listener = std::make_unique<QAudioListener>(m_engine.get());
    m_alarmSound = std::make_unique<QSpatialSound>(m_engine.get());
    m_alarmSound->setLoops(QSpatialSound::Infinite);
    m_alarmSound->setAutoPlay(false);
    m_alarmSound->setDistanceModel(QSpatialSound::DistanceModel::Linear);
    // No .wav asset is bundled: this sandbox has no audio-authoring tool
    // available to synthesize one, and the brief asks us not to fetch binary
    // assets from the internet. m_alarmSound->setSource(...) is therefore
    // intentionally left unset here; a real deployment (or a future PR, on a
    // Qt install where OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO is actually
    // defined) would point it at a real alarm cue file. play() on a sourceless
    // QSpatialSound is a documented no-op, so this stays behavior-safe.
    m_engine->start();
#endif
}

AlarmAudioController::~AlarmAudioController() = default;

bool AlarmAudioController::realAudioAvailable()
{
#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO
    return true;
#else
    return false;
#endif
}

AlarmAudioController::Cue AlarmAudioController::computeCue(const QVector3D &listenerPos,
                                                            const QVector3D &listenerForward,
                                                            const QVector3D &sourcePos,
                                                            double sourceLoudness)
{
    const QVector3D toSource = sourcePos - listenerPos;
    const float distance = toSource.length();

    // Linear rolloff out to 20 world units, floor at 0 (fully inaudible).
    constexpr double kCutoffDistance = 20.0;
    const double attenuatedGain = std::clamp(sourceLoudness * (1.0 - static_cast<double>(distance) / kCutoffDistance), 0.0, 1.0);

    // Pan from the horizontal angle between the listener's forward direction
    // and the direction to the source: project onto the listener's right
    // vector (forward rotated -90 degrees around world up) and normalize.
    QVector3D forward = listenerForward;
    forward.setY(0.0f);
    if (forward.lengthSquared() < 1e-6f)
        forward = QVector3D(0.0f, 0.0f, -1.0f);
    forward.normalize();
    const QVector3D up(0.0f, 1.0f, 0.0f);
    const QVector3D right = QVector3D::crossProduct(forward, up).normalized();

    QVector3D flatToSource = toSource;
    flatToSource.setY(0.0f);
    double panValue = 0.0;
    if (flatToSource.lengthSquared() > 1e-6f) {
        flatToSource.normalize();
        panValue = std::clamp(static_cast<double>(QVector3D::dotProduct(flatToSource, right)), -1.0, 1.0);
    }

    return Cue{panValue, attenuatedGain};
}

void AlarmAudioController::startAlarm(const QVector3D &sourcePos)
{
    m_sourcePos = sourcePos;
    m_active = true;
    emit activeChanged();
    recompute();

#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO
    m_alarmSound->setPosition(sourcePos * 100.0f); // meters -> centimeters, per QSpatialSound::position docs
    m_alarmSound->play();
#endif
}

void AlarmAudioController::stopAlarm()
{
    if (!m_active)
        return;
    m_active = false;
    emit activeChanged();

#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO
    m_alarmSound->stop();
#endif
}

void AlarmAudioController::updateListener(const QVector3D &listenerPos, const QVector3D &listenerForward)
{
    m_listenerPos = listenerPos;
    m_listenerForward = listenerForward;
    recompute();

#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO
    m_listener->setPosition(listenerPos * 100.0f);
#endif
}

void AlarmAudioController::recompute()
{
    if (!m_active)
        return;
    const Cue cue = computeCue(m_listenerPos, m_listenerForward, m_sourcePos);
    if (cue.pan != m_pan || cue.gain != m_gain) {
        m_pan = cue.pan;
        m_gain = cue.gain;
        emit cueChanged();
    }
}

} // namespace qttutorial::offboard_digital_twin
