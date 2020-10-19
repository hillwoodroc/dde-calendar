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
#include "calendardatedatemanage.h"

CalendarDateDateManage::CalendarDateDateManage(QObject *parent)
    : QObject(parent)
    ,m_currentDateTime(QDateTime::currentDateTime())
    ,m_selectDate(m_currentDateTime.date())
    ,m_weekFirstDay(Qt::Sunday)
    ,m_weekDayFormat("ddd")
{
    
}

void CalendarDateDateManage::setSelectDate(const QDate &selectDate)
{
    m_selectDate = selectDate;
}

QDate CalendarDateDateManage::getSelectDate() const
{
    return m_selectDate;
}

void CalendarDateDateManage::setCurrentDateTime(const QDateTime &currentDateTime)
{
    m_currentDateTime = currentDateTime;
}

QDateTime CalendarDateDateManage::getCurrentDate() const
{
    return  m_currentDateTime;
}

QMap<int,QVector<QDate> > CalendarDateDateManage::getYearDate(const int &year)
{
    QMap<int ,QVector<QDate> > resultMap;
    for (int i = 1; i < 13; ++i) {
        resultMap[i] = getMonthDate(year,i);
    }
    return resultMap;
}

QVector<QDate> CalendarDateDateManage::getMonthDate(const int &year, const int &month)
{
    QVector<QDate> resultDate;
    //自然月的第一天
    const QDate _monthFirstDay{year,month,1};
    //获取显示月的第一天
    const QDate _firstDayofMonth = getFirstDayOfWeek(_monthFirstDay);
    //获取该月所有显示时间
    for (int i = 0; i < 42; ++i) {
        resultDate.append(_firstDayofMonth.addDays(i));
    }
    return resultDate;
}

QVector<QDate> CalendarDateDateManage::getWeekDate(const QDate &date)
{
    QVector<QDate> resultDate;
    //获取这个周的第一天日期
    const QDate _firstDayofWeek = getFirstDayOfWeek(date);
    //获取该周所有显示时间
    for (int i = 0; i<7; ++i) {
        resultDate.append(_firstDayofWeek.addDays(i));
    }
    return resultDate;
}

QDate CalendarDateDateManage::getDayDateBySelectDate() const
{
    return m_selectDate;
}

void CalendarDateDateManage::setWeekFirstDay(const Qt::DayOfWeek &firstDay)
{
    m_weekFirstDay = firstDay;
}

Qt::DayOfWeek CalendarDateDateManage::getWeekFirstDay() const
{
    return  m_weekFirstDay;
}

void CalendarDateDateManage::setWeekDayFormatByID(const int &weekDayFormatID)
{
    switch (weekDayFormatID) {
    case 0:
        m_weekDayFormat = "dddd";
        break;
    default:
        m_weekDayFormat = "ddd";
        break;
    }
}

QString CalendarDateDateManage::getWeekDayFormat() const
{
    return m_weekDayFormat;
}

QDate CalendarDateDateManage::getFirstDayOfWeek(const QDate &date)
{
    //根据选择时间周工作日和每周第一天的周工作日得到偏移量
    int offset = date.dayOfWeek() - m_weekFirstDay;
    //根据偏移量获取需要添加还有减去的偏移天数
    const int offsetDay = offset <0 ?offset +7:offset;
    //返回这周第一天的日期
    return date.addDays(0 - offsetDay);
}
