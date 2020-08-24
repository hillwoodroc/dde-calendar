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
#ifndef CREATESCHEDULETASK_H
#define CREATESCHEDULETASK_H

#include "schedulebasetask.h"
#include "../widget/createschedulewidget.h"

class createScheduleTask : public scheduleBaseTask
{
    Q_OBJECT
public:
    createScheduleTask(CSchedulesDBus *dbus);
    Reply SchedulePress(semanticAnalysisTask &semanticTask) override;

private:
    void setWeekBehindPartSchedule(QDateTime begintime, QDateTime endtime, int isSameWeek, int startDay, int endDay, createSchedulewidget *m_widget);
    void setMonthBehindPartSchedule(QDateTime begintime, QDateTime endtime, int startDay, int endDay, bool nextMonth, createSchedulewidget *m_widget);
};

#endif // CREATESCHEDULETASK_H
