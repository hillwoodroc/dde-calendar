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
#ifndef SCHEDULEBASETASK_H
#define SCHEDULEBASETASK_H

#include "semanticanalysistask.h"
#include "../interface/reply.h"
#include "../dbus/schedulesdbus.h"
#include "../data/schedulestructs.h"
#include <QObject>

class scheduleState;
class scheduleBaseTask : public QObject
{
    Q_OBJECT
public:
    scheduleBaseTask(CSchedulesDBus *dbus, scheduleState *state = nullptr);
    virtual ~scheduleBaseTask();
    virtual Reply SchedulePress(semanticAnalysisTask &semanticTask);
    virtual Reply getFeedbackByQuerySchedule(const QVector<ScheduleDtailInfo> &info);
    virtual Reply getReplyBySelectSchedule(const ScheduleDtailInfo &info);
    virtual Reply InitState(const JsonData *jsonData, bool isUpdateState = false);
    virtual Reply repeatScheduleHandle(const ScheduleDtailInfo &info, bool isOnlyOne);
    virtual Reply confirwScheduleHandle(const ScheduleDtailInfo &info);
    virtual Reply confirmInfo(bool isOK);
    scheduleState *getState() const;
signals:
    void signaleSendMessage(Reply reply);

protected:
#ifdef LOG_PRINT
    virtual void logPrint(const JsonData *jsonData) = 0;
#endif
    void updateState();

protected:
    CSchedulesDBus *m_dbus {nullptr};
    scheduleState *m_State {nullptr};
};

#endif // SCHEDULEBASETASK_H
