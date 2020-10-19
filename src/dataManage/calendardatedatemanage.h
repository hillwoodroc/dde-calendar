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
#ifndef CALENDARDATEDATEMANAGE_H
#define CALENDARDATEDATEMANAGE_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QMap>
/**
 * @brief The CalendarDateDateManage class
 * 日历时间管理类
 */
class CalendarDateDateManage : public QObject
{
    Q_OBJECT
public:
    typedef  QVector<QDate> CDateVector;

public:
    /**
     * @brief CalendarDateDateManage    构造函数
     * @param parent
     */
    explicit CalendarDateDateManage(QObject *parent = nullptr);
    /**
     * @brief setSelectDate     设置选择时间
     * @param selectDate        选择时间
     */
    void setSelectDate(const QDate &selectDate);
    /**
     * @brief getSelectDate     获取选择时间
     * @return                  返回选择时间
     */
    QDate getSelectDate() const;
    /**
     * @brief setCurrentDateTime        设置当前时间
     * @param currentDateTime           当前时间
     */
    void setCurrentDateTime(const QDateTime &currentDateTime = QDateTime::currentDateTime());
    /**
     * @brief getCurrentDate    获取当前时间
     * @return                  返回当前时间
     */
    QDateTime getCurrentDate() const;
    /**
     * @brief getYearDate       获取全年的时间
     * @param year              设置的年份
     * @return                  返回全年的时间，按照月份分组
     */
    QMap<int,QVector<QDate> > getYearDate(const int &year);
    /**
     * @brief getMonthDate      获取月份的所有时间
     * @param year              设置的年份
     * @param month             设置的月份
     * @return                  返回当月全部时间
     */
    QVector<QDate> getMonthDate(const int &year,const int &month);
    /**
     * @brief getWeekDate       获取一周的所有时间
     * @param date              设置的时间
     * @return                  返回这个周全部时间
     */
    QVector<QDate> getWeekDate(const QDate &date);
    /**
     * @brief getDayDateBySelectDate    根据选择时间获取当天日期
     * @return                  返回选择时间
     */
    QDate getDayDateBySelectDate() const;
    /**
     * @brief setWeekFirstDay           设置每周以周几作为每周第一天
     * @param firstDay                  每周第一天
     */
    void setWeekFirstDay(const Qt::DayOfWeek &firstDay);
    /**
     * @brief getWeekFirstDay           获取每周以周几作为每周第一天
     * @return                          每周第一天
     */
    Qt::DayOfWeek getWeekFirstDay() const;
    /**
     * @brief setWeekDayFormatByID          设置周显示格式
     * @param weekDayFormatID
     *                  0 "dddd"
     *                  1 "ddd"
     */
    void setWeekDayFormatByID(const int &weekDayFormatID);
    /**
     * @brief getWeekDayFormat          获取周显示格式
     * @return                          周显示格式
     * "ddd"  周一
     * "dddd" 星期一
     */
    QString getWeekDayFormat() const;
    /**
     * @brief getFirstDayOfWeek         根据日期获取当前周第一天的日期
     * @param date                      选择的日期
     * @return                          当前周第一天的日期
     */
    QDate getFirstDayOfWeek(const QDate &date);
signals:
    
public slots:
private:
    /**
     * @brief m_currentDateTime     当前时间
     */
    QDateTime   m_currentDateTime;
    /**
     * @brief m_selectDate          设置选择时间
     */
    QDate m_selectDate;
    /**
     * @brief m_firstDay          每周第一天
     * 1  周一 
     * 2  周二 
     * 3  周三 
     * 4  周四 
     * 5  周五 
     * 6  周六 
     * 7  周日 
     */
    Qt::DayOfWeek     m_weekFirstDay{Qt::Sunday};
    /**
     * @brief m_weekDayFormat          周显示格式
     * ddd  周一
     * dddd  星期一
     */
    QString     m_weekDayFormat{"ddd"};
};

#endif // CALENDARDATEDATEMANAGE_H
