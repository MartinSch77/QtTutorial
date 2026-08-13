// SPDX-License-Identifier: MIT
#pragma once

#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>

namespace qttutorial::rhi_under_qml {

class RhiUnderlayRenderer;

// Mirrors the RhiSquircle item from Qt's own "Scene Graph - RHI Under QML"
// example: a QQuickItem that never sets ItemHasContents and never
// reimplements updatePaintNode(). It draws nothing itself; it only owns the
// `threshold` value exposed to QML and forwards it, on the render thread, to
// an RhiUnderlayRenderer that issues raw QRhi draw calls underneath the rest
// of the Qt Quick scene.
class RhiUnderlayItem : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(qreal threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
public:
    RhiUnderlayItem();
    ~RhiUnderlayItem() override;

    [[nodiscard]] qreal threshold() const { return m_threshold; }
    void setThreshold(qreal threshold);

signals:
    void thresholdChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow* window);

private:
    void releaseResources() override;

    qreal m_threshold = 0.0;
    RhiUnderlayRenderer* m_renderer = nullptr;
};

} // namespace qttutorial::rhi_under_qml
