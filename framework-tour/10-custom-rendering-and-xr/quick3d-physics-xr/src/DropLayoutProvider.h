// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QVariantList>

namespace qttutorial::quick3d_physics {

// Thin QML-facing wrapper around the pure DropLayout helper: converts its
// plain DropPosition values into vector3d-compatible QVariants so QML can use
// the result directly as a Repeater3D model.
class DropLayoutProvider : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit DropLayoutProvider(QObject* parent = nullptr)
        : QObject(parent)
    {
    }

    [[nodiscard]] Q_INVOKABLE QVariantList gridPositions(int count, double radius,
                                                          double dropHeight) const;
};

} // namespace qttutorial::quick3d_physics
