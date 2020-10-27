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
#ifndef SCHEDULEJSONDATA_H
#define SCHEDULEJSONDATA_H

#include "repetitionrule.h"
#include "reminddata.h"

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QMap>
#include <QJsonObject>
#include <QDataStream>


class QDebug;
/**
 * @brief The ScheduleJsonData class
 * 日程数据结构与Json对应
 */
class ScheduleJsonData
{
public:
    explicit ScheduleJsonData();
    ScheduleJsonData(const ScheduleJsonData &scheduleJsonData);
    /**
     * @brief ScheduleID    获取日程ID
     * @return	    日程ID
     */
    int ScheduleID() const;
    /**
     * @brief setScheduleID	设置日程ID
     * @param ScheduleID	日程id
     */
    void setScheduleID(int ScheduleID);
    
    /**
     * @brief ScheduleBeginDateTime	获取日程开始时间
     * @return	    日程开始时间
     */
    QDateTime ScheduleBeginDateTime() const;
    /**
     * @brief setScheduleBeginDateTime	    设置日程开始时间
     * @param ScheduleBeginDateTime	日程开始时间
     */
    void setScheduleBeginDateTime(const QDateTime &ScheduleBeginDateTime);
    
    /**
     * @brief ScheduleEndDateTime	获取日程结束时间
     * @return	    日程结束时间
     */
    QDateTime ScheduleEndDateTime() const;
    /**
     * @brief setScheduleEndDateTime	设置日程结束时间
     * @param ScheduleEndDateTime	日程结束时间
     */
    void setScheduleEndDateTime(const QDateTime &ScheduleEndDateTime);
    
    /**
     * @brief IgnoreVectorTime	    获取日程重复忽略时间集
     * @return	    日程重复忽略时间集
     */
    QVector<QDateTime> IgnoreVectorTime() const;
    /**
     * @brief setIgnoreVectorTime   设置日程重复忽略时间集
     * @param IgnoreVectorTime	日程重复忽略时间集
     */
    void setIgnoreVectorTime(const QVector<QDateTime> &IgnoreVectorTime);
    
    /**
     * @brief ScheudleTitleName	    获取日程标题
     * @return	    日程标题
     */
    QString ScheudleTitleName() const;
    /**
     * @brief setScheudleTitleName  设置日程标题
     * @param ScheudleTitleName	    日程标题
     */
    void setScheudleTitleName(const QString &ScheudleTitleName);
    
    /**
     * @brief ScheudleDescription   获取日程描述信息
     * @return	    日程描述信息
     */
    QString ScheudleDescription() const;
    /**
     * @brief setScheudleDescription	设置日程描述信息
     * @param ScheudleDescription	日程描述信息
     */
    void setScheudleDescription(const QString &ScheudleDescription);
    
    /**
     * @brief ScheduleIsAllDay	    获取日程是否为全体
     * @return	    是否为全体
     */
    bool ScheduleIsAllDay() const;
    /**
     * @brief setScheduleIsAllDay   设置日程是否为全体
     * @param ScheduleIsAllDay	    是否为全体
     */
    void setScheduleIsAllDay(bool ScheduleIsAllDay);
    
    /**
     * @brief ScheudleType	获取日程类型
     * @return	    日程类型
     */
    int ScheudleType() const;
    /**
     * @brief setScheudleType	设置日程类型
     * @param ScheudleType	日程类型
     */
    void setScheudleType(int ScheudleType);
    
