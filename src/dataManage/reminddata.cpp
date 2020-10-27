/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chenhaifeng  <chenhaifeng@uniontech.com>
*
* Maintainer: chenhaifeng  <chenhaifeng@uniontech.com>
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
#include "reminddata.h"

RemindData::RemindData()
    :m_RemindNum(-1)
    ,m_RemindTime(9,0,0)
{
    
}

void RemindData::setRemindNum(const int num)
{
    m_RemindNum = num;
}

int RemindData::getRemindNum() const
{
    return m_RemindNum;
}

void RemindData::setRemindTime(const QTime &remindTime)
{
    m_RemindTime = remindTime;
}

QTime RemindData::getRemindTime() const
{
    return  m_RemindTime;
}

RemindData RemindData::RemindStringToRemindData(const QString &remindStr)
{
    RemindData _restuleRemindData;
    if(!remindStr.isEmpty()){
        QStringList liststr = remindStr.split(";", QString::SkipEmptyParts);
        _restuleRemindData.setRemindNum(liststr.at(0).toInt());
        if(liststr.size()>1){
            _restuleRemindData.setRemindTime(QTime::fromString(liststr.at(1), "hh:mm"));
        }
    }
    return _restuleRemindData;
}

QString RemindData::RemindDataToRemindString(const RemindData &remindData, const bool isAllDay)
{
    QString _restuleStr{""};
    if(remindData.getRemindNum() != -1){
        _restuleStr += QString::number(remindData.getRemindNum());
        if(isAllDay){
            _restuleStr +=QString(";%1").arg(remindData.getRemindTime().toString("hh:mm"));
        }
    }
    return _restuleStr;
}
