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
#include "jsondata.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonDocument>

#include "../globaldef.h"

JsonData::JsonData()
{
}

JsonData::~JsonData()
{
}

void JsonData::JosnResolve(const QJsonObject &jsObj)
{
    setDefaultValue();
    if (jsObj.contains("slots") && jsObj["slots"].isArray()) {
        auto slotsObjArr = jsObj["slots"].toArray();
        for (int j = 0; j < slotsObjArr.size(); ++j) {
            auto slotsObj = slotsObjArr[j].toObject();
            if (slotsObj.contains(JSON_NAME) && slotsObj[JSON_NAME].isString()) {
                if (slotsObj[JSON_NAME] == JSON_CONTENT) {
                    contentJsonResolve(slotsObj);
                    continue;
                }
                if (slotsObj[JSON_NAME] == JSON_REPEAT) {
                    repeatJsonResolve(slotsObj);
                    continue;
                }
                if (slotsObj[JSON_NAME] == JSON_DATETIME) {
                    datetimeJsonResolve(slotsObj);
                    continue;
                }
                if (slotsObj[JSON_NAME] == JSON_PROPERTY) {
                    propertyJsonResolve(slotsObj);
                    continue;
                }
                if (slotsObj[JSON_NAME] == JSON_POSRANK_OFFSET) {
                    posRankOffsetResolve(slotsObj);
                    continue;
                }
                jsonObjResolve(slotsObj);
            }
        }
    }
}

QString JsonData::SuggestMsg() const
{
    return m_SuggestMsg;
}

void JsonData::setSuggestMsg(const QString &SuggestMsg)
{
    m_SuggestMsg = SuggestMsg;
}

bool JsonData::ShouldEndSession() const
{
    return m_ShouldEndSession;
}

void JsonData::setShouldEndSession(bool ShouldEndSession)
{
    m_ShouldEndSession = ShouldEndSession;
}

void JsonData::setRepeatStatus(const JsonData::RepeatStatus &RepeatStatus)
{
    m_RepeatStatus = RepeatStatus;
}

QString JsonData::TitleName() const
{
    return m_TitleName;
}

void JsonData::setTitleName(const QString &TitleName)
{
    m_TitleName = TitleName;
}

void JsonData::setDateTimeStatus(const JsonData::DateTimeStatus &DateTimeStatus)
{
    m_DateTimeStatus = DateTimeStatus;
}

QVector<DateTimeInfo> JsonData::DateTime() const
{
    return m_DateTime;
}

JsonData::RepeatStatus JsonData::getRepeatStatus() const
{
    return m_RepeatStatus;
}

QVector<int> JsonData::getRepeatNum() const
{
    return m_RepeatNum;
}

void JsonData::setRepeatNum(const QVector<int> &RepeatNum)
{
    m_RepeatNum = RepeatNum;
}

QVector<DateTimeInfo> JsonData::getDateTime() const
{
    return m_DateTime;
}

void JsonData::setDateTime(const QVector<DateTimeInfo> &DateTime)
{
    m_DateTime = DateTime;
}

void JsonData::contentJsonResolve(const QJsonObject &jsobj)
{
    setTitleName(jsobj[JSON_VALUE].toString());
}

