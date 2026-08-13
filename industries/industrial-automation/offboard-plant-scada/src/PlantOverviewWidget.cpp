// SPDX-License-Identifier: MIT
#include "PlantOverviewWidget.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPainterPath>

namespace qttutorial::plant_scada {

namespace {
QColor severityColor(Severity severity)
{
    switch (severity) {
    case Severity::Critical:
        return QColor("#e5484d");
    case Severity::Warning:
        return QColor("#e5b93d");
    case Severity::Normal:
        return QColor("#3ddc84");
    }
    return QColor("#5a6472");
}
}

PlantOverviewWidget::PlantOverviewWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(120);
}

void PlantOverviewWidget::setModel(PlantOverviewModel* model)
{
    if (m_model == model) {
        return;
    }
    if (m_model) {
        disconnect(m_model, nullptr, this, nullptr);
    }
    m_model = model;
    if (m_model) {
        connect(m_model, &QAbstractItemModel::modelReset, this, [this] { update(); });
    }
    update();
}

void PlantOverviewWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#101820"));

    if (!m_model || m_model->lines().empty()) {
        painter.setPen(QColor("#8a94a3"));
        painter.drawText(rect(), Qt::AlignCenter, tr("No plant lines"));
        return;
    }

    const auto& lines = m_model->lines();
    const int count = static_cast<int>(lines.size());
    const double margin = 10.0;
    const double cellWidth = (width() - margin * (count + 1)) / count;

    for (int i = 0; i < count; ++i) {
        const QRectF cell(margin + i * (cellWidth + margin), margin, cellWidth, height() - margin * 2);
        drawLineMimic(painter, cell, lines[static_cast<std::size_t>(i)]);
    }
}

void PlantOverviewWidget::drawLineMimic(QPainter& painter, const QRectF& cell, const LineStatus& status) const
{
    const QColor statusColor = severityColor(status.worstSeverity);
    const QColor equipmentColor = status.running ? QColor("#3ddc84") : QColor("#5a6472");

    painter.setPen(QPen(statusColor, 1.5));
    painter.setBrush(QColor("#182230"));
    painter.drawRoundedRect(cell, 6, 6);

    painter.setPen(QColor("#c8d0da"));
    QFont labelFont = painter.font();
    labelFont.setPixelSize(11);
    labelFont.setBold(true);
    painter.setFont(labelFont);
    painter.drawText(cell.adjusted(8, 6, -8, 0), Qt::AlignLeft | Qt::AlignTop, status.line);

    // A tiny tank -> valve -> conveyor -> motor mimic row, mirroring the
    // process line drawn on the onboard HMI panel, scaled down to fit an
    // overview tile.
    const double rowY = cell.center().y() + cell.height() * 0.12;
    const double glyphH = cell.height() * 0.28;
    double x = cell.left() + 12;

    // Tank.
    const QRectF tankRect(x, rowY - glyphH / 2, glyphH * 0.7, glyphH);
    painter.setPen(QPen(QColor("#5a6472"), 1.5));
    painter.setBrush(QColor("#0d1116"));
    painter.drawRoundedRect(tankRect, 3, 3);
    x = tankRect.right() + 10;

    // Valve (bowtie).
    const double valveSize = glyphH * 0.6;
    QPainterPath valve;
    valve.moveTo(x, rowY - valveSize / 2);
    valve.lineTo(x + valveSize, rowY);
    valve.lineTo(x, rowY + valveSize / 2);
    valve.closeSubpath();
    QPainterPath valve2;
    valve2.moveTo(x + valveSize, rowY - valveSize / 2);
    valve2.lineTo(x, rowY);
    valve2.lineTo(x + valveSize, rowY + valveSize / 2);
    valve2.closeSubpath();
    painter.setPen(QPen(QColor("#0d1116"), 1));
    painter.setBrush(equipmentColor);
    painter.drawPath(valve);
    painter.drawPath(valve2);
    x += valveSize + 10;

    // Conveyor (rounded belt outline).
    const double conveyorWidth = qMax(cell.width() * 0.28, 20.0);
    const QRectF conveyorRect(x, rowY - glyphH * 0.18, conveyorWidth, glyphH * 0.36);
    painter.setPen(QPen(equipmentColor, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(conveyorRect, conveyorRect.height() / 2, conveyorRect.height() / 2);
    x = conveyorRect.right() + 10;

    // Motor (circle with a dot hub).
    const double motorSize = glyphH * 0.7;
    const QRectF motorRect(x, rowY - motorSize / 2, motorSize, motorSize);
    painter.setPen(QPen(equipmentColor, 2));
    painter.setBrush(QColor("#0d1116"));
    painter.drawEllipse(motorRect);
    painter.setBrush(equipmentColor);
    painter.setPen(Qt::NoPen);
    const double hubSize = motorSize * 0.28;
    painter.drawEllipse(motorRect.center(), hubSize / 2, hubSize / 2);

    // Status strip along the bottom: RUNNING/STOPPED plus worst severity.
    QFont statusFont = painter.font();
    statusFont.setPixelSize(10);
    statusFont.setBold(false);
    painter.setFont(statusFont);
    painter.setPen(equipmentColor);
    painter.drawText(cell.adjusted(8, 0, -8, -20), Qt::AlignLeft | Qt::AlignBottom,
                      status.running ? tr("RUNNING") : tr("STOPPED"));
    painter.setPen(statusColor);
    painter.drawText(cell.adjusted(8, 0, -8, -6), Qt::AlignLeft | Qt::AlignBottom,
                      QString::fromLatin1(severityLabel(status.worstSeverity)));
}

} // namespace qttutorial::plant_scada
