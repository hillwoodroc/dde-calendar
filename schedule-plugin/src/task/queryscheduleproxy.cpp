/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     uniontech  <uniontech@uniontech.com>
*
* Maintainer: uniontech  <chenhaifeng@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "queryscheduleproxy.h"

#include "../globaldef.h"
#include "../data/changejsondata.h"

bool scheduleSort(const ScheduleDtailInfo &s1, const ScheduleDtailInfo &s2)
{
    if (s1.beginDateTime < s2.beginDateTime) {
        return true;
    } else if (s1.beginDateTime == s2.beginDateTime) {
        return s1.endDateTime < s2.endDateTime;
    } else {
        return false;
    }
}

queryScheduleProxy::queryScheduleProxy(JsonData *jsonData, CSchedulesDBus *dbus)
    : m_queryJsonData(jsonData)
    , m_dbus(dbus)
{
}

QVector<ScheduleDtailInfo> queryScheduleProxy::querySchedule()
{
    QVector<ScheduleDtailInfo> scheduleInfo {};
    scheduleInfo.clear();
    switch (m_queryJsonData->getRepeatStatus()) {
    case JsonData::RepeatStatus::NONE:
        scheduleInfo = queryNonRepeatingSchedule();
        break;
    case JsonData::RepeatStatus::EVED: {
        TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
        scheduleInfo = queryEveryDaySchedule(beginTime, endTime);
    } break;
    case JsonData::RepeatStatus::EVEW: {
        TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
        int beginW = 0;
        int endW = 0;
        if (m_queryJsonData->getRepeatNum().size() == 0) {
        } else if (m_queryJsonData->getRepeatNum().size() == 1) {
            beginW = m_queryJsonData->getRepeatNum().at(0);
            endW = m_queryJsonData->getRepeatNum().at(0);
        } else {
            beginW = m_queryJsonData->getRepeatNum().at(0);
            endW = m_queryJsonData->getRepeatNum().at(1);
        }
        scheduleInfo = queryWeeklySchedule(beginTime, endTime, beginW, endW);
        QVector<DateTimeInfo> queryDatetime = getQueryDateTime(m_queryJsonData);
        if (queryDatetime.size() == 0) {
        } else if (queryDatetime.size() == 1) {
            if (queryDatetime.at(0).hasTime) {
                QTime fileterTime = queryDatetime.at(0).datetime.time();
                scheduleInfo = scheduleFileterByTime(scheduleInfo, fileterTime, fileterTime);
            }
        } else {
            QTime fileterBeginTime = queryDatetime.at(0).datetime.time();
            QTime fileterEndTime = queryDatetime.at(1).datetime.time();
            scheduleInfo = scheduleFileterByTime(scheduleInfo, fileterBeginTime, fileterEndTime);
        }
    } break;
    case JsonData::RepeatStatus::EVEM: {
        TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
        int beginM = 0;
        int endM = 0;
        if (m_queryJsonData->getRepeatNum().size() == 0) {
        } else if (m_queryJsonData->getRepeatNum().size() == 1) {
            beginM = m_queryJsonData->getRepeatNum().at(0);
            endM = m_queryJsonData->getRepeatNum().at(0);
        } else {
            beginM = m_queryJsonData->getRepeatNum().at(0);
            endM = m_queryJsonData->getRepeatNum().at(1);
        }
        scheduleInfo = queryMonthlySchedule(beginTime, endTime, beginM, endM);
    } break;
    case JsonData::RepeatStatus::EVEY: {
        TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
        scheduleInfo = queryEveryYearSchedule(beginTime, endTime);
    } break;
    case JsonData::RepeatStatus::RESTD: {
        TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
        scheduleInfo = queryWeeklySchedule(beginTime, endTime, 6, 7);
    } break;
    case JsonData::RepeatStatus::WORKD: {
        TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
        scheduleInfo = queryWorkingDaySchedule(beginTime, endTime);
    } break;
    }
    return scheduleInfo;
}

