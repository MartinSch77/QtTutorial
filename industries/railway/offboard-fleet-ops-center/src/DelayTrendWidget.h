// SPDX-License-Identifier: MIT
#pragma once

#include "RunHistoryStore.h"

#include <QWidget>

namespace qttutorial::fleet_ops {

// A hand-drawn punctuality/delay trend (no Qt Charts / Qt Graphs, both
// commercial-only): a zero line plus a polyline of delay-minutes samples for
// the selected train over the queried time window.
class DelayTrendWidget : public QWidget {
    Q_OBJECT
public:
    explicit DelayTrendWidget(QWidget* parent = nullptr);

    void setSamples(const std::vector<RunSample>& samples);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<RunSample> m_samples;
};

} // namespace qttutorial::fleet_ops
