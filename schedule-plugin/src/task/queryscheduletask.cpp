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
#include "queryscheduletask.h"
#include "../data/queryjsondata.h"
#include "../globaldef.h"
#include "../widget/viewschedulewidget.h"

queryScheduleTask::queryScheduleTask(CSchedulesDBus *dbus)
    : scheduleBaseTask(dbus)
{
}

Reply queryScheduleTask::SchedulePress(semanticAnalysisTask &semanticTask)
{
    QueryJsonData *queryJsonData = dynamic_cast<QueryJsonData *>(semanticTask.getJsonData());

    //查询日程

    if (queryJsonData->offset() > -1
        && queryJsonData->getPropertyStatus() == JsonData::PRO_NONE) {
        Reply m_reply;
        m_reply.setReplyType(Reply::RT_STRING_TTS | Reply::RT_STRING_DISPLAY);
        m_reply.ttsMessage(CANCEL_ERR_TTS);
        m_reply.displayMessage(CANCEL_ERR_TTS);
        return m_reply;
    }

    QDateTime m_BeginDateTime;
    QDateTime m_EndDateTime;
    QTime m_BTime;
    QTime m_ETime;
    QString m_queryTitleName = queryJsonData->TitleName();

    QDateTime m_currentDateTime = QDateTime::currentDateTime();
    QDateTime m_halfAYearDateTime = m_currentDateTime.addMonths(6);

    QVector<int> getDayNum = queryJsonData->getRepeatNum();

    QVector<ScheduleDateRangeInfo> out;
    QVector<ScheduleDateRangeInfo> outdefault;
    QVector<ScheduleDateRangeInfo> showdate;

    viewschedulewidget *viewWidget = new viewschedulewidget();

    m_dbus->QueryJobs(m_queryTitleName, m_currentDateTime, m_halfAYearDateTime, out);
    viewWidget->setScheduleDateRangeInfo(out);
    showdate = viewWidget->getAllScheduleInfo();
    switch (queryJsonData->getRepeatStatus()) {
    case QueryJsonData::RepeatStatus::EVED: {
        showdate = viewWidget->getAllRpeatScheduleInfo(1);
        if (queryJsonData->getDateTime().at(0).hasTime) {
            m_BTime = queryJsonData->getDateTime().at(0).datetime.time();
            m_ETime = queryJsonData->getDateTime().at(0).datetime.time();
            showdate = viewWidget->queryScheduleWithTime(showdate, m_BTime, m_ETime);
        }
    } break;
    case QueryJsonData::RepeatStatus::WORKD: {
        showdate = viewWidget->getAllRpeatScheduleInfo(2);
        if (queryJsonData->getDateTime().at(0).hasTime) {
            m_BTime = queryJsonData->getDateTime().at(0).datetime.time();
            m_ETime = m_BTime;
            showdate = viewWidget->queryScheduleWithTime(showdate, m_BTime, m_ETime);
        }
    } break;
    case QueryJsonData::RepeatStatus::RESTD: {
        showdate = viewWidget->getAllRpeatScheduleInfo(3);

        QVector<int> queryRest;
        queryRest.append(6);
        queryRest.append(7);

        showdate = viewWidget->queryScheduleWithWeek(showdate, queryRest);
        if (queryJsonData->getDateTime().at(0).hasTime) {
            m_BTime = queryJsonData->getDateTime().at(0).datetime.time();
            m_ETime = m_BTime;
            showdate = viewWidget->queryScheduleWithTime(showdate, m_BTime, m_ETime);
        }

    } break;
    case QueryJsonData::RepeatStatus::EVEW: {
        showdate = viewWidget->getAllRpeatScheduleInfo(3);

        if (getDayNum.size() == 1) {
            QVector<int> queryWeek;
            queryWeek.append(getDayNum[0]);

            showdate = viewWidget->queryScheduleWithWeek(showdate, queryWeek);
            if (queryJsonData->getDateTime().at(0).hasTime) {
                m_BTime = queryJsonData->getDateTime().at(0).datetime.time();
                m_ETime = m_BTime;
                showdate = viewWidget->queryScheduleWithTime(showdate, m_BTime, m_ETime);
            }
        } else if (getDayNum.size() == 2) {
            int start = getDayNum[0];
            int end = getDayNum[1];
            QVector<int> queryWeek;

            if (start == end) {
                showdate = viewWidget->getAllRpeatScheduleInfo(1);
                break;
            } else if (start < end) {
                if (start == 1 && end == 5) {
                    showdate = viewWidget->getAllRpeatScheduleInfo(2);
                    break;
                } else if (end - start == 6) {
                    showdate = viewWidget->getAllRpeatScheduleInfo(1);
                    break;
                } else {
                    for (int i = start; i <= end; i++) {
                        queryWeek.append(i);
                    }
                }
            } else {
                if (start - end == 1) {
                    showdate = viewWidget->getAllRpeatScheduleInfo(1);
                    break;
                } else {
                    for (int i = 0; i <= 7 - start; i++) {
                        queryWeek.append(i + start);
                    }
                    for (int i = 0; i < end; i++) {
                        queryWeek.append(i + 1);
                    }
                }
            }

            showdate = viewWidget->queryScheduleWithWeek(showdate, queryWeek);
            if (queryJsonData->getDateTime().size() == 1) {
                if (queryJsonData->getDateTime().at(0).hasTime) {
                    m_BTime = QTime(0, 0, 0);
                    m_ETime = queryJsonData->getDateTime().at(0).datetime.time();
                    showdate = viewWidget->queryScheduleWithWeek(showdate, queryWeek, end, m_BTime, m_ETime);
                }
            } /* else if (queryJsonData->getDateTime().size() == 2) {
} else {

}*/
        } /*else {
if (queryJsonData->getDateTime().size() == 1) {
//按时间查询
if (queryJsonData->getDateTime().at(0).hasTime) {
CLOG(INFO, TitleName.toStdString());
}
} else if (queryJsonData->getDateTime().size() == 2) {

}
}*/
    } break;
    case QueryJsonData::RepeatStatus::EVEM: {
        showdate = viewWidget->getAllRpeatScheduleInfo(4);

        if (getDayNum.size() == 1) {
            QVector<int> queryMonth;
            queryMonth.append(getDayNum[0]);

            showdate = viewWidget->queryScheduleWithMonth(showdate, queryMonth);
            if (queryJsonData->getDateTime().size() == 1) {
                if (queryJsonData->getDateTime().at(0).hasTime) {
                    m_BTime = queryJsonData->getDateTime().at(0).datetime.time();
                    m_ETime = m_BTime;
                    showdate = viewWidget->queryScheduleWithTime(showdate, m_BTime, m_ETime);
                }
            } else if (queryJsonData->getDateTime().size() == 2) {
                m_BTime = queryJsonData->getDateTime().at(0).datetime.time();
                m_ETime = queryJsonData->getDateTime().at(1).datetime.time();
                showdate = viewWidget->queryScheduleWithTime(showdate, m_BTime, m_ETime);
            }
        } else if (getDayNum.size() == 2) {
            int start = getDayNum[0];
            int end = getDayNum[1];
            QVector<int> queryMonth;

            if (start == end) {
                showdate = viewWidget->getAllRpeatScheduleInfo(1);
                break;
            } else if (start < end) {
                for (int i = start; i <= end; i++) {
                    queryMonth.append(i);
                }
            }

            showdate = viewWidget->queryScheduleWithMonth(showdate, queryMonth);
            //            if (queryJsonData->getDateTime().size() == 1) {
            //                if (queryJsonData->getDateTime().at(0).hasTime) {

            //                }
            //            } else if (queryJsonData->getDateTime().size() == 2) {
            //            } else {
            //            }
        } else {
            if (queryJsonData->getDateTime().at(0).hasTime) {
                m_BTime = queryJsonData->getDateTime().at(0).datetime.time();
                m_ETime = m_BTime;
                showdate = viewWidget->queryScheduleWithTime(showdate, m_BTime, m_ETime);
            }
        }
    } break;
    case QueryJsonData::RepeatStatus::EVEY: {
        showdate = viewWidget->getAllRpeatScheduleInfo(5);

        //        if (queryJsonData->getDateTime().size() == 1) {
        //            QDate m_beginD = queryJsonData->getDateTime().at(0).datetime.date();
        //            QDate m_endD = m_beginD;
        //            showdate = viewWidget->queryScheduleWithDate(showdate, m_beginD, m_endD);
        //        }
    } break;
    default: {
        switch (queryJsonData->getPropertyStatus()) {
        case QueryJsonData::PropertyStatus::ALL:
            break;
        case QueryJsonData::PropertyStatus::NEXT: {
            ScheduleDateRangeInfo scheduleDinfo;
            if (showdate.isEmpty()) {
                break;
            } else {
                scheduleDinfo.vData.append(showdate.at(0).vData.at(0));
                showdate.clear();
                showdate.append(scheduleDinfo);
            }
            //            QVector<ScheduleDateRangeInfo> out1;
            //            m_dbus->GetJobsWithLimit(m_currentDateTime, m_halfAYearDateTime, 1, out1);
            //            viewWidget->setScheduleDateRangeInfo(out1);
            //            showdate = viewWidget->getAllScheduleInfo();
        } break;
        case QueryJsonData::PropertyStatus::LAST:
            break;
        default: {
            if (queryJsonData->getDateTime().size() == 1) {
                if (queryJsonData->getDateTime().at(0).hasTime) {
                    m_BeginDateTime = queryJsonData->getDateTime().at(0).datetime;
                    m_EndDateTime = m_BeginDateTime;
                } else {
                    m_BeginDateTime = queryJsonData->getDateTime().at(0).datetime;
                    if (m_BeginDateTime.date() == m_currentDateTime.date()) {
                        m_BeginDateTime.setTime(m_currentDateTime.time());
                    }
                    m_EndDateTime.setDate(m_BeginDateTime.date());
                    m_EndDateTime.setTime(QTime(23, 59, 59));
                }
                m_dbus->QueryJobs(m_queryTitleName, m_BeginDateTime, m_EndDateTime, outdefault);
                viewWidget->setScheduleDateRangeInfo(outdefault);
                showdate = viewWidget->getAllScheduleInfo();
            } else if (queryJsonData->getDateTime().size() == 2) {
                m_BeginDateTime = queryJsonData->getDateTime().at(0).datetime;
                m_EndDateTime = queryJsonData->getDateTime().at(1).datetime;

                if (queryJsonData->getDateTime().at(0).datetime.date() < m_currentDateTime.date()) {
                    m_BeginDateTime.setDate(m_currentDateTime.date());
                }
                if (queryJsonData->getDateTime().at(1).datetime.date() < m_currentDateTime.date()) {
                    break;
                }

                if (queryJsonData->getDateTime().at(0).hasTime) {
                    if (queryJsonData->getDateTime().at(0).datetime.time() <= m_currentDateTime.time())
                        m_BeginDateTime.setTime(m_currentDateTime.time());
                    else
                        m_BeginDateTime.setTime(queryJsonData->getDateTime().at(0).datetime.time());
                } else {
                    if (queryJsonData->getDateTime().at(0).datetime.date() > m_currentDateTime.date()) {
                        m_BeginDateTime.setTime(QTime(0, 0, 0));
                    } else {
                        m_BeginDateTime.setTime(m_currentDateTime.time());
                    }
                }
                if (queryJsonData->getDateTime().at(1).hasTime) {
                    m_EndDateTime.setTime(queryJsonData->getDateTime().at(1).datetime.time());
                } else {
                    m_EndDateTime.setTime(QTime(23, 59, 59));
                }
                m_dbus->QueryJobs(m_queryTitleName, m_BeginDateTime, m_EndDateTime, outdefault);
                viewWidget->setScheduleDateRangeInfo(outdefault);
                showdate = viewWidget->getAllScheduleInfo();
            }
        }
        }
    } break;
    }

    viewWidget->viewScheduleInfoShow(showdate);

    Reply m_reply;
    bool overduedate = false;
    int datenum;
    if (queryJsonData->getDateTime().size() > 0) {
        if (queryJsonData->getDateTime().size() == 1)
            datenum = 0;
        else
            datenum = 1;

        if (queryJsonData->getDateTime().at(datenum).datetime.date() < m_currentDateTime.date()) {
            overduedate = true;
        } else if (queryJsonData->getDateTime().at(datenum).datetime.date() == m_currentDateTime.date()
                   && queryJsonData->getDateTime().at(datenum).hasTime
                   && queryJsonData->getDateTime().at(datenum).datetime.time() < m_currentDateTime.time()) {
            overduedate = true;
        } else {
            overduedate = false;
        }
    } else {
        overduedate = false;
    }

    if (overduedate) {
        m_reply.setReplyType(Reply::RT_STRING_TTS | Reply::RT_STRING_DISPLAY);
        m_reply.ttsMessage("抱歉，不能查询过期的提醒");
        m_reply.displayMessage("抱歉，不能查询过期的提醒");
    } else {
        if (viewWidget->getScheduleNum(showdate) == 0) {
            m_reply.setReplyType(Reply::RT_STRING_TTS | Reply::RT_STRING_DISPLAY);
            m_reply.ttsMessage("没有找到对应的日程");
            m_reply.displayMessage("没有找到对应的日程");
        } else {
            QString str = QString("找到%1个日程").arg(viewWidget->getScheduleNum(showdate));
            m_reply.setReplyType(Reply::RT_INNER_WIDGET | Reply::RT_STRING_TTS | Reply::RT_STRING_DISPLAY);
            m_reply.setReplyWidget(viewWidget);
            m_reply.ttsMessage(str);
            m_reply.displayMessage(str);
        }
    }
    return m_reply;
}

#ifdef LOG_PRINT
void queryScheduleTask::logPrint(const JsonData *jsonData)
{
    Q_UNUSED(jsonData)
}
#endif
