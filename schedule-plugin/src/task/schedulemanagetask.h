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
#ifndef SCHEDULEMANAGETASK_H
#define SCHEDULEMANAGETASK_H

#include <QObject>

#include "../interface/reply.h"
#include "semanticanalysistask.h"
#include "../dbus/schedulesdbus.h"
#include "../widget/icondframe.h"
#include "../data/schedulestructs.h"
#include "../widget/createschedulewidget.h"
#include "../interface/reply.h"
#include "schedulebasetask.h"

//class widgetStrategy;
class CSchedulesDBus;
class ScheduleManageTask : public QObject
{
    Q_OBJECT
public:
    static ScheduleManageTask *getInstance();
    static void releaseInstance();

    void process(semanticAnalysisTask &semanticTask);

    Reply getReply() const;
    void setReply(const Reply &Reply);

private:
    explicit ScheduleManageTask(QObject *parent = nullptr);
    ~ScheduleManageTask();
signals:
    void signaleSendMessage(Reply reply);
public slots:
    void slotReceivce(QVariant data, Reply *reply);

private:
    static ScheduleManageTask *m_scheduleManageTask;

private:
    CSchedulesDBus *m_dbus {nullptr};
    QMap<QString, scheduleBaseTask *> m_scheduleTaskMap;
    Reply m_Reply;
    scheduleBaseTask *m_preScheduleTask {nullptr};
};

#endif // SCHEDULEMANAGETASK_H
