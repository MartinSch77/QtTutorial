// SPDX-License-Identifier: MIT
#pragma once

#include "PatientListModel.h"

#include <QWidget>

#include <vector>

class QGridLayout;

namespace qttutorial::medical::telehealth {

// A single small "at a glance" vitals card for one patient: name, heart
// rate, SpO2 and blood pressure each labelled with a procedurally-drawn
// clinical icon (IconPainter), with a severity-coloured border. Clicking a
// card selects that patient for the trend chart.
class PatientCardWidget : public QWidget {
    Q_OBJECT
public:
    explicit PatientCardWidget(QWidget* parent = nullptr);

    void setVitals(const PatientVitals& vitals, AlarmSeverity severity);

    [[nodiscard]] QSize sizeHint() const override;

signals:
    void clicked(const QString& patientId, const QString& name);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    PatientVitals m_vitals;
    AlarmSeverity m_severity = AlarmSeverity::Normal;
};

// A grid of PatientCardWidget instances, one per patient tracked by a
// PatientListModel - a multi-patient overview that a charge nurse or
// telehealth manager could scan at a glance before drilling into any one
// patient's detailed trend.
class PatientOverviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit PatientOverviewWidget(PatientListModel* model, QWidget* parent = nullptr);

signals:
    void patientSelected(const QString& patientId, const QString& name);

private:
    void refresh();

    PatientListModel* m_model;
    std::vector<PatientCardWidget*> m_cards;
};

} // namespace qttutorial::medical::telehealth
