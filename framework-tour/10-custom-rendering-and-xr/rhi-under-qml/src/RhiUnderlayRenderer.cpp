// SPDX-License-Identifier: MIT
#include "RhiUnderlayRenderer.h"

#include <QFile>

namespace qttutorial::rhi_under_qml {

namespace {

QShader loadShader(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Failed to open shader file: %s", qPrintable(path));
        return {};
    }
    return QShader::fromSerialized(file.readAll());
}

// clang-format off
constexpr float kVertices[] = {
    -0.6f, -0.55f,
     0.6f, -0.55f,
     0.0f,  0.65f,
};
// clang-format on

} // namespace

void RhiUnderlayRenderer::frameStart()
{
    // Invoked on the render thread, if there is one.
    QRhi* rhi = m_window->rhi();
    if (!rhi) {
        qWarning("QQuickWindow is not using QRhi for rendering");
        return;
    }
    QRhiSwapChain* swapChain = m_window->swapChain();
    if (!swapChain) {
        qWarning("No QRhiSwapChain?");
        return;
    }

    QRhiResourceUpdateBatch* resourceUpdates = rhi->nextResourceUpdateBatch();

    if (!m_pipeline) {
        m_vertexShader = loadShader(QStringLiteral(":/shaders/underlay.vert.qsb"));
        m_fragmentShader = loadShader(QStringLiteral(":/shaders/underlay.frag.qsb"));
        if (!m_vertexShader.isValid() || !m_fragmentShader.isValid()) {
            qWarning("Failed to load rhi-under-qml shaders; rendering will be incorrect");
        }

        m_vertexBuffer.reset(rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer,
                                             sizeof(kVertices)));
        m_vertexBuffer->create();
        resourceUpdates->uploadStaticBuffer(m_vertexBuffer.get(), kVertices);

        constexpr quint32 kUniformBufferSize = 4 + 4; // threshold, yFlip - both float
        m_uniformBuffer.reset(rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer,
                                              kUniformBufferSize));
        m_uniformBuffer->create();

        // NDC is Y-up everywhere QRhi supports except Vulkan; without this
        // flip the triangle would point downward on a Vulkan backend while
        // pointing upward everywhere else, purely due to a coordinate-system
        // difference that has nothing to do with our own drawing logic.
        const float yFlip = rhi->isYUpInNDC() ? 1.0f : -1.0f;
        resourceUpdates->updateDynamicBuffer(m_uniformBuffer.get(), 4, 4, &yFlip);

        m_srb.reset(rhi->newShaderResourceBindings());
        const auto visibleToAll =
            QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage;
        m_srb->setBindings(
            {QRhiShaderResourceBinding::uniformBuffer(0, visibleToAll, m_uniformBuffer.get())});
        m_srb->create();

        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings({{2 * sizeof(float)}});
        inputLayout.setAttributes({{0, 0, QRhiVertexInputAttribute::Float2, 0}});

        m_pipeline.reset(rhi->newGraphicsPipeline());
        m_pipeline->setTopology(QRhiGraphicsPipeline::Triangles);
        QRhiGraphicsPipeline::TargetBlend blend;
        blend.enable = true;
        blend.srcColor = QRhiGraphicsPipeline::SrcAlpha;
        blend.srcAlpha = QRhiGraphicsPipeline::SrcAlpha;
        blend.dstColor = QRhiGraphicsPipeline::OneMinusSrcAlpha;
        blend.dstAlpha = QRhiGraphicsPipeline::OneMinusSrcAlpha;
        m_pipeline->setTargetBlends({blend});
        m_pipeline->setShaderStages(
            {{QRhiShaderStage::Vertex, m_vertexShader}, {QRhiShaderStage::Fragment, m_fragmentShader}});
        m_pipeline->setVertexInputLayout(inputLayout);
        m_pipeline->setShaderResourceBindings(m_srb.get());
        m_pipeline->setRenderPassDescriptor(
            swapChain->currentFrameRenderTarget()->renderPassDescriptor());
        m_pipeline->create();
    }

    const float threshold = static_cast<float>(m_threshold);
    resourceUpdates->updateDynamicBuffer(m_uniformBuffer.get(), 0, 4, &threshold);

    swapChain->currentFrameCommandBuffer()->resourceUpdate(resourceUpdates);
}

void RhiUnderlayRenderer::mainPassRecordingStart()
{
    // Invoked on the render thread, if there is one.
    QRhi* rhi = m_window->rhi();
    QRhiSwapChain* swapChain = m_window->swapChain();
    if (!rhi || !swapChain || !m_pipeline) {
        return;
    }

    const QSize outputPixelSize = swapChain->currentFrameRenderTarget()->pixelSize();
    QRhiCommandBuffer* cb = swapChain->currentFrameCommandBuffer();
    cb->setViewport(
        {0.0f, 0.0f, float(outputPixelSize.width()), float(outputPixelSize.height())});
    cb->setGraphicsPipeline(m_pipeline.get());
    cb->setShaderResources();
    const QRhiCommandBuffer::VertexInput vertexBinding(m_vertexBuffer.get(), 0);
    cb->setVertexInput(0, 1, &vertexBinding);
    cb->draw(3);
}

} // namespace qttutorial::rhi_under_qml
