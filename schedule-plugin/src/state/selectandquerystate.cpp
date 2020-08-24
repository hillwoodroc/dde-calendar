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
#include "selectandquerystate.h"
#include "../globaldef.h"
#include "../task/schedulebasetask.h"
#include "../data/changejsondata.h"

SelectAndQueryState::SelectAndQueryState(CSchedulesDBus *dbus, scheduleBaseTask *task)
    : scheduleState(dbus, task)
{
}

Reply SelectAndQueryState::getReplyByIntent(bool isOK)
{
    Q_UNUSED(isOK)
    Reply reply;
    REPLY_ONLY_TTS(reply, G_ERR_TTS, G_ERR_TTS, true)
    return reply;
}

scheduleState::Filter_Flag SelectAndQueryState::eventFilter(const JsonData *jsonData)
{
    if (jsonData->getPropertyStatus() == JsonData::ALL
        || jsonData->getPropertyStatus() == JsonData::NEXT
        || jsonData->isVaild()
        || jsonData->getRepeatStatus() != JsonData::NONE) {
        return Filter_Flag::Fileter_Init;
    }
    if (jsonData->getPropertyStatus() == JsonData::LAST)
        return Fileter_Normal;
    JsonData *queryData = const_cast<JsonData *>(jsonData);
    changejsondata *mchangeJsonData = dynamic_cast<changejsondata *>(queryData);
    if (mchangeJsonData->toDateTime().size() > 0
        || !mchangeJsonData->toPlaceStr().isEmpty()
        || mchangeJsonData->fromDateTime().size() > 0) {
        return Filter_Flag::Fileter_Init;
    }
    if (m_localData->getOffet() < 0 && jsonData->offset() < 0) {
        return Fileter_Err;
    }
    bool showOpenWidget = m_localData->scheduleInfoVector().size() > ITEM_SHOW_NUM;
    const int showcount = showOpenWidget ? ITEM_SHOW_NUM : m_localData->scheduleInfoVector().size();
    if (jsonData->offset() > showcount) {
        return Fileter_Err;
    }
    return Fileter_Normal;
}

Reply SelectAndQueryState::ErrEvent()
{
    Reply reply;
    REPLY_ONLY_TTS(reply, QUERY_ERR_TTS, QUERY_ERR_TTS, true)
    return reply;
}

Reply SelectAndQueryState::normalEvent(const JsonData *jsonData)
{
    if (m_localData->getOffet() < 0 && jsonData->offset() > 0) {
        bool showOpenWidget = m_localData->scheduleInfoVector().size() > ITEM_SHOW_NUM;
        const int showcount = showOpenWidget ? ITEM_SHOW_NUM : m_localData->scheduleInfoVector().size();
        int offset = 0;
        if (jsonData->getPropertyStatus() == JsonData::LAST) {
            offset = showcount;
        } else {
            offset = jsonData->offset();
        }
        m_localData->setOffset(offset);
        m_localData->setSelectInfo(m_localData->scheduleInfoVector().at(offset - 1));
        Reply m_reply;
    }
    if (jsonData->DateTime().size() > 0)
        m_localData->setToTime(jsonData->DateTime());
    if (!jsonData->TitleName().isEmpty())
        m_localData->setToTitleName(jsonData->TitleName());

    ScheduleDtailInfo info = m_localData->SelectInfo();
    return m_Task->getReplyBySelectSchedule(info);
}
