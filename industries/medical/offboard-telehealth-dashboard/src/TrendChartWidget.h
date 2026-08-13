// SPDX-License-Identifier: MIT
#pragma once

#include <QString>
#include <QWidget>

namespace qttutorial::medical::telehealth {

class VitalsHistoryStore;

// A small QPainter-drawn line chart (no charting library, matching the
// approach already used by SparklineDelegate) showing one selected
// patient's heart-rate trend over the last few minutes, sourced from
// VitalsHistoryStore.
class TrendChartWidget : public QWidget {
    Q_OBJECT
public:
    static constexpr int kTrendMinutes = 3;
    static constexpr int kSampleIntervalSeconds = 1;
    static constexpr int kMaxSamples = kTrendMinutes * 60 / kSampleIntervalSeconds;

    explicit TrendChartWidget(QWidget* parent = nullptr);

    void setHistoryStore(VitalsHistoryStore* store);
    void setSelectedPatient(const QString& patientId, const QString& patientName);
    void refresh();

    [[nodiscard]] QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    VitalsHistoryStore* m_store = nullptr;
    QString m_patientId;
    QString m_patientName;
};

} // namespace qttutorial::medical::telehealth
