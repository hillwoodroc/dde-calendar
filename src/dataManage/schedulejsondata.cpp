/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chenhaifeng  <uniochenhaifengntech@uniontech.com>
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
#include "schedulejsondata.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

ScheduleJsonData::ScheduleJsonData() 
    :m_ScheduleID(0)
    ,m_ScheduleBeginDateTime()
    ,m_ScheduleEndDateTime()
    ,m_IgnoreVectorTime()
    ,m_ScheudleTitleName("")
    ,m_ScheudleDescription("")
    ,m_ScheduleIsAllDay(true)
    ,m_ScheudleType(1)
    ,m_ScheudleRecurID(0)
    ,m_ScheudleRemindStr("")
    ,m_ScheduleRRuleStr("")
    ,m_ScheduleRRule()
{
    
}

ScheduleJsonData::ScheduleJsonData(const ScheduleJsonData &scheduleJsonData)
{
    this->setScheduleID(scheduleJsonData.ScheduleID());
    this->setScheudleType(scheduleJsonData.ScheudleType());
    this->setScheduleRRule(scheduleJsonData.ScheduleRRule());
    this->setScheudleRemind(scheduleJsonData.ScheudleRemind());
    this->setScheudleRecurID(scheduleJsonData.ScheudleRecurID());
    this->setScheduleIsAllDay(scheduleJsonData.ScheduleIsAllDay());
    this->setScheudleTitleName(scheduleJsonData.ScheudleTitleName());
    this->setScheduleEndDateTime(scheduleJsonData.ScheduleEndDateTime());
    this->setScheudleDescription(scheduleJsonData.ScheudleDescription());
    this->setScheduleBeginDateTime(scheduleJsonData.ScheduleBeginDateTime());
    this->setIgnoreVectorTime(scheduleJsonData.IgnoreVectorTime());
}

int ScheduleJsonData::ScheduleID() const
{
    return m_ScheduleID;
}

void ScheduleJsonData::setScheduleID(int ScheduleID)
{
    m_ScheduleID = ScheduleID;
}

QDateTime ScheduleJsonData::ScheduleBeginDateTime() const
{
    return m_ScheduleBeginDateTime;
}

void ScheduleJsonData::setScheduleBeginDateTime(const QDateTime &ScheduleBeginDateTime)
{
    m_ScheduleBeginDateTime = ScheduleBeginDateTime;
}

QDateTime ScheduleJsonData::ScheduleEndDateTime() const
{
    return m_ScheduleEndDateTime;
}

void ScheduleJsonData::setScheduleEndDateTime(const QDateTime &ScheduleEndDateTime)
{
    m_ScheduleEndDateTime = ScheduleEndDateTime;
}

QVector<QDateTime> ScheduleJsonData::IgnoreVectorTime() const
{
    return m_IgnoreVectorTime;
}

void ScheduleJsonData::setIgnoreVectorTime(const QVector<QDateTime> &IgnoreVectorTime)
{
    m_IgnoreVectorTime = IgnoreVectorTime;
}

QString ScheduleJsonData::ScheudleTitleName() const
{
    return m_ScheudleTitleName;
}

void ScheduleJsonData::setScheudleTitleName(const QString &ScheudleTitleName)
{
    m_ScheudleTitleName = ScheudleTitleName;
}

QString ScheduleJsonData::ScheudleDescription() const
{
    return m_ScheudleDescription;
}

void ScheduleJsonData::setScheudleDescription(const QString &ScheudleDescription)
{
    m_ScheudleDescription = ScheudleDescription;
}

bool ScheduleJsonData::ScheduleIsAllDay() const
{
    return m_ScheduleIsAllDay;
}

void ScheduleJsonData::setScheduleIsAllDay(bool ScheduleIsAllDay)
{
    m_ScheduleIsAllDay = ScheduleIsAllDay;
}

int ScheduleJsonData::ScheudleType() const
{
    return m_ScheudleType;
}

void ScheduleJsonData::setScheudleType(int ScheudleType)
{
    m_ScheudleType = ScheudleType;
}

int ScheduleJsonData::ScheudleRecurID() const
{
    return m_ScheudleRecurID;
}

void ScheduleJsonData::setScheudleRecurID(int ScheudleRecurID)
{
    m_ScheudleRecurID = ScheudleRecurID;
}