void JsonData::repeatJsonResolve(const QJsonObject &jsobj)
{
    QString repeatValue = jsobj[JSON_VALUE].toString();
    if (repeatValue.contains("EVERYEAR")) {
        setRepeatStatus(EVEY);
        return;
    }
    if (repeatValue.contains("EVERYDAY")) {
        setRepeatStatus(EVED);
        return;
    }
    if (repeatValue.contains("WORKDAY")) {
        setRepeatStatus(WORKD);
        return;
    }
    if (repeatValue.contains("RESTDAY")) {
        setRepeatStatus(RESTD);
        return;
    }
    if (repeatValue.contains("WEEKEND")) {
        setRepeatStatus(RESTD);
        return;
    }

    QRegExp rxlen("([W,M])([0-9]{0,2})");
    int pos = 0;
    QVector<int> repeatnum {};
    repeatnum.clear();
    while ((pos = rxlen.indexIn(repeatValue, pos)) != -1) {
        if (rxlen.cap(1).contains("M")) {
            setRepeatStatus(EVEM);
        }
        if (rxlen.cap(1).contains("W")) {
            setRepeatStatus(EVEW);
        }
        if (rxlen.cap(0).size() > 0 && rxlen.cap(2) != "") {
            repeatnum.append(rxlen.cap(2).toInt());
        }
        pos += rxlen.matchedLength();
    }
    setRepeatNum(repeatnum);
}

void JsonData::datetimeJsonResolve(const QJsonObject &jsobj)
{
    setDateTime(suggestDatetimeResolve(jsobj));
}

void JsonData::propertyJsonResolve(const QJsonObject &jsobj)
{
    QString propertyValue = jsobj[JSON_VALUE].toString();
    if (propertyValue == JSON_NEXT) {
        setPropertyStatus(NEXT);
        return;
    }
    if (propertyValue == JSON_LAST) {
        setPropertyStatus(LAST);
        return;
    }
    if (propertyValue == JSON_ALL) {
        setPropertyStatus(ALL);
        return;
    }
    if (propertyValue == JSON_THIS) {
        setPropertyStatus(PRO_THIS);
        return;
    }
}

void JsonData::posRankOffsetResolve(const QJsonObject &jsobj)
{
    setOffset(jsobj[JSON_VALUE].toString().toInt());
}

QVector<DateTimeInfo> JsonData::suggestDatetimeResolve(const QJsonObject &jsobj)
{
    QVector<DateTimeInfo> datetime {};
    datetime.clear();
    QString DateTimeJson = jsobj[JSON_NORMVALUE].toString();
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(DateTimeJson.toUtf8(), &jsonError);
    QString dateTime;

    if (!doc.isNull() && (jsonError.error == QJsonParseError::NoError)) {
        auto rootObject = doc.object();
        dateTime = (rootObject["suggestDatetime"].toString());
    }
    if (dateTime != "") {
        if (dateTime.contains("/")) {
            QStringList dateTimeList = dateTime.split("/");
            for (int i = 0; i < dateTimeList.size(); ++i) {
                datetime.append(resolveNormValue(dateTimeList.at(i)));
            }
        } else {
            datetime.append(resolveNormValue(dateTime));
        }
    }
    return datetime;
}

DateTimeInfo JsonData::resolveNormValue(const QString &str)
{
    DateTimeInfo datetimeInfo;
    datetimeInfo.hasTime = str.contains(JSON_DATETIME_DELIMITER);
    datetimeInfo.datetime = QDateTime::fromString(str, Qt::ISODate);
    return datetimeInfo;
}

void JsonData::setDefaultValue()
{
    setTitleName("");
    setDateTimeStatus(NODATE);
    setRepeatStatus(NONE);
    setPropertyStatus(PRO_NONE);
    m_RepeatNum.clear();
    m_DateTime.clear();
    setOffset(-1);
}

void JsonData::jsonObjResolve(const QJsonObject &jsobj)
{
    Q_UNUSED(jsobj);
}

JsonData::PropertyStatus JsonData::getPropertyStatus() const
{
    return m_propertyStatus;
}

void JsonData::setPropertyStatus(const PropertyStatus &propertyStatus)
{
    m_propertyStatus = propertyStatus;
}

int JsonData::offset() const
{
    return m_offset;
}

void JsonData::setOffset(int offset)
{
    m_offset = offset;
}

bool JsonData::isVaild() const
{
    return TitleName() == "" && getDateTime().size() == 0
           && m_offset == -1 && m_RepeatStatus == NONE && m_propertyStatus == PRO_NONE;
    //    return  m_isVaild;
}
