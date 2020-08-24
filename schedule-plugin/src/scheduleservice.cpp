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
#include "scheduleservice.h"

#include "globaldef.h"
#include "task/schedulemanagetask.h"
#include "task/semanticanalysistask.h"

scheduleservice::scheduleservice()
{
}

scheduleservice::~scheduleservice()
{
}

QString scheduleservice::serviceName()
{
    return SERVICE_NAME;
}

int scheduleservice::servicePriority()
{
    return 0;
}

bool scheduleservice::canHandle(const QString &s)
{
    Q_UNUSED(s);
    return true;
}

IRet scheduleservice::service(const QString &semantic)
{
    //解析云端返回的数据,进行业务处理
    QString str = semantic;
    semanticAnalysisTask task;
    if (!task.resolveTaskJson(str)) {
        return ERR_FAIL;
    }
    ScheduleManageTask::getInstance()->process(task);
    return ERR_SUCCESS;
}

Reply &scheduleservice::getServiceReply()
{
    m_reply = ScheduleManageTask::getInstance()->getReply();
    return m_reply;
}

ScheduleManageTask *scheduleservice::getScheduleManageTask() const
{
    return ScheduleManageTask::getInstance();
}
