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
#include "createscheduletask.h"

#include "../data/createjsondata.h"
#include "../globaldef.h"

createScheduleTask::createScheduleTask(CSchedulesDBus *dbus)
    : scheduleBaseTask(dbus)
{
}

Reply createScheduleTask::SchedulePress(semanticAnalysisTask &semanticTask)
{
    // 如果大于当前时间推出

    //创建
    CreateJsonData *createJsonData = dynamic_cast<CreateJsonData *>(semanticTask.getJsonData());
    //查询日程

    if (createJsonData->getPropertyStatus() != JsonData::PRO_NONE
        || createJsonData->offset() > -1) {
        Reply m_reply;
        m_reply.setReplyType(Reply::RT_STRING_TTS | Reply::RT_STRING_DISPLAY);
        m_reply.ttsMessage(CREATE_ERR_TTS);
        m_reply.displayMessage(CREATE_ERR_TTS);
        return m_reply;
    }

    QDateTime currentdatetime = QDateTime::currentDateTime();
    int w = currentdatetime.date().dayOfWeek();
    int d = currentdatetime.date().day();

    QVector<int> getDayNum = createJsonData->getRepeatNum();
    createSchedulewidget *m_widget = new createSchedulewidget();
    QDateTime m_begintime;
    QDateTime m_endtime;
    if (createJsonData->getDateTime().size() > 0) {
        m_begintime = createJsonData->DateTime().at(0).datetime;
    } else {
        m_begintime = currentdatetime;
    }
    if (createJsonData->DateTime().size() == 1) {
        m_endtime = m_begintime.addSecs(60 * 60);
    } else if (createJsonData->DateTime().size() == 2) {
        m_endtime = createJsonData->DateTime().at(1).datetime;
    }

    if (createJsonData->DateTime().size() > 0) {
        if (m_begintime > QDateTime::currentDateTime()) {
            if (createJsonData->DateTime().begin()->hasTime) {
                m_widget->setDateTime(m_begintime, m_endtime);
                if (createJsonData->TitleName().isEmpty())
                    m_widget->setTitleName(NEW_SCHEDULE);
                else
                    m_widget->setTitleName(createJsonData->TitleName());
                switch (createJsonData->getRepeatStatus()) {
                case CreateJsonData::NONE:
                    m_widget->setRpeat(0);
                    break;
                case CreateJsonData::EVED:
                    m_widget->setRpeat(1);
                    break;
                case CreateJsonData::EVEW: {
                    if (getDayNum.size() == 0) {
                        if (m_begintime.time() > currentdatetime.time()) {
                            m_begintime.setDate(currentdatetime.date());
                        } else {
                            m_begintime.setDate(currentdatetime.date().addDays(1));
                        }
                        m_endtime.setDate(m_begintime.date());
                        m_widget->setDateTime(m_begintime, m_endtime);
                        m_widget->setRpeat(3);
                    } else if (getDayNum.size() == 1) {
                        if (getDayNum[0] >= w) {
                            m_begintime.setDate(currentdatetime.date().addDays(getDayNum[0] - w));
                        } else {
                            m_begintime.setDate(currentdatetime.date().addDays(getDayNum[0] + 7 - w));
                        }
                        m_endtime.setDate(m_begintime.date());
                        m_widget->setDateTime(m_begintime, m_endtime);
                        m_widget->setRpeat(3);
                    } else if (getDayNum.size() == 2) {
                        int startweekday = getDayNum[0];
                        int endweekday = getDayNum[1];

                        if (startweekday == endweekday) {
                            m_widget->setRpeat(1);
                        } else {
                            if (w < startweekday) {
                                setWeekBehindPartSchedule(m_begintime, m_endtime, w, startweekday + 1, endweekday + 1, m_widget);
                                m_begintime.setDate(currentdatetime.date().addDays(startweekday - w));
                                m_endtime.setDate(m_begintime.date());
                                m_widget->setDateTime(m_begintime, m_endtime);
                            } else if (w >= startweekday && w < endweekday) {
                                if (m_begintime.time() > currentdatetime.time()) {
                                    setWeekBehindPartSchedule(m_begintime, m_endtime, w, w + 1, endweekday + 1, m_widget);
                                    setWeekBehindPartSchedule(m_begintime, m_endtime, w - 7, startweekday, w + 1, m_widget);
                                    m_begintime.setDate(currentdatetime.date());
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                } else {
                                    setWeekBehindPartSchedule(m_begintime, m_endtime, w, w + 2, endweekday + 1, m_widget);
                                    setWeekBehindPartSchedule(m_begintime, m_endtime, w - 7, startweekday, w + 1, m_widget);
                                    m_begintime.setDate(currentdatetime.date().addDays(1));
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                }
                            } else if (w > endweekday) {
                                setWeekBehindPartSchedule(m_begintime, m_endtime, w - 7, startweekday + 1, endweekday + 1, m_widget);
                                m_begintime.setDate(currentdatetime.date().addDays(startweekday - w + 7));
                                m_endtime.setDate(m_begintime.date());
                                m_widget->setDateTime(m_begintime, m_endtime);
                            } else if (w == endweekday) {
                                if (m_begintime.time() > currentdatetime.time()) {
                                    setWeekBehindPartSchedule(m_begintime, m_endtime, w - 7, startweekday, endweekday, m_widget);
                                    m_begintime.setDate(currentdatetime.date());
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                } else {
                                    setWeekBehindPartSchedule(m_begintime, m_endtime, w - 7, startweekday + 1, endweekday + 1, m_widget);
                                    m_begintime.setDate(currentdatetime.date().addDays(startweekday - w + 7));
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                }
                            }
                            m_widget->setRpeat(3);
                        }
                    }
                } break;
                case CreateJsonData::EVEM: {
                    if (getDayNum.size() == 0) {
                        if (m_begintime.time() > currentdatetime.time()) {
                            m_begintime.setDate(currentdatetime.date());
                        } else {
                            m_begintime.setDate(currentdatetime.date().addDays(1));
                        }
                        m_endtime.setDate(m_begintime.date());
                        m_widget->setDateTime(m_begintime, m_endtime);
                        m_widget->setRpeat(4);
                    } else if (getDayNum.size() == 1) {
                        if (d > getDayNum[0]) {
                            m_begintime.setDate(currentdatetime.date().addDays(getDayNum[0] - d));
                        } else if (d < getDayNum[0]) {
                            m_begintime.setDate(currentdatetime.date().addDays(getDayNum[0] - d));
                        } else {
                            if (m_begintime.time() > currentdatetime.time()) {
                                m_begintime.setDate(currentdatetime.date());
                            } else {
                                m_begintime.setDate(currentdatetime.date().addMonths(1));
                            }
                        }
                        m_endtime.setDate(m_begintime.date());
                        m_widget->setDateTime(m_begintime, m_endtime);
                        m_widget->setRpeat(4);
                    } else if (getDayNum.size() == 2) {
                        int startday = getDayNum[0];
                        int endday = getDayNum[1];

                        if (startday == endday) {
                            m_widget->setRpeat(1);
                        } else {
                            if (d >= endday) {
                                if (m_begintime.time() > currentdatetime.time()) {
                                    setMonthBehindPartSchedule(m_begintime, m_endtime, startday, endday, true, m_widget);
                                    m_begintime.setDate(currentdatetime.date());
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                } else {
                                    setMonthBehindPartSchedule(m_begintime, m_endtime, startday + 1, endday, true, m_widget);
                                    m_begintime.setDate(currentdatetime.date().addDays(startday - d).addMonths(1));
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                }
                            } else if (d > startday && d < endday) {
                                if (m_begintime.time() > currentdatetime.time()) {
                                    setMonthBehindPartSchedule(m_begintime, m_endtime, startday, d, true, m_widget);
                                    setMonthBehindPartSchedule(m_begintime, m_endtime, d + 1, endday + 1, false, m_widget);
                                    m_begintime.setDate(currentdatetime.date());
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                } else {
                                    setMonthBehindPartSchedule(m_begintime, m_endtime, startday, d + 1, true, m_widget);
                                    setMonthBehindPartSchedule(m_begintime, m_endtime, d + 2, endday + 1, false, m_widget);
                                    m_begintime.setDate(currentdatetime.date().addDays(1));
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                }
                            } else {
                                if (m_begintime.time() > currentdatetime.time()) {
                                    setMonthBehindPartSchedule(m_begintime, m_endtime, startday + 1, endday + 1, false, m_widget);
                                    m_begintime.setDate(currentdatetime.date().addDays(startday - d));
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                } else {
                                    setMonthBehindPartSchedule(m_begintime, m_endtime, startday + 2, endday + 1, false, m_widget);
                                    m_begintime.setDate(currentdatetime.date().addDays(startday + 1 - d));
                                    m_endtime.setDate(m_begintime.date());
                                    m_widget->setDateTime(m_begintime, m_endtime);
                                }
                            }
                            m_widget->setRpeat(4);
                        }
                    }
                } break;
                case CreateJsonData::EVEY:
                    m_widget->setRpeat(5);
                    break;
                case CreateJsonData::WORKD: /*{
                    if (w >= 1 && w <= 5) {
                        if (m_begintime.time() > currentdatetime.time()) {
                            m_begintime.setDate(currentdatetime.date());
                            m_widget->setDateTime(m_begintime, m_endtime);
                        } else {
                            if (w == 5) {
                                m_begintime.setDate(currentdatetime.date().addDays(3));
                                m_widget->setDateTime(m_begintime, m_endtime);
                            } else {
                                m_begintime.setDate(currentdatetime.date().addDays(1));
                                m_widget->setDateTime(m_begintime, m_endtime);
                            }
                        }
                    } else if (w > 5) {
                        m_begintime.setDate(currentdatetime.date().addDays(1 - w + 7));
                        m_widget->setDateTime(m_begintime, m_endtime);
                    }*/
                    m_widget->setRpeat(2);
                    //                }
                    break;
                case CreateJsonData::RESTD: {
                    if (w < 6) {
                        setWeekBehindPartSchedule(m_begintime, m_endtime, w, 7, 8, m_widget);
                        m_begintime.setDate(currentdatetime.date().addDays(6 - w));
                        m_endtime.setDate(m_begintime.date());
                        m_widget->setDateTime(m_begintime, m_endtime);
                    } else if (w == 6) {
                        if (m_begintime.time() > currentdatetime.time()) {
                            setWeekBehindPartSchedule(m_begintime, m_endtime, w, 7, 8, m_widget);
                            m_begintime.setDate(currentdatetime.date().addDays(6 - w));
                            m_endtime.setDate(m_begintime.date());
                            m_widget->setDateTime(m_begintime, m_endtime);
                        } else {
                            setWeekBehindPartSchedule(m_begintime, m_endtime, w - 7, 6, 8, m_widget);
                            m_begintime.setDate(currentdatetime.date().addDays(1));
                            m_endtime.setDate(m_begintime.date());
                            m_widget->setDateTime(m_begintime, m_endtime);
                        }
                    } else {
                        if (m_begintime.time() > currentdatetime.time()) {
                            setWeekBehindPartSchedule(m_begintime, m_endtime, w - 7, 6, 8, m_widget);
                            m_begintime.setDate(currentdatetime.date());
                            m_endtime.setDate(m_begintime.date());
                            m_widget->setDateTime(m_begintime, m_endtime);
                        } else {
                            setWeekBehindPartSchedule(m_begintime, m_endtime, w - 7, 7, 8, m_widget);
                            m_begintime.setDate(currentdatetime.date().addDays(6 - w + 7));
                            m_endtime.setDate(m_begintime.date());
                            m_widget->setDateTime(m_begintime, m_endtime);
                        }
                    }
                    m_widget->setRpeat(3);
                } break;
                }
                m_widget->setschedule();
                m_widget->scheduleEmpty(true);
                m_widget->updateUI();
                m_dbus->CreateJob(m_widget->getScheduleDtailInfo());
            }
        }
    }
    Reply m_reply;
    if (m_widget == nullptr) {
        m_reply.setReplyType(Reply::RT_STRING_TTS | Reply::RT_STRING_DISPLAY);
        m_reply.ttsMessage(createJsonData->SuggestMsg());
        m_reply.displayMessage(createJsonData->SuggestMsg());
    } else {
        m_reply.setReplyType(Reply::RT_INNER_WIDGET | Reply::RT_STRING_TTS | Reply::RT_STRING_DISPLAY);
        m_reply.setReplyWidget(m_widget);
        if (createJsonData->getRepeatStatus() == CreateJsonData::RESTD) {
            QString str = QString("好的，每周六到周日的%1点我都会提醒您。").arg(m_begintime.time().hour());
            m_reply.ttsMessage(str);
            m_reply.displayMessage(str);
        } else {
            m_reply.ttsMessage(createJsonData->SuggestMsg());
            m_reply.displayMessage(createJsonData->SuggestMsg());
        }
    }
    return m_reply;
}

