// SPDX-License-Identifier: MIT
#include "StatusIconStrip.h"

#include "IconPainter.h"

#include <QPainter>

namespace qttutorial::space {

StatusIconStrip::StatusIconStrip(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(80);
}

void StatusIconStrip::setState(bool charging, bool groundStationInView, bool warning)
{
    m_charging = charging;
    m_groundStationInView = groundStationInView;
    m_warning = warning;
    update();
}

void StatusIconStrip::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x0c, 0x0c, 0x12));

    const int cellCount = 5;
    const double cellWidth = width() / static_cast<double>(cellCount);
    const QColor accent(0x40, 0xc0, 0xff);
    const QColor amber(0xd0, 0xa0, 0x30);

    auto cellRect = [&](int index) {
        return QRectF(index * cellWidth + cellWidth * 0.15, height() * 0.1, cellWidth * 0.7, height() * 0.7);
    };
    auto caption = [&](int index, const QString& text) {
        painter.setPen(Qt::lightGray);
        QFont f = font();
        f.setPointSize(8);
        painter.setFont(f);
        painter.drawText(QRectF(index * cellWidth, height() * 0.82, cellWidth, height() * 0.16), Qt::AlignCenter, text);
    };

    icons::drawSatellite(painter, cellRect(0), accent);
    caption(0, tr("Vehicle"));

    icons::drawOrbitArc(painter, cellRect(1), accent);
    caption(1, tr("Orbit"));

    icons::drawGroundStation(painter, cellRect(2), m_groundStationInView ? accent : QColor(0x60, 0x60, 0x68),
                              m_groundStationInView);
    caption(2, m_groundStationInView ? tr("Station: in view") : tr("Station: out of view"));

    icons::drawBatteryCharge(painter, cellRect(3), m_charging ? accent : amber, m_charging);
    caption(3, m_charging ? tr("Charging") : tr("Discharging"));

    if (m_warning) {
        icons::drawWarningTriangle(painter, cellRect(4), QColor(0xe0, 0x40, 0x30));
        caption(4, tr("Subsystem alert"));
    } else {
        caption(4, tr("All nominal"));
    }
}

} // namespace qttutorial::space
