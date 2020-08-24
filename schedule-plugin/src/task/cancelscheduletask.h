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
#ifndef CANCELSCHEDULETASK_H
#define CANCELSCHEDULETASK_H

#include "schedulebasetask.h"

class scheduleState;

class cancelScheduleTask : public scheduleBaseTask
{
    Q_OBJECT
public:
    cancelScheduleTask(CSchedulesDBus *dbus);
    Reply getFeedbackByQuerySchedule(const QVector<ScheduleDtailInfo> &infoVector) override;
    Reply getReplyBySelectSchedule(const ScheduleDtailInfo &info) override;
    Reply InitState(const JsonData *jsonData, bool isUpdateState = false) override;
    Reply repeatScheduleHandle(const ScheduleDtailInfo &info, bool isOnlyOne) override;
    Reply confirwScheduleHandle(const ScheduleDtailInfo &info) override;
    Reply confirmInfo(bool isOK) override;
public slots:
    void slotSelectScheduleIndex(int index);
    void slotButtonCheckNum(int index, const QString &text, const int buttonCount);

private:
    scheduleState *getCurrentState();
    QWidget *createRepeatWidget(const ScheduleDtailInfo &info);
    QWidget *createConfirmWidget(const ScheduleDtailInfo &info);

    Reply getListScheduleReply(const QVector<ScheduleDtailInfo> &infoVector);

    Reply getConfirwScheduleReply(const ScheduleDtailInfo &info);
    Reply getRepeatReply(const ScheduleDtailInfo &info);

    void deleteRepeatSchedule(const ScheduleDtailInfo &info, bool isOnlyOne = true);
    void deleteOrdinarySchedule(const ScheduleDtailInfo &info);

private:
};

#endif // CANCELSCHEDULETASK_H
