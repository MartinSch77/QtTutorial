// SPDX-License-Identifier: MIT
#pragma once

#include <QQuickWindow>

// QShader has no public doc page (it is a private-API class, like QRhi
// itself) so its exact header path could not be verified against Qt
// documentation the way <rhi/qrhi.h> was. It is included from the same
// restructured "rhi/" private-header directory as QRhi on the assumption
// that it moved there together with QRhi; this file's <rhi/qrhi.h>
// availability guard in CMakeLists.txt covers this module as a whole, so if
// this assumption is wrong for a given Qt version, the failure surfaces as
// an ordinary build error in this one optional module rather than breaking
// the rest of the repository's configure step.
#include <rhi/qrhi.h>
#include <rhi/qshader.h>

#include <memory>

namespace qttutorial::rhi_under_qml {

// Owns the QRhi resources (buffers, pipeline, shader resource bindings) that
// draw a single hard-edged, two-color triangle underneath the Qt Quick
// scene. Lives on the Qt Quick render thread (mirrors SquircleRenderer from
// Qt's "Scene Graph - RHI Under QML" example).
class RhiUnderlayRenderer : public QObject {
    Q_OBJECT
public:
    void setThreshold(qreal threshold) { m_threshold = threshold; }
    void setWindow(QQuickWindow* window) { m_window = window; }

public slots:
    void frameStart();
    void mainPassRecordingStart();

private:
    QQuickWindow* m_window = nullptr;
    qreal m_threshold = 0.0;

    QShader m_vertexShader;
    QShader m_fragmentShader;
    std::unique_ptr<QRhiBuffer> m_vertexBuffer;
    std::unique_ptr<QRhiBuffer> m_uniformBuffer;
    std::unique_ptr<QRhiShaderResourceBindings> m_srb;
    std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
};

} // namespace qttutorial::rhi_under_qml