QVector<ScheduleDtailInfo> queryScheduleProxy::queryWeeklySchedule(QDateTime &beginTime, QDateTime &endTime, int beginW, int endW)
{
    QSet<int> weeklySet;
    if (beginW == endW) {
        weeklySet.insert(beginW);
    } else if (beginW < endW) {
        if (beginW == 1) {
            if (endW == 5) {
                //WORK
                return queryWorkingDaySchedule(beginTime, endTime);
            } else if (endW == 7) {
                //EVED
                return queryEveryDaySchedule(beginTime, endTime);
            }
        }
        for (int i = beginW; i <= endW; ++i) {
            weeklySet.insert(i);
        }
    } else {
        if (beginW - endW == 1) {
            //EVED
            return queryEveryDaySchedule(beginTime, endTime);
        }
        int w;
        for (int i = beginW; i <= endW + 7; ++i) {
            w = i % 7;
            w = w ? w : 7;
            weeklySet.insert(w);
        }
    }
    QVector<ScheduleDateRangeInfo> out {};
    m_dbus->QueryJobsWithRule(beginTime, endTime, DBUS_RRUL_EVEW, out);

    if (beginW == 0 || endW == 0) {
        weeklySet.clear();
    }

    return WeeklyScheduleFileter(out, weeklySet);
}

QVector<ScheduleDtailInfo> queryScheduleProxy::queryMonthlySchedule(QDateTime &beginTime, QDateTime &endTime, int beginM, int endM)
{
    QVector<ScheduleDateRangeInfo> out {};
    m_dbus->QueryJobsWithRule(beginTime, endTime, DBUS_RRUL_EVEM, out);
    return MonthlyScheduleFileter(out, beginM, endM);
}

QVector<ScheduleDtailInfo> queryScheduleProxy::queryEveryDaySchedule(QDateTime &beginTime, QDateTime &endTime)
{
    QVector<ScheduleDateRangeInfo> out {};
    m_dbus->QueryJobsWithRule(beginTime, endTime, DBUS_RRUL_EVED, out);
    return sortAndFilterSchedule(out);
}

QVector<ScheduleDtailInfo> queryScheduleProxy::queryEveryYearSchedule(QDateTime &beginTime, QDateTime &endTime)
{
    QVector<ScheduleDateRangeInfo> out {};
    m_dbus->QueryJobsWithRule(beginTime, endTime, DBUS_RRUL_EVEY, out);
    return sortAndFilterSchedule(out);
}

QVector<ScheduleDtailInfo> queryScheduleProxy::queryWorkingDaySchedule(QDateTime &beginTime, QDateTime &endTime)
{
    QVector<ScheduleDateRangeInfo> out {};
    m_dbus->QueryJobsWithRule(beginTime, endTime, DBUS_RRUL_WORK, out);
    return sortAndFilterSchedule(out);
}

QVector<ScheduleDtailInfo> queryScheduleProxy::queryNonRepeatingSchedule()
{
    QVector<ScheduleDtailInfo> mScheduleInfoVector {};
    mScheduleInfoVector.clear();
    QVector<DateTimeInfo> queryDatetime = getQueryDateTime(m_queryJsonData);
    switch (m_queryJsonData->getPropertyStatus()) {
    case JsonData::PropertyStatus::ALL: {
        TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
        mScheduleInfoVector = queryAllSchedule("", beginTime, endTime);
        if (queryDatetime.size() == 1
            && queryDatetime.at(0).hasTime) {
            QTime fileterTime = queryDatetime.at(0).datetime.time();
            mScheduleInfoVector = scheduleFileterByTime(mScheduleInfoVector, fileterTime, fileterTime);
        }
    } break;
    case JsonData::PropertyStatus::NEXT: {
        TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
        mScheduleInfoVector = queryNextNumSchedule(beginTime, endTime, 1);
    } break;
    case JsonData::PropertyStatus::LAST: {
    } break;
    default: {
        //NONE
        if (queryDatetime.size() == 0) {
            TIME_FRAME_IN_THE_NEXT_SIX_MONTHT
            mScheduleInfoVector = queryAllSchedule(m_queryJsonData->TitleName(), beginTime, endTime);
        } else if (queryDatetime.size() == 1) {
            QDateTime beginTime;
            QDateTime endTime;
            beginTime = queryDatetime.at(0).datetime;
            if (queryDatetime.at(0).hasTime) {
                endTime = beginTime;
            } else {
                endTime.setDate(beginTime.date());
                endTime.setTime(QTime(23, 59, 59));
            }
            mScheduleInfoVector = queryAllSchedule(m_queryJsonData->TitleName(), beginTime, endTime);
        } else {
            QDateTime beginTime;
            QDateTime endTime;
            beginTime = queryDatetime.at(0).datetime;
            if (queryDatetime.at(1).hasTime) {
                endTime = queryDatetime.at(1).datetime;
            } else {
                endTime.setDate(queryDatetime.at(1).datetime.date());
                endTime.setTime(QTime(23, 59, 59));
            }
            mScheduleInfoVector = queryAllSchedule(m_queryJsonData->TitleName(), beginTime, endTime);
        }
    } break;
    }
    return mScheduleInfoVector;
}

