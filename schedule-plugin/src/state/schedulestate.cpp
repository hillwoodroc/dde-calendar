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
#include "schedulestate.h"

#include "../task/schedulebasetask.h"

scheduleState::scheduleState(CSchedulesDBus *dbus, scheduleBaseTask *task)
    : m_dbus(dbus)
    , m_Task(task)
{
}

scheduleState::~scheduleState()
{
    if (m_localData != nullptr)
        delete m_localData;
}

Reply scheduleState::process(const JsonData *jsonData)
{
    Reply reply;
    switch (eventFilter(jsonData)) {
    case Fileter_Err: {
        reply = ErrEvent();
    } break;
    case Fileter_Normal: {
        reply = normalEvent(jsonData);
    } break;
    case Fileter_Init: {
        reply = initEvent(jsonData);
    } break;
    }
    return reply;
}

void scheduleState::setNextState(scheduleState *nextState)
{
    m_nextState = nextState;
}

scheduleState *scheduleState::getNextState() const
{
    return m_nextState;
}

void scheduleState::setLocalData(CLocalData *localData)
{
    if (m_localData == localData) {
        return;
    }
    if (m_localData != nullptr) {
        delete m_localData;
        m_localData = nullptr;
    }
    if (localData == nullptr)
        return;
    m_localData = localData;
}

CLocalData *scheduleState::getLocalData() const
{
    return m_localData;
}

Reply scheduleState::initEvent(const JsonData *jsonData)
{
    return m_Task->InitState(jsonData);
}
