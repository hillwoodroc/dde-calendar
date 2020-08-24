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
#ifndef SCHEDULESERVICE_H
#define SCHEDULESERVICE_H

#include "interface/service.h"

class ScheduleManageTask;
class scheduleservice : public IService
{
    Q_OBJECT
public:
    scheduleservice();
    ~scheduleservice() override;
    QString serviceName() override;
    int servicePriority() override;
    bool canHandle(const QString &s) override;
    IRet service(const QString &semantic) override;
    Reply &getServiceReply() override;
    ScheduleManageTask *getScheduleManageTask() const;

private:
    Reply m_reply;
};

#endif // SCHEDULESERVICE_H
