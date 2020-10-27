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
#ifndef REPETITIONRULE_H
#define REPETITIONRULE_H

#include <QObject>
#include <QDateTime>
/**
 * @brief The RepetitionRule class
 * 重复规则
 */

class RepetitionRule
{
public:
    enum RRuleID{
	RRule_NONE,		//不重复
	RRule_EVEDAY,		//每天
	RRule_WORKDAY,		//每个工作日
	RRule_EVEWEEK,		//每周
	RRule_EVEMONTH,		//每月
	RRule_EVEYEAR,		//每年
	RRule_CUSTOM		//用户自定义
    };
    enum RRuleEndType{
	RRuleType_NEVER,	//用不
	RRuleType_FREQ,		//结束于次数
	RRuleType_DATE		//结束与日期
    };
public:
    explicit RepetitionRule();
    /**
     * @brief getRuleId		获取重复规则id
     * @return 
     */
    RRuleID getRuleId() const;
    /**
     * @brief setRuleId		设置重复规则id
     * @param ruleId
     */
    void setRuleId(const RRuleID ruleId);
    /**
     * @brief setRuleType	设置重复结束类型
     * @param endType
     */
    void setRuleType(const RRuleEndType &endType);
    /**
     * @brief getRuleType	获取重复结束类型
     * @return 
     */
    RRuleEndType getRuleType() const;
    /**
     * @brief getEndDate	获取结束时间
     * @return 
     */
    QDateTime getEndDate() const;
    /**
     * @brief setEndDate	设置结束时间
     * @param endDate		结束时间
     */
    void setEndDate(const QDateTime &endDate);
    /**
     * @brief getEndCount	获取结束次数
     * @return 
     */
    int getEndCount() const;
    /**
     * @brief setEndCount	设置结束次数
     * @param endCount		结束次数
     */
    void setEndCount(const int endCount);
    /**
     * @brief RuleStringToRuleData  解析重复规则
     * @param ruleStr
     * @return 
     */
    static RepetitionRule RuleStringToRuleData(const QString &ruleStr);
    /**
     * @brief RuleDataToRuleString
     * @param rule
     * @return 
     */
    static QString RuleDataToRuleString(const RepetitionRule &rule);
private:
    /**
     * @brief m_ruleID
     * 重复规则编号
     */
    RRuleID   m_ruleID;
    /**
     * @brief m_endType
     * 结束类型
     */
    RRuleEndType    m_ruleEndType;
    /**
     * @brief m_endDateTime
     *  重复结束时间
     */
    QDateTime	m_endDateTime;
    /**
     * @brief m_endCount
     * 重复结束次数
     */
    int 	m_endCount;
};

#endif // REPETITIONRULE_H