QVector<ScheduleDtailInfo> queryScheduleProxy::queryNextNumSchedule(QDateTime &beginTime, QDateTime &endTime, int NextNum)
{
    QVector<ScheduleDtailInfo> mScheduleInfoVector {};
    QVector<ScheduleDateRangeInfo> out {};
    m_dbus->QueryJobsWithLimit(beginTime, endTime, NextNum, out);
    return sortAndFilterSchedule(out);
}

QVector<ScheduleDtailInfo> queryScheduleProxy::queryAllSchedule(QString key, QDateTime &beginTime, QDateTime &endTime)
{
    QVector<ScheduleDateRangeInfo> out {};
    m_dbus->QueryJobs(key, beginTime, endTime, out);
    return sortAndFilterSchedule(out);
}

QVector<ScheduleDtailInfo> queryScheduleProxy::sortAndFilterSchedule(QVector<ScheduleDateRangeInfo> &out)
{
    QVector<ScheduleDtailInfo> scheduleInfo {};
    for (int i = 0; i < out.size(); ++i) {
        for (int j = 0; j < out[i].vData.size(); ++j) {
            if (!(scheduleInfo.contains(out[i].vData[j]) || out[i].vData[j].type.ID == 4)) {
                scheduleInfo.append(out[i].vData[j]);
            }
        }
    }
    std::sort(scheduleInfo.begin(), scheduleInfo.end(), scheduleSort);
    return scheduleInfo;
}

QVector<ScheduleDtailInfo> queryScheduleProxy::WeeklyScheduleFileter(QVector<ScheduleDateRangeInfo> &out, QSet<int> &weeklySet)
{
    QVector<ScheduleDtailInfo> scheduleInfo {};
    if (weeklySet.size() == 0) {
        return sortAndFilterSchedule(out);
    } else {
        for (int i = 0; i < out.size(); ++i) {
            for (int j = 0; j < out[i].vData.size(); ++j) {
                if (!(scheduleInfo.contains(out[i].vData[j])) && weeklyIsIntersections(out[i].vData[j].beginDateTime, out[i].vData[j].endDateTime, weeklySet)) {
                    scheduleInfo.append(out[i].vData[j]);
                }
            }
        }
    }
    return scheduleInfo;
}

QVector<ScheduleDtailInfo> queryScheduleProxy::MonthlyScheduleFileter(QVector<ScheduleDateRangeInfo> &out, int beginM, int endM)
{
    if (beginM == 0 || endM == 0) {
        return sortAndFilterSchedule(out);
    }
    QVector<ScheduleDtailInfo> scheduleInfo {};
    for (int i = 0; i < out.size(); ++i) {
        for (int j = 0; j < out[i].vData.size(); ++j) {
            if (!(scheduleInfo.contains(out[i].vData[j])) && monthlyIsIntersections(out[i].vData[j].beginDateTime, out[i].vData[j].endDateTime, beginM, endM)) {
                scheduleInfo.append(out[i].vData[j]);
            }
        }
    }
    return scheduleInfo;
}

