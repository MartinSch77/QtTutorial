// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QVector3D>

#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO
#include <QAudioEngine>
#include <QAudioListener>
#include <QSpatialSound>
#include <memory>
#endif

namespace qttutorial::offboard_digital_twin {

// Step 2 of the demo scenario: a spatial-audio alarm that audibly comes
// "from" the overheating machine's 3D position relative to the camera.
//
// The brief asked us to attempt Qt6::Multimedia + Qt6::SpatialAudio
// (QSpatialSound/QAudioEngine/QAudioListener - all confirmed real via
// qt_documentation_read on qspatialsound.html/qaudioengine.html/
// qaudiolistener.html: `find_package(Qt6 REQUIRED COMPONENTS SpatialAudio)`)
// first. Neither Qt6Multimedia nor Qt6SpatialAudio is present in this
// repository's local Qt 6.4.2 baseline (no libQt6SpatialAudio*/
// libQt6Multimedia* under the active Qt prefix), so the real playback path
// could not be configured or built in this sandbox. The class therefore
// always computes the pan/attenuation math itself (computeCue(), pure and
// unit-testable without any audio backend); when
// OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO is defined (i.e. this repo is
// built against a Qt >= 6.5 install that has both modules - e.g. CI's Qt
// 6.11.1), that math additionally drives a real QSpatialSound positioned at
// the machine's 3D coordinates, played through a QAudioEngine with a
// QAudioListener at the camera position. Without it, the alarm is
// visual-only (a pulsing red banner + speaker-muted icon in the UI) and
// updateCue() still reports the pan/gain values it *would* have used, so the
// behavior is honestly degraded rather than silently faked.
class AlarmAudioController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(double pan READ pan NOTIFY cueChanged)     // -1 (full left) .. +1 (full right)
    Q_PROPERTY(double gain READ gain NOTIFY cueChanged)   // 0 (inaudible) .. 1 (full volume)
    Q_PROPERTY(bool realAudioAvailable READ realAudioAvailable CONSTANT)

public:
    explicit AlarmAudioController(QObject *parent = nullptr);
    ~AlarmAudioController() override;

    [[nodiscard]] bool isActive() const { return m_active; }
    [[nodiscard]] double pan() const { return m_pan; }
    [[nodiscard]] double gain() const { return m_gain; }
    [[nodiscard]] static bool realAudioAvailable();

    // Pure, unit-tested attenuation/pan model: gain falls off with distance
    // (simple inverse-square-ish rolloff clamped to [0,1]); pan is derived
    // from the source's position relative to the listener's right vector.
    struct Cue {
        double pan = 0.0;
        double gain = 0.0;
    };
    [[nodiscard]] static Cue computeCue(const QVector3D &listenerPos, const QVector3D &listenerForward,
                                        const QVector3D &sourcePos, double sourceLoudness = 1.0);

public slots:
    void startAlarm(const QVector3D &sourcePos);
    void stopAlarm();
    void updateListener(const QVector3D &listenerPos, const QVector3D &listenerForward);

signals:
    void activeChanged();
    void cueChanged();

private:
    void recompute();

    bool m_active = false;
    QVector3D m_sourcePos;
    QVector3D m_listenerPos;
    QVector3D m_listenerForward{0.0f, 0.0f, -1.0f};
    double m_pan = 0.0;
    double m_gain = 0.0;

#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO
    std::unique_ptr<QAudioEngine> m_engine;
    std::unique_ptr<QAudioListener> m_listener;
    std::unique_ptr<QSpatialSound> m_alarmSound;
#endif
};

} // namespace qttutorial::offboard_digital_twin
