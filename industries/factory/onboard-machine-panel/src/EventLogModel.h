// SPDX-License-Identifier: MIT
#pragma once

#include <QAbstractListModel>
#include <QDateTime>

#include <vector>

namespace qttutorial::factory_machine_panel {

// A small, in-memory-only (no persistence, per REQ-IND-04) event/fault log
// backing the panel's event list: newest event first, each carrying a
// severity used by the QML delegate to pick a colour. Bounded to the last
// kMaxEntries so the panel never grows without limit across a long-running
// session.
class EventLogModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString latestMessage READ latestMessage NOTIFY countChanged)
    Q_PROPERTY(int latestSeverity READ latestSeverity NOTIFY countChanged)
public:
    enum class Severity { Info, Warning, Fault };

    enum Role { MessageRole = Qt::UserRole + 1, SeverityRole, TimestampRole };

    struct Entry {
        QString message;
        Severity severity;
        QDateTime timestamp;
    };

    explicit EventLogModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void record(const QString& message, Severity severity);
    [[nodiscard]] int count() const { return static_cast<int>(m_entries.size()); }
    [[nodiscard]] const Entry& at(int row) const { return m_entries.at(static_cast<std::size_t>(row)); }

    [[nodiscard]] QString latestMessage() const { return m_entries.empty() ? QString() : m_entries.front().message; }
    [[nodiscard]] int latestSeverity() const
    {
        return m_entries.empty() ? -1 : static_cast<int>(m_entries.front().severity);
    }

signals:
    void countChanged();

private:
    static constexpr std::size_t kMaxEntries = 50;
    std::vector<Entry> m_entries;
};

} // namespace qttutorial::factory_machine_panel