void ScheduleJsonData::setScheduleRemindData(const RemindData &remindData)
{
    m_ScheduleRemind = remindData;
    m_ScheduleRRuleStr = RemindData::RemindDataToRemindString(m_ScheduleRemind,this->ScheduleIsAllDay());
}

RemindData ScheduleJsonData::getScheduleRemindData() const
{
    return m_ScheduleRemind;
}

QString ScheduleJsonData::ScheudleRemind() const
{
    return m_ScheudleRemindStr;
}

void ScheduleJsonData::setScheudleRemind(const QString &ScheudleRemind)
{
    m_ScheudleRemindStr = ScheudleRemind;
    m_ScheduleRemind = RemindData::RemindStringToRemindData(m_ScheudleRemindStr);
}

QString ScheduleJsonData::ScheduleRRule() const
{
    return m_ScheduleRRuleStr;
}

void ScheduleJsonData::setScheduleRRule(const QString &ScheduleRRule)
{
    m_ScheduleRRuleStr = ScheduleRRule;
    m_ScheduleRRule = RepetitionRule::RuleStringToRuleData(m_ScheduleRRuleStr);
}

void ScheduleJsonData::setRepetitionRule(const RepetitionRule &rule)
{
    m_ScheduleRRule = rule;
    m_ScheduleRRuleStr = m_ScheduleRRule.RuleDataToRuleString(m_ScheduleRRule);
}

RepetitionRule ScheduleJsonData::getRepetitionRule() const
{
    return m_ScheduleRRule;
}

QString ScheduleJsonData::ScheduleToJsonStr(const ScheduleJsonData &scheduleJsonData)
{
    QJsonObject _scheduleJsonObject;
    _scheduleJsonObject.insert("ID", scheduleJsonData.ScheduleID());
    _scheduleJsonObject.insert("AllDay", scheduleJsonData.ScheduleIsAllDay());
    _scheduleJsonObject.insert("Remind", scheduleJsonData.ScheudleRemind());
    _scheduleJsonObject.insert("RRule", scheduleJsonData.ScheduleRRule());
    _scheduleJsonObject.insert("Title", scheduleJsonData.ScheudleTitleName());
    _scheduleJsonObject.insert("Description", scheduleJsonData.ScheudleDescription());
    _scheduleJsonObject.insert("Type", scheduleJsonData.ScheudleType());
    _scheduleJsonObject.insert("Start", DateTimeToStringDate(scheduleJsonData.ScheduleBeginDateTime()));
    _scheduleJsonObject.insert("End", DateTimeToStringDate(scheduleJsonData.ScheduleEndDateTime()));
    _scheduleJsonObject.insert("RecurID", scheduleJsonData.ScheudleRecurID());
    QJsonArray _ignorreJsonArrory;
    for (int i = 0; i < scheduleJsonData.IgnoreVectorTime().count(); i++) {
        _ignorreJsonArrory.append(DateTimeToStringDate(scheduleJsonData.IgnoreVectorTime().at(i)));
    }
    _scheduleJsonObject.insert("Ignore", _ignorreJsonArrory);
    // 构建 JSON 文档
    QJsonDocument _scheduleJsonDocument;
    _scheduleJsonDocument.setObject(_scheduleJsonObject);
    QByteArray _scheduleByteArray = _scheduleJsonDocument.toJson(QJsonDocument::Compact);
    QString _resultJsonStr(_scheduleByteArray);
    return _resultJsonStr;
}

