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
#include "confirwfeedbackstate.h"

#include "../globaldef.h"
#include "../task/schedulebasetask.h"
#include "../data/clocaldata.h"
#include "../data/changejsondata.h"

confirwFeedbackState::confirwFeedbackState(CSchedulesDBus *dbus, scheduleBaseTask *task)
    : scheduleState(dbus, task)
{
}

Reply confirwFeedbackState::getReplyByIntent(bool isOK)
{
    return m_Task->confirmInfo(isOK);
}

scheduleState::Filter_Flag confirwFeedbackState::eventFilter(const JsonData *jsonData)
{
    if (jsonData->getPropertyStatus() != JsonData::PRO_NONE
        || jsonData->offset() > 0) {
        return Fileter_Err;
    }
    if (jsonData->getDateTime().size() > 0) {
        return Fileter_Init;
    }
    JsonData *queryData = const_cast<JsonData *>(jsonData);
    changejsondata *mchangeJsonData = dynamic_cast<changejsondata *>(queryData);
    if (mchangeJsonData != nullptr) {
        if (mchangeJsonData->toDateTime().size() > 0
            || !mchangeJsonData->toPlaceStr().isEmpty()
            || mchangeJsonData->fromDateTime().size() > 0) {
            return Filter_Flag::Fileter_Init;
        }
    }
    return Fileter_Normal;
}

Reply confirwFeedbackState::ErrEvent()
{
    Reply reply;
    REPLY_ONLY_TTS(reply, G_ERR_TTS, G_ERR_TTS, true)
    return reply;
}

Reply confirwFeedbackState::normalEvent(const JsonData *jsonData)
{
    Q_UNUSED(jsonData);
    return m_Task->confirwScheduleHandle(m_localData->SelectInfo());
}
