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
#ifndef QUERYSCHEDULEPROXY_H
#define QUERYSCHEDULEPROXY_H

#include "../data/schedulestructs.h"
#include "../data/jsondata.h"
#include "../dbus/schedulesdbus.h"

class queryScheduleProxy
{
public:
    queryScheduleProxy(JsonData *jsonData, CSchedulesDBus *dbus);
    QVector<ScheduleDtailInfo> querySchedule();

private:
    QVector<ScheduleDtailInfo> queryWeeklySchedule(QDateTime &beginTime, QDateTime &endTime, int beginW = 0, int endW = 0);
    QVector<ScheduleDtailInfo> queryMonthlySchedule(QDateTime &beginTime, QDateTime &endTime, int beginM = 0, int endM = 0);
    QVector<ScheduleDtailInfo> queryEveryDaySchedule(QDateTime &beginTime, QDateTime &endTime);
    QVector<ScheduleDtailInfo> queryEveryYearSchedule(QDateTime &beginTime, QDateTime &endTime);
    QVector<ScheduleDtailInfo> queryWorkingDaySchedule(QDateTime &beginTime, QDateTime &endTime);
    QVector<ScheduleDtailInfo> queryNonRepeatingSchedule();
    QVector<ScheduleDtailInfo> queryNextNumSchedule(QDateTime &beginTime, QDateTime &endTime, int NextNum);
    QVector<ScheduleDtailInfo> queryAllSchedule(QString key, QDateTime &beginTime, QDateTime &endTime);

    QVector<ScheduleDtailInfo> sortAndFilterSchedule(QVector<ScheduleDateRangeInfo> &out);
    QVector<ScheduleDtailInfo> WeeklyScheduleFileter(QVector<ScheduleDateRangeInfo> &out, QSet<int> &weeklySet);
    QVector<ScheduleDtailInfo> MonthlyScheduleFileter(QVector<ScheduleDateRangeInfo> &out, int beginM = 0, int endM = 0);
    bool monthlyIsIntersections(QDateTime &beginTime, QDateTime &endTime, int beginM = 0, int endM = 0);

    bool checkedTimeIsIntersection(QTime &beginTime, QTime &endTime, QTime &fixbeginTime, QTime &fixendTime);

    QVector<ScheduleDtailInfo> scheduleFileterByTime(QVector<ScheduleDtailInfo> &scheduleInfo, QTime &fileterBeginTime, QTime &fileterEndTime);
    bool weeklyIsIntersections(QDateTime &beginTime, QDateTime &endTime, QSet<int> &weeklySet);

    QVector<DateTimeInfo> getQueryDateTime(JsonData *jsonData);

private:
    JsonData *m_queryJsonData;
    CSchedulesDBus *m_dbus {nullptr};
};

#endif // QUERYSCHEDULEPROXY_H
