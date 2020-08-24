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
#ifndef CLOCALDATA_H
#define CLOCALDATA_H

#include "schedulestructs.h"
#include "timedatastruct.h"
class CLocalData
{
public:
    CLocalData();
    QVector<ScheduleDtailInfo> scheduleInfoVector() const;
    void setScheduleInfoVector(const QVector<ScheduleDtailInfo> &scheduleInfoVector);
    ScheduleDtailInfo SelectInfo() const;
    void setSelectInfo(const ScheduleDtailInfo &SelectInfo);
    void setToTitleName(const QString &title);
    QString getToTitleName() const;
    void setToTime(const QVector<DateTimeInfo> &timeInfo);
    QVector<DateTimeInfo> getToTime() const;
    int getOffet() const;
    void setOffset(int offset);
    CLocalData *getDataByPoint(const CLocalData *localData);
    void setNewInfo(const ScheduleDtailInfo &newInfo);
    ScheduleDtailInfo getNewInfo() const;

private:
    QVector<ScheduleDtailInfo> m_scheduleInfoVector {};
    ScheduleDtailInfo m_SelectInfo {};
    ScheduleDtailInfo m_NewInfo {};
    QString m_ToTitleName {""};
    QVector<DateTimeInfo> m_ToTime {};
    int m_offset {-1};
};

#endif // CLOCALDATA_H
