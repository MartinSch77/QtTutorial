// SPDX-License-Identifier: MIT
#include "RhiUnderlayItem.h"
#include "RhiUnderlayRenderer.h"

#include <QQuickWindow>

namespace qttutorial::rhi_under_qml {

RhiUnderlayItem::RhiUnderlayItem()
{
    connect(this, &QQuickItem::windowChanged, this, &RhiUnderlayItem::handleWindowChanged);
}

RhiUnderlayItem::~RhiUnderlayItem() = default;

void RhiUnderlayItem::setThreshold(qreal threshold)
{
    if (qFuzzyCompare(m_threshold, threshold)) {
        return;
    }
    m_threshold = threshold;
    emit thresholdChanged();
    if (window()) {
        window()->update();
    }
}

void RhiUnderlayItem::handleWindowChanged(QQuickWindow* window)
{
    if (window) {
        // Qt::DirectConnection is required because beforeSynchronizing() is
        // emitted from the Qt Quick render thread (if there is one) - we
        // want sync() to run on that same thread rather than being queued
        // back to the main/GUI thread.
        connect(window, &QQuickWindow::beforeSynchronizing, this, &RhiUnderlayItem::sync,
                Qt::DirectConnection);
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &RhiUnderlayItem::cleanup,
                Qt::DirectConnection);
    }
}

void RhiUnderlayItem::sync()
{
    if (!m_renderer) {
        m_renderer = new RhiUnderlayRenderer;
        connect(window(), &QQuickWindow::beforeRendering, m_renderer,
                &RhiUnderlayRenderer::frameStart, Qt::DirectConnection);
        // Connecting to beforeRenderPassRecording (rather than
        // afterRenderPassRecording) is what makes this an underlay: our draw
        // call ends up recorded before the Qt Quick scene's own content.
        connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer,
                &RhiUnderlayRenderer::mainPassRecordingStart, Qt::DirectConnection);
    }
    m_renderer->setThreshold(m_threshold);
    m_renderer->setWindow(window());
}

void RhiUnderlayItem::cleanup()
{
    delete m_renderer;
    m_renderer = nullptr;
}

void RhiUnderlayItem::releaseResources()
{
    // Called on the GUI thread when the item is about to be removed from the
    // window; the renderer itself is owned/destroyed via cleanup() on the
    // render thread, so only drop our pointer to it here.
    m_renderer = nullptr;
}

} // namespace qttutorial::rhi_under_qml
