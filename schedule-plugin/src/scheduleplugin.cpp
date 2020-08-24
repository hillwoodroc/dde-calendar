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
#include "scheduleplugin.h"

#include "scheduleservice.h"
#include "globaldef.h"
#include "task/schedulemanagetask.h"

Scheduleplugin::Scheduleplugin(QObject *parent)
{
    Q_UNUSED(parent);
}

Scheduleplugin::~Scheduleplugin()
{
    ScheduleManageTask::releaseInstance();
}

int Scheduleplugin::init()
{
    return 0;
}

void Scheduleplugin::uninit()
{
}

QStringList Scheduleplugin::getSupportService()
{
    return QStringList {SERVICE_NAME};
}

IService *Scheduleplugin::createService(const QString &service)
{
    if (!getSupportService().contains(service)) {
        return nullptr;
    }
    QMutexLocker lock(&m_ServiceLock);
    auto pService = new scheduleservice();
    connect(pService->getScheduleManageTask(), &ScheduleManageTask::signaleSendMessage,
            this, &Scheduleplugin::slotSendMessage, Qt::UniqueConnection);
    m_ServiceSet.insert(pService);
    return pService;
}

void Scheduleplugin::releaseService(IService *service)
{
    QMutexLocker lock(&m_ServiceLock);
    for (auto iter = m_ServiceSet.begin(); iter != m_ServiceSet.end(); iter++) {
        if (service && service == *iter) {
            m_ServiceSet.erase(iter);
            break;
        }
    }
    if (service) {
        delete service;
    }
}

bool Scheduleplugin::needRunInDifferentThread()
{
    return false;
}

bool Scheduleplugin::isAutoDetectSession()
{
    return true;
}

void Scheduleplugin::slotSendMessage(Reply reply)
{
    if (this->m_messageHandle)
        this->m_messageHandle(this, reply);
}