void createScheduleTask::setWeekBehindPartSchedule(QDateTime begintime, QDateTime endtime, int isSameWeek, int startDay, int endDay, createSchedulewidget *m_widget)
{
    QDateTime currentdatetime = QDateTime::currentDateTime();
    QVector<QDateTime> datetime;

    for (int i = startDay; i < endDay; i++) {
        begintime.setDate(currentdatetime.date().addDays(i - isSameWeek));
        endtime.setDate(begintime.date());
        m_widget->setDateTime(begintime, endtime);
        m_widget->setRpeat(3);
        m_widget->setschedule();
        m_dbus->CreateJob(m_widget->getScheduleDtailInfo());
    }
}

void createScheduleTask::setMonthBehindPartSchedule(QDateTime begintime, QDateTime endtime, int startDay, int endDay, bool nextMonth, createSchedulewidget *m_widget)
{
    QDateTime currentdatetime = QDateTime::currentDateTime();
    int d = currentdatetime.date().day();
    QVector<QDateTime> datetime;

    if (nextMonth) {
        for (int i = startDay; i < endDay + 1; i++) {
            begintime.setDate(currentdatetime.date().addDays(i - d).addMonths(1));
            endtime.setDate(begintime.date());
            m_widget->setDateTime(begintime, endtime);
            m_widget->setRpeat(4);
            m_widget->setschedule();
            m_dbus->CreateJob(m_widget->getScheduleDtailInfo());
        }
    } else {
        for (int i = startDay; i < endDay + 1; i++) {
            begintime.setDate(currentdatetime.date().addDays(i - d));
            endtime.setDate(begintime.date());
            m_widget->setDateTime(begintime, endtime);
            m_widget->setRpeat(4);
            m_widget->setschedule();
            m_dbus->CreateJob(m_widget->getScheduleDtailInfo());
        }
    }
}