    /**
     * @brief ScheudleRecurID	获取日程重复id类型
     * @return	重复id类型
     */
    int ScheudleRecurID() const;
    /**
     * @brief setScheudleRecurID    设置日程重复id类型
     * @param ScheudleRecurID	    重复id类型
     */
    void setScheudleRecurID(int ScheudleRecurID);
    /**
     * @brief setScheduleRemindData
     * @param remindData
     */
    void setScheduleRemindData(const RemindData &remindData);
    /**
     * @brief getScheduleRemindData     获取提醒规则
     * @return 
     */
    RemindData getScheduleRemindData() const;
    /**
     * @brief setRepetitionRule     设置重复规则
     * @param rule
     */
    void setRepetitionRule(const RepetitionRule &rule);
    /**
     * @brief getRepetitionRule         获取重复规则
     * @return 
     */
    RepetitionRule getRepetitionRule() const;
    /**
     * @brief ScheduleToJson        日程数据转换为json形式的字符串
     * @param scheduleJsonData      日程数据
     * @return          json形式的字符串
     */
    static QString ScheduleToJsonStr(const ScheduleJsonData &scheduleJsonData);
    /**
     * @brief JsonToSchedule            Json对象转换到日程数据
     * @param scheudleJsonObject        json对象
     * @return      日程数据
     */
    static ScheduleJsonData JsonToSchedule(const QJsonObject &scheudleJsonObject);
    /**
     * @brief StringDateToDateTime      string类型的datetime数据转换为QDateTime
     * @param dateStr       string类型的datetime数据
     * @return      QDateTime
     */
    static QDateTime StringDateToDateTime(const QString &dateStr);
    /**
     * @brief DateTimeToStringDate      QDateTime转换为string类型的datetime数据
     * @param dateTime          需要转换的时间
     * @return          string类型的datetime数据
     */
    static QString DateTimeToStringDate(const QDateTime &dateTime);
    /**
     * @brief JsonArrayDataToScheduleMapData        将JsonArray数据转换为QMap存储Schedule数据
     * @param jsonArrData           JsonArray数据
     * @return                      QMap存储Schedule数据
     */
    static QMap<QDate,QVector<ScheduleJsonData>> JsonArrayDataToScheduleMapData(const QJsonArray &jsonArrData);
private:
    /**
     * @brief ScheudleRemind	设置日程提醒规则
     * @return		提醒规则
     */
    QString ScheudleRemind() const;
    /**
     * @brief setScheudleRemind	    设置日程提醒规则
     * @param ScheudleRemind	    提醒规则
     */
    void setScheudleRemind(const QString &ScheudleRemind);
    /**
     * @brief ScheduleRRule	获取日程重复规则
     * @return		日程重复规则
     */
    QString ScheduleRRule() const;
    /**
     * @brief setScheduleRRule	设置日程重复规则
     * @param ScheduleRRule	日程重复规则
     */
    void setScheduleRRule(const QString &ScheduleRRule);
public:
    friend QDebug operator<<(QDebug debug,const ScheduleJsonData &scheduleJsonData);
private:
    /**
     * @brief m_ScheduleID	日程ID
     */
    int			m_ScheduleID;
    /**
     * @brief m_ScheduleBeginDateTime	日常开始时间
     */
    QDateTime		m_ScheduleBeginDateTime;
    /**
     * @brief m_ScheduleEndDateTime	日程结束时间
     */
    QDateTime		m_ScheduleEndDateTime;
    /**
     * @brief m_Ignore_VectorTime	重复日程忽略时间集
     */
    QVector<QDateTime>	    m_IgnoreVectorTime;
    /**
     * @brief m_ScheudleTitleName	日程标题
     */
    QString		m_ScheudleTitleName;
    /**
     * @brief m_ScheudleDescription	日程描述
     */
    QString		m_ScheudleDescription;
    /**
     *@brief m_ScheduleIsAllDay		是否为全体日程
     * true  表示全天
     * false	表示非全体
     */
    bool		m_ScheduleIsAllDay;  //1全天
    /**
     * @brief m_ScheudleType	日程类型
     * 1 工作
     * 2 生活
     * 3 其他
     * 4 节日信息
     */
    int         m_ScheudleType;    //1工作 2 生活 3其他
    /**
     * @brief m_ScheudleRecurID	    日程重复id类型
     * 0 原始数据
     * >0   克隆数据
     */
    int			m_ScheudleRecurID;
    /**
     * @brief m_ScheudleRemind		日程提醒规则
     */
    QString		m_ScheudleRemindStr;
    /**
     * @brief m_ScheduleRemind      提醒规则
     */
    RemindData      m_ScheduleRemind;
    /**r
     * @brief m_ScheduleRRule	日程重复规则
     * 具体规则可参考资料 https://www.rfc-editor.org/rfc/rfc5545.html#page-38
     *			3.3.10.  Recurrence Rule
     */
    QString		m_ScheduleRRuleStr;
    /**
     * @brief m_Rule         重复规则
     */
    RepetitionRule  m_ScheduleRRule;
};
#endif // SCHEDULEJSONDATA_H
