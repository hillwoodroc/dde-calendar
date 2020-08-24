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
#include "repeatfeedbackstate.h"
#include "../globaldef.h"
#include "../task/schedulebasetask.h"
#include "../data/clocaldata.h"
#include "../data/changejsondata.h"

repeatfeedbackstate::repeatfeedbackstate(CSchedulesDBus *dbus, scheduleBaseTask *task)
    : scheduleState(dbus, task)
{
}

Reply repeatfeedbackstate::getReplyByIntent(bool isOK)
{
    if (isOK) {
        Q_UNUSED(isOK)
        Reply reply;
        REPLY_ONLY_TTS(reply, G_ERR_TTS, G_ERR_TTS, true)
        return reply;
    } else {
        return m_Task->InitState(nullptr);
    }
}

scheduleState::Filter_Flag repeatfeedbackstate::eventFilter(const JsonData *jsonData)
{
    if (jsonData->getPropertyStatus() == JsonData::ALL
        || jsonData->getPropertyStatus() == JsonData::PRO_THIS) {
        return Fileter_Normal;
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

    if (jsonData->getPropertyStatus() == JsonData::LAST
        || jsonData->offset() > 0) {
        return Fileter_Err;
    }
    return Fileter_Init;
}

Reply repeatfeedbackstate::ErrEvent()
{
    Reply reply;
    REPLY_ONLY_TTS(reply, G_ERR_TTS, G_ERR_TTS, true)
    return reply;
}

Reply repeatfeedbackstate::normalEvent(const JsonData *jsonData)
{
    bool isOnlyOne = true;
    if (jsonData->getPropertyStatus() == JsonData::ALL) {
        isOnlyOne = false;
    }
    return m_Task->repeatScheduleHandle(m_localData->SelectInfo(), isOnlyOne);
}
