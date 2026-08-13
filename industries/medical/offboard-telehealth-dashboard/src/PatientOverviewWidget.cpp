// SPDX-License-Identifier: MIT
#include "PatientOverviewWidget.h"

#include "IconPainter.h"

#include <QGridLayout>
#include <QMouseEvent>
#include <QPainter>

namespace qttutorial::medical::telehealth {

namespace {
constexpr int kCardWidth = 190;
constexpr int kCardHeight = 118;
constexpr int kColumns = 3;

QColor severityColor(AlarmSeverity severity)
{
    switch (severity) {
    case AlarmSeverity::Critical:
        return QColor("#ff6b6b");
    case AlarmSeverity::Warning:
        return QColor("#ffcc66");
    case AlarmSeverity::Normal:
        return QColor("#3ddc6f");
    }
    return QColor("#3ddc6f");
}
} // namespace

PatientCardWidget::PatientCardWidget(QWidget* parent)
    : QWidget(parent)
{
    setCursor(Qt::PointingHandCursor);
}

void PatientCardWidget::setVitals(const PatientVitals& vitals, AlarmSeverity severity)
{
    m_vitals = vitals;
    m_severity = severity;
    update();
}

QSize PatientCardWidget::sizeHint() const
{
    return QSize(kCardWidth, kCardHeight);
}

void PatientCardWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QColor accent = severityColor(m_severity);
    const QRectF cardRect = rect().adjusted(1, 1, -1, -1);
    painter.setPen(QPen(accent, 2));
    painter.setBrush(QColor("#1c212b"));
    painter.drawRoundedRect(cardRect, 8, 8);

    painter.setPen(Qt::white);
    QFont nameFont = painter.font();
    nameFont.setBold(true);
    painter.setFont(nameFont);
    painter.drawText(QRectF(cardRect.left() + 10, cardRect.top() + 6, cardRect.width() - 20, 20),
                      Qt::AlignLeft | Qt::AlignVCenter, m_vitals.name);

    const double rowY = cardRect.top() + 34;
    const double iconSize = 16;
    const double rowHeight = 24;

    IconPainter::paintHeartbeat(painter, QRectF(cardRect.left() + 10, rowY, iconSize, iconSize), accent);
    painter.drawText(QRectF(cardRect.left() + 32, rowY - 3, cardRect.width() - 42, iconSize + 6),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      QStringLiteral("%1 bpm").arg(qRound(m_vitals.heartRate)));

    IconPainter::paintDroplet(painter, QRectF(cardRect.left() + 10, rowY + rowHeight, iconSize, iconSize), accent);
    painter.drawText(QRectF(cardRect.left() + 32, rowY + rowHeight - 3, cardRect.width() - 42, iconSize + 6),
                      Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("SpO2 %1%").arg(qRound(m_vitals.spo2)));

    IconPainter::paintCuff(painter, QRectF(cardRect.left() + 10, rowY + 2 * rowHeight, iconSize, iconSize), accent);
    painter.drawText(QRectF(cardRect.left() + 32, rowY + 2 * rowHeight - 3, cardRect.width() - 42, iconSize + 6),
                      Qt::AlignLeft | Qt::AlignVCenter,
                      QStringLiteral("%1/%2 mmHg").arg(qRound(m_vitals.systolic)).arg(qRound(m_vitals.diastolic)));
}

void PatientCardWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_vitals.id, m_vitals.name);
    }
    QWidget::mousePressEvent(event);
}

PatientOverviewWidget::PatientOverviewWidget(PatientListModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
{
    auto* layout = new QGridLayout(this);
    layout->setSpacing(10);

    const int rowCount = m_model->rowCount();
    m_cards.reserve(static_cast<std::size_t>(rowCount));
    for (int i = 0; i < rowCount; ++i) {
        auto* card = new PatientCardWidget(this);
        connect(card, &PatientCardWidget::clicked, this, &PatientOverviewWidget::patientSelected);
        layout->addWidget(card, i / kColumns, i % kColumns);
        m_cards.push_back(card);
    }

    connect(m_model, &PatientListModel::samplesUpdated, this, &PatientOverviewWidget::refresh);
    refresh();
}

void PatientOverviewWidget::refresh()
{
    for (std::size_t i = 0; i < m_cards.size(); ++i) {
        const int row = static_cast<int>(i);
        m_cards[i]->setVitals(m_model->vitalsAt(row), m_model->severityAt(row));
    }
}

} // namespace qttutorial::medical::telehealth