bool queryScheduleProxy::monthlyIsIntersections(QDateTime &beginTime, QDateTime &endTime, int beginM, int endM)
{
    bool b_monthly = false;
    int beginDay = beginTime.date().day();
    int endDay = endTime.date().day();
    if (beginM > endM) {
        b_monthly = beginDay > endM && endDay < beginM ? false : true;
    } else {
        b_monthly = (beginM <= beginDay && endM >= beginDay) || (beginM <= endDay && endM >= endDay)
                    || (beginDay <= beginM && endDay >= endM);
    }
    return b_monthly;
}

bool queryScheduleProxy::checkedTimeIsIntersection(QTime &beginTime, QTime &endTime, QTime &fixbeginTime, QTime &fixendTime)
{
    bool b_checked = false;
    if (beginTime.secsTo(endTime) < 0) {
        if (fixbeginTime.secsTo(fixendTime) < 0) {
            b_checked = true;
        } else {
            b_checked = endTime.secsTo(fixbeginTime) > 0 && fixendTime.secsTo(beginTime) > 0 ? false : true;
        }
    } else {
        if (fixbeginTime.secsTo(fixendTime) < 0) {
            b_checked = fixendTime.secsTo(beginTime) > 0 && endTime.secsTo(fixbeginTime) > 0 ? false : true;
        } else {
            b_checked = (beginTime.secsTo(fixbeginTime) >= 0 && fixbeginTime.secsTo(endTime) >= 0)
                        || (beginTime.secsTo(fixendTime) >= 0 && fixendTime.secsTo(endTime) >= 0);
        }
    }
    return b_checked;
}

QVector<ScheduleDtailInfo> queryScheduleProxy::scheduleFileterByTime(QVector<ScheduleDtailInfo> &scheduleInfo, QTime &fileterBeginTime, QTime &fileterEndTime)
{
    QVector<ScheduleDtailInfo> mScheduleFileter {};
    qint64 timeoffset_Secs = 0;
    bool isApppend = false;
    for (int i = 0; i < scheduleInfo.size(); ++i) {
        timeoffset_Secs = scheduleInfo.at(i).beginDateTime.secsTo(scheduleInfo.at(i).endDateTime);
        isApppend = false;
        if (timeoffset_Secs < ONE_DAY_SECS) {
            QTime begTime = scheduleInfo.at(i).beginDateTime.time();
            QTime endTime = scheduleInfo.at(i).endDateTime.time();
            isApppend = checkedTimeIsIntersection(begTime, endTime, fileterBeginTime, fileterEndTime);
        } else {
            isApppend = true;
        }
        if (isApppend) {
            mScheduleFileter.append(scheduleInfo.at(i));
        }
    }
    return mScheduleFileter;
}

bool queryScheduleProxy::weeklyIsIntersections(QDateTime &beginTime, QDateTime &endTime, QSet<int> &weeklySet)
{
    QSet<int> scheduleWeekSet;
    bool returnValue = false;
    scheduleWeekSet.clear();
    qint64 dayoffset = beginTime.daysTo(endTime);
    if (dayoffset > 5) {
        returnValue = true;
    } else {
        int beginW = beginTime.date().dayOfWeek();
        int w;
        for (int i = beginW; i <= beginW + dayoffset; ++i) {
            w = i % 7;
            w = w ? w : 7;
            scheduleWeekSet.insert(w);
        }
        returnValue = scheduleWeekSet.intersects(weeklySet);
    }
    return returnValue;
}

QVector<DateTimeInfo> queryScheduleProxy::getQueryDateTime(JsonData *jsonData)
{
    changejsondata *changedata = dynamic_cast<changejsondata *>(jsonData);
    if (changedata != nullptr && changedata->fromDateTime().size() > 0) {
        return changedata->fromDateTime();
    }
    return jsonData->getDateTime();
}
