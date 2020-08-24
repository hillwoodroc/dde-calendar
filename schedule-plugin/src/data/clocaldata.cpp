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
#include "clocaldata.h"

CLocalData::CLocalData()
{
}

QVector<ScheduleDtailInfo> CLocalData::scheduleInfoVector() const
{
    return m_scheduleInfoVector;
}

void CLocalData::setScheduleInfoVector(const QVector<ScheduleDtailInfo> &scheduleInfoVector)
{
    m_scheduleInfoVector = scheduleInfoVector;
}

ScheduleDtailInfo CLocalData::SelectInfo() const
{
    return m_SelectInfo;
}

void CLocalData::setSelectInfo(const ScheduleDtailInfo &SelectInfo)
{
    m_SelectInfo = SelectInfo;
}

void CLocalData::setToTitleName(const QString &title)
{
    m_ToTitleName = title;
}

QString CLocalData::getToTitleName() const
{
    return m_ToTitleName;
}

void CLocalData::setToTime(const QVector<DateTimeInfo> &timeInfo)
{
    m_ToTime = timeInfo;
}

QVector<DateTimeInfo> CLocalData::getToTime() const
{
    return m_ToTime;
}

int CLocalData::getOffet() const
{
    return m_offset;
}

void CLocalData::setOffset(int offset)
{
    m_offset = offset;
}

CLocalData *CLocalData::getDataByPoint(const CLocalData *localData)
{
    if (this == localData || localData == nullptr)
        return this;
    this->m_offset = localData->m_offset;
    this->m_SelectInfo = localData->m_SelectInfo;
    this->m_ToTime = localData->m_ToTime;
    this->m_scheduleInfoVector = localData->m_scheduleInfoVector;
    this->m_ToTitleName = localData->m_ToTitleName;
    return this;
}

void CLocalData::setNewInfo(const ScheduleDtailInfo &newInfo)
{
    m_NewInfo = newInfo;
}

ScheduleDtailInfo CLocalData::getNewInfo() const
{
    return m_NewInfo;
}
