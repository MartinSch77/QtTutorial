// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QVariantList>

#include <QtGraphs/qsurfacedataproxy.h>

namespace qttutorial::offboard_digital_twin {

// QSurfaceDataProxy::resetArray()/setRow()/setRows() are plain C++ methods,
// not Q_INVOKABLE (confirmed by reading qsurfacedataproxy.h directly - Qt
// Graphs' QML API only exposes read-only rowCount/columnCount/series on the
// proxy itself), so a Surface3D chart's data cannot be populated purely
// from QML property bindings. This bridge owns a QSurfaceDataProxy and
// exposes the one C++ call QML actually needs.
class SurfaceGridBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QSurfaceDataProxy *proxy READ proxy CONSTANT)

public:
    explicit SurfaceGridBridge(QObject *parent = nullptr);

    [[nodiscard]] QSurfaceDataProxy *proxy() const;

    // `rows` is a JS array of arrays of numbers (root.surfaceGrid: the
    // temperature grid), not an already-built vector3d array - this method
    // does the row/column -> QSurfaceDataItem(x, value, z) conversion that
    // used to be attempted (incorrectly) directly in QML.
    Q_INVOKABLE void setGrid(const QVariantList &rows);

private:
    QSurfaceDataProxy *m_proxy;
};

} // namespace qttutorial::offboard_digital_twin
