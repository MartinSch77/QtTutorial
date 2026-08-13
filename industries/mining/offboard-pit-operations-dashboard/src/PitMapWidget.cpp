// SPDX-License-Identifier: MIT
#include "PitMapWidget.h"

#include "PitIcons.h"

#include <QPainter>

namespace qttutorial::mining::pit {

PitMapWidget::PitMapWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(220, 220);
}

void PitMapWidget::setSamples(const std::vector<TruckSample>& samples)
{
    m_samples = samples;
    update();
}

QColor PitMapWidget::colorForState(int stateIndex)
{
    switch (stateIndex) {
    case 0:
        return QColor("#e5b93d"); // Loading
    case 1:
        return QColor("#39c0ff"); // Hauling
    case 2:
        return QColor("#ff9f43"); // Dumping
    default:
        return QColor("#3ddc6f"); // Returning
    }
}

void PitMapWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#1c212b"));

    const QRectF pitRect = rect().adjusted(10, 10, -10, -10);
    icons::paintPitBenches(painter, pitRect, QColor("#5c6472"));

    constexpr double kGlyphSize = 16.0;
    for (const TruckSample& sample : m_samples) {
        const double cx = pitRect.left() + sample.positionX * pitRect.width();
        const double cy = pitRect.top() + sample.positionY * pitRect.height();
        const QRectF glyphRect(cx - kGlyphSize / 2.0, cy - kGlyphSize / 2.0, kGlyphSize, kGlyphSize);

        icons::paintTruckGlyph(painter, glyphRect, colorForState(sample.stateIndex));

        if (sample.overloaded) {
            const QRectF warnRect(cx + kGlyphSize * 0.35, cy - kGlyphSize * 0.9, kGlyphSize * 0.6, kGlyphSize * 0.6);
            painter.drawPixmap(warnRect.toRect(),
                                icons::warningTriangleIcon(QColor("#e5484d"), static_cast<int>(kGlyphSize)).pixmap(
                                    static_cast<int>(kGlyphSize)));
        }

        painter.setPen(QColor("#9aa4b2"));
        painter.drawText(QRectF(cx - 24, cy + kGlyphSize / 2.0, 48, 14), Qt::AlignHCenter, sample.id);
    }

    if (m_samples.empty()) {
        painter.setPen(QColor("#9aa4b2"));
        painter.drawText(rect(), Qt::AlignCenter, tr("Waiting for fleet telemetry..."));
    }
}

} // namespace qttutorial::mining::pit
