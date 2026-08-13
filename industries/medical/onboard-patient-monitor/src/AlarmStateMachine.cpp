// SPDX-License-Identifier: MIT
#include "AlarmStateMachine.h"

namespace qttutorial::medical {

AlarmLevel AlarmStateMachine::update(double heartRateBpm, double spo2Percent)
{
    const bool meetsCriticalEnter = heartRateBpm >= kCriticalEnterHeartRate || spo2Percent <= kCriticalEnterSpo2;
    const bool meetsWarningEnter = heartRateBpm >= kWarningEnterHeartRate || spo2Percent <= kWarningEnterSpo2;
    const bool clearOfWarning = heartRateBpm < kWarningExitHeartRate && spo2Percent > kWarningExitSpo2;
    const bool clearOfCritical = heartRateBpm < kCriticalExitHeartRate && spo2Percent > kCriticalExitSpo2;

    switch (m_level) {
    case AlarmLevel::Normal:
        if (meetsCriticalEnter) {
            m_level = AlarmLevel::Critical;
        } else if (meetsWarningEnter) {
            m_level = AlarmLevel::Warning;
        }
        break;
    case AlarmLevel::Warning:
        if (meetsCriticalEnter) {
            m_level = AlarmLevel::Critical;
        } else if (clearOfWarning) {
            m_level = AlarmLevel::Normal;
        }
        break;
    case AlarmLevel::Critical:
        if (clearOfCritical) {
            m_level = meetsWarningEnter ? AlarmLevel::Warning : AlarmLevel::Normal;
        }
        break;
    }

    return m_level;
}

} // namespace qttutorial::medical
