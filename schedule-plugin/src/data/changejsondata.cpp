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
#include "changejsondata.h"

#include "../globaldef.h"

#include <QJsonObject>

changejsondata::changejsondata()
    : JsonData()
{
}

void changejsondata::jsonObjResolve(const QJsonObject &jsobj)
{
    if (jsobj[JSON_NAME] == JSON_FROMDATETIME) {
        fromDatetimeJsonResolve(jsobj);
        return;
    }
    if (jsobj[JSON_NAME] == JSON_TODATETIME) {
        toDatetimeJsonResolve(jsobj);
        return;
    }
    if (jsobj[JSON_NAME] == JSON_TOPLACE) {
        toPlaceJsonResolve(jsobj);
        return;
    }
}

void changejsondata::setDefaultValue()
{
    JsonData::setDefaultValue();
    m_fromDateTime.clear();
    m_toDateTime.clear();
    m_toPlaceStr.clear();
}

void changejsondata::fromDatetimeJsonResolve(const QJsonObject &jsobj)
{
    setFromDateTime(suggestDatetimeResolve(jsobj));
}

void changejsondata::toDatetimeJsonResolve(const QJsonObject &jsobj)
{
    setToDateTime(suggestDatetimeResolve(jsobj));
}

void changejsondata::toPlaceJsonResolve(const QJsonObject &jsobj)
{
    setToPlaceStr(jsobj[JSON_VALUE].toString());
}

QString changejsondata::toPlaceStr() const
{
    return m_toPlaceStr;
}

void changejsondata::setToPlaceStr(const QString &toPlaceStr)
{
    m_toPlaceStr = toPlaceStr;
}

QVector<DateTimeInfo> changejsondata::toDateTime() const
{
    return m_toDateTime;
}

void changejsondata::setToDateTime(const QVector<DateTimeInfo> &toDateTime)
{
    m_toDateTime = toDateTime;
}

QVector<DateTimeInfo> changejsondata::fromDateTime() const
{
    return m_fromDateTime;
}

void changejsondata::setFromDateTime(const QVector<DateTimeInfo> &fromDateTime)
{
    m_fromDateTime = fromDateTime;
}
