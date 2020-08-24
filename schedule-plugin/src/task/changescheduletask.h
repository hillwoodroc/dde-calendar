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
#ifndef CHANGESCHEDULETASK_H
#define CHANGESCHEDULETASK_H

#include "schedulebasetask.h"

class changeScheduleTask : public scheduleBaseTask
{
    Q_OBJECT
public:
    changeScheduleTask(CSchedulesDBus *dbus);
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
    QWidget *createInquiryWidget(const ScheduleDtailInfo &info);

    Reply getListScheduleReply(const QVector<ScheduleDtailInfo> &infoVector);

    Reply getConfirwScheduleReply(const ScheduleDtailInfo &info);
    Reply getRepeatReply(const ScheduleDtailInfo &info);
    void getNewInfo();

    void changeRepeatSchedule(const ScheduleDtailInfo &info, bool isOnlyOne);
    void changeOnlyInfo(const ScheduleDtailInfo &info);
    void changeAllInfo(const ScheduleDtailInfo &info);
    void changeOrdinarySchedule(const ScheduleDtailInfo &info);

private:
    QVector<ScheduleDtailInfo> m_scheduleInfo;
};

#endif // CHANGESCHEDULETASK_H