ScheduleJsonData ScheduleJsonData::JsonToSchedule(const QJsonObject &scheudleJsonObject)
{
    ScheduleJsonData _resultSchedule;
    //因为是预先定义好的JSON数据格式，所以这里可以这样读取
    //日程id
    if (scheudleJsonObject.contains("ID")) {
        _resultSchedule.setScheduleID(scheudleJsonObject.value("ID").toInt());
    }
    //日程是否为全体
    if (scheudleJsonObject.contains("AllDay")) {
        _resultSchedule.setScheduleIsAllDay(scheudleJsonObject.value("AllDay").toBool());
    }
    //日程提醒规则
    if (scheudleJsonObject.contains("Remind")) {
        _resultSchedule.setScheudleRemind(scheudleJsonObject.value("Remind").toString());
    }
    //日程标题
    if (scheudleJsonObject.contains("Title")) {
        _resultSchedule.setScheudleTitleName(scheudleJsonObject.value("Title").toString());
    }
    //日程描述
    if (scheudleJsonObject.contains("Description")) {
        _resultSchedule.setScheudleDescription(scheudleJsonObject.value("Description").toString());
    }
    //日程类型
    if (scheudleJsonObject.contains("Type")) {
        _resultSchedule.setScheudleType(scheudleJsonObject.value("Type").toInt());
    }
    //日程开始时间
    if (scheudleJsonObject.contains("Start")) {
        _resultSchedule.setScheduleBeginDateTime(StringDateToDateTime(scheudleJsonObject.value("Start").toString()));
    }
    //日程结束时间
    if (scheudleJsonObject.contains("End")) {
        _resultSchedule.setScheduleEndDateTime(StringDateToDateTime(scheudleJsonObject.value("End").toString()));
    }
    //日程重复ID
    if (scheudleJsonObject.contains("RecurID")) {
        _resultSchedule.setScheudleRecurID(scheudleJsonObject.value("RecurID").toInt());
    }
    //日程重复规则
    if (scheudleJsonObject.contains("RRule")) {
        _resultSchedule.setScheduleRRule(scheudleJsonObject.value("RRule").toString());
    }
    //重复日程忽略日期集
    if (scheudleJsonObject.contains("Ignore")) {
        QJsonArray subArray = scheudleJsonObject.value("Ignore").toArray();
        QVector<QDateTime> _ignoreDateVector;
        for (int i = 0; i < subArray.size(); i++) {
            QString subObj = subArray.at(i).toString();
            _ignoreDateVector.append(StringDateToDateTime(subObj));
        }
        _resultSchedule.setIgnoreVectorTime(_ignoreDateVector);
    }
    return _resultSchedule;
}

QDateTime ScheduleJsonData::StringDateToDateTime(const QString &dateStr)
{
    return QDateTime::fromString(dateStr,Qt::ISODate);
}

QString ScheduleJsonData::DateTimeToStringDate(const QDateTime &dateTime)
{
    return dateTime.toString(Qt::ISODate);
}

QMap<QDate, QVector<ScheduleJsonData> > ScheduleJsonData::JsonArrayDataToScheduleMapData(const QJsonArray &jsonArrData)
{
    QMap<QDate, QVector<ScheduleJsonData> > _resultMap;
    QJsonObject _jsonObjScheudleVector;
    QVector<ScheduleJsonData> _scheuduleVector;
    QDate _date;
    for (int i = 0; i < jsonArrData.size(); i++) {
        _jsonObjScheudleVector = jsonArrData.at(i).toObject();
        _scheuduleVector.clear();
        //因为是预先定义好的JSON数据格式，所以这里可以这样读取
        if (_jsonObjScheudleVector.contains("Date")) {
            _date = QDate::fromString(_jsonObjScheudleVector.value("Date").toString(), "yyyy-MM-dd");
        }
        if (_jsonObjScheudleVector.contains("Jobs")) {
            QJsonArray subarry = _jsonObjScheudleVector.value("Jobs").toArray();
            for (int j = 0; j < subarry.size(); j++) {
                QJsonObject _jsonObjectScheudle = subarry.at(j).toObject();
                _scheuduleVector.append(JsonToSchedule(_jsonObjectScheudle));
            }
        }
        _resultMap[_date] = _scheuduleVector;
    }
    return _resultMap;
}

QDebug operator<<(QDebug debug,const ScheduleJsonData &scheduleJsonData)
{
    debug<<QString("ID:")<<scheduleJsonData.ScheduleID()<<QString("    ")
        <<QString("IsAllDay:")<<scheduleJsonData.ScheduleIsAllDay()<<QString("    ")
        <<QString("TitleName:")<<scheduleJsonData.ScheudleTitleName()<<QString("    ")
        <<QString("BeginDateTime:")<<scheduleJsonData.ScheduleBeginDateTime()<<QString("    ")
        <<QString("EndDateTime:")<<scheduleJsonData.ScheduleEndDateTime()<<QString("    ")
        <<QString("Description:")<<scheduleJsonData.ScheudleDescription()<<QString("    ")
        <<QString("Type:")<<scheduleJsonData.ScheudleType()<<QString("    ")
        <<QString("RRule:")<<scheduleJsonData.ScheduleRRule()<<QString("    ")
        <<QString("Remind:")<<scheduleJsonData.ScheudleRemind()<<QString("    ")
        <<QString("RecurID:")<<scheduleJsonData.ScheudleRecurID();
    return debug;
}
