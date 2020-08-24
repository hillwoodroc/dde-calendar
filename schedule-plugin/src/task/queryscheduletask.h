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
#ifndef QUERYSCHEDULETASK_H
#define QUERYSCHEDULETASK_H

#include "schedulebasetask.h"

class queryScheduleTask : public scheduleBaseTask
{
    Q_OBJECT
public:
    queryScheduleTask(CSchedulesDBus *dbus);
    Reply SchedulePress(semanticAnalysisTask &semanticTask) override;

protected:
#ifdef LOG_PRINT
    void logPrint(const JsonData *jsonData) override;
#endif
};

#endif // QUERYSCHEDULETASK_H
