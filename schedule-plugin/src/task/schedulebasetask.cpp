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
#include "schedulebasetask.h"
#include "../state/schedulestate.h"
#include "../globaldef.h"

scheduleBaseTask::scheduleBaseTask(CSchedulesDBus *dbus, scheduleState *state)
    : QObject()
    , m_dbus(dbus)
    , m_State(state)
{
}

scheduleBaseTask::~scheduleBaseTask()
{
    if (m_State == nullptr) {
        delete m_State;
        m_State = nullptr;
    }
}

Reply scheduleBaseTask::SchedulePress(semanticAnalysisTask &semanticTask)
{
#ifdef LOG_PRINT
    logPrint(semanticTask.getJsonData());
#endif
    Reply reply;
    if (m_State != nullptr) {
        if (semanticTask.Intent() == JSON_NO_INTENT) {
            reply = m_State->getReplyByIntent(false);
        } else if (semanticTask.Intent() == JSON_YES_INTENT) {
            reply = m_State->getReplyByIntent(true);
        } else {
            reply = m_State->process(semanticTask.getJsonData());
        }
        updateState();
    }
    return reply;
}

Reply scheduleBaseTask::getFeedbackByQuerySchedule(const QVector<ScheduleDtailInfo> &info)
{
    Q_UNUSED(info);
    return Reply();
}

Reply scheduleBaseTask::getReplyBySelectSchedule(const ScheduleDtailInfo &info)
{
    Q_UNUSED(info);
    return Reply();
}

Reply scheduleBaseTask::InitState(const JsonData *jsonData, bool isUpdateState)
{
    Q_UNUSED(jsonData);
    Q_UNUSED(isUpdateState);
    return Reply();
}

Reply scheduleBaseTask::repeatScheduleHandle(const ScheduleDtailInfo &info, bool isOnlyOne)
{
    Q_UNUSED(info);
    Q_UNUSED(isOnlyOne);
    return Reply();
}

Reply scheduleBaseTask::confirwScheduleHandle(const ScheduleDtailInfo &info)
{
    Q_UNUSED(info);
    return Reply();
}

Reply scheduleBaseTask::confirmInfo(bool isOK)
{
    Q_UNUSED(isOK);
    return Reply();
}

scheduleState *scheduleBaseTask::getState() const
{
    return m_State;
}

void scheduleBaseTask::updateState()
{
    do {
        scheduleState *nextState = m_State->getNextState();
        if (nextState != nullptr) {
            delete m_State;
            m_State = nextState;
        } else {
            break;
        }
    } while (1);
}
