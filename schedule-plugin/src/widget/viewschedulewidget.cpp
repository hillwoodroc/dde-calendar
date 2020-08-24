#include "viewschedulewidget.h"
#include "opencalendarwidget.h"
#include <QVBoxLayout>
#include <QProcess>

#include "../globaldef.h"
#include "../dbus/schedulesdbus.h"

viewschedulewidget::viewschedulewidget(QWidget *parent)
    : IconDFrame(parent)
{
}

viewschedulewidget::~viewschedulewidget()
{
}

void viewschedulewidget::setScheduleDateRangeInfo(QVector<ScheduleDateRangeInfo> &scheduleDateInfo)
{
    m_scheduleDateInfo = scheduleDateInfo;
}

void viewschedulewidget::viewScheduleInfoShow(QVector<ScheduleDateRangeInfo> m_showdate)
{
    QVBoxLayout *mainlayout = new QVBoxLayout();
    mainlayout->setSpacing(10);
    int scheduleNum = 0;

    for (int i = 0; i < m_showdate.count(); i++) {
        scheduleitemwidget *item = new scheduleitemwidget();
        connect(item, &scheduleitemwidget::signalItemPress, this, &viewschedulewidget::slotItemPress);
        QVector<ScheduleDtailInfo> scheduleDtailInfo;
        scheduleDtailInfo = m_showdate.at(i).vData;
        scheduleNum += scheduleDtailInfo.count();

        if (scheduleNum > 10) {
            for (int j = 0; j < scheduleNum - 10; j++) {
                scheduleDtailInfo.removeLast();
            }
            item->setScheduleDtailInfo(scheduleDtailInfo);
            item->addscheduleitem();
            mainlayout->addWidget(item);
            OpenCalendarWidget *openWidget = new OpenCalendarWidget();
            openWidget->setScheduleCount(getScheduleNum(m_showdate));
            //    mainlayout->addSpacing(6);
            mainlayout->addWidget(openWidget);
            break;
        }
        item->setScheduleDtailInfo(scheduleDtailInfo);
        item->addscheduleitem();
        mainlayout->addWidget(item);
    }
    setCenterLayout(mainlayout);
}

int viewschedulewidget::getScheduleNum(QVector<ScheduleDateRangeInfo> m_showdate)
{
    int scheduleTotalNum = 0;

    for (int i = 0; i < m_showdate.count(); i++) {
        scheduleTotalNum += m_showdate.at(i).vData.count();
    }
    return scheduleTotalNum;
}

void viewschedulewidget::setQueryBeginDateTime(QDateTime begindatetime)
{
    m_beginDateTime = begindatetime;
}

void viewschedulewidget::setQueryEndDateTime(QDateTime enddatetime)
{
    m_endDateTime = enddatetime;
}

QVector<ScheduleDateRangeInfo> viewschedulewidget::getAllRpeatScheduleInfo(int rpeat)
{
    QVector<ScheduleDateRangeInfo> scheduleInfo;
    ScheduleDateRangeInfo scheduleDtailInfo;

    for (int i = 0; i < getAllScheduleInfo().count(); i++) {
        for (int j = 0; j < getAllScheduleInfo().at(i).vData.count(); j++) {
            if (getAllScheduleInfo().at(i).vData.at(j).rpeat == rpeat) {
                scheduleDtailInfo.vData.append(getAllScheduleInfo().at(i).vData.at(j));
            }
        }
        if (scheduleDtailInfo.vData.count() > 0)
            scheduleInfo.append(scheduleDtailInfo);
        scheduleDtailInfo.vData.clear();
    }
    return scheduleInfo;
}

void viewschedulewidget::slotItemPress(const ScheduleDtailInfo &info)
{
    QProcess proc;
    proc.startDetached(PROCESS_OPEN_CALENDAR);
    QThread::msleep(750);
    QString schedulestr = CSchedulesDBus::createScheduleDtailInfojson(info);
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_CALENDAR_SERVICE,
                                                          DBUS_CALENDAR_PATCH,
                                                          DBUS_CALENDAR_INTFACE,
                                                          "OpenSchedule");
    message << schedulestr;
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
}

QVector<ScheduleDateRangeInfo> viewschedulewidget::queryScheduleWithTime(QVector<ScheduleDateRangeInfo> &scheduleinfo, QTime beginT, QTime endT)
{
    QVector<ScheduleDateRangeInfo> scheduleDateInfo;
    ScheduleDateRangeInfo scheduleDInfo;

    for (int i = 0; i < scheduleinfo.count(); i++) {
        for (int j = 0; j < scheduleinfo.at(i).vData.count(); j++) {
            if (scheduleinfo.at(i).vData.at(j).beginDateTime.time() <= endT
                && scheduleinfo.at(i).vData.at(j).endDateTime.time() >= beginT) {
                scheduleDInfo.vData.append(scheduleinfo.at(i).vData.at(j));
            }
        }
        if (scheduleDInfo.vData.count() > 0)
            scheduleDateInfo.append(scheduleDInfo);
        scheduleDInfo.vData.clear();
    }
    return scheduleDateInfo;
}

QVector<ScheduleDateRangeInfo> viewschedulewidget::queryScheduleWithDate(QVector<ScheduleDateRangeInfo> &scheduleinfo, QDate beginD, QDate endD)
{
    QVector<ScheduleDateRangeInfo> scheduleDateInfo;
    ScheduleDateRangeInfo scheduleDInfo;

    for (int i = 0; i < scheduleinfo.count(); i++) {
        for (int j = 0; j < scheduleinfo.at(i).vData.count(); j++) {
            if (scheduleinfo.at(i).vData.at(j).beginDateTime.date() <= endD
                && scheduleinfo.at(i).vData.at(j).endDateTime.date() >= beginD) {
                scheduleDInfo.vData.append(scheduleinfo.at(i).vData.at(j));
            }
        }
        if (scheduleDInfo.vData.count() > 0)
            scheduleDateInfo.append(scheduleDInfo);
        scheduleDInfo.vData.clear();
    }
    return scheduleDateInfo;
}

QVector<ScheduleDateRangeInfo> viewschedulewidget::queryScheduleWithWeek(QVector<ScheduleDateRangeInfo> &scheduleinfo, QVector<int> weekDay, int dayofweek, QTime beginT, QTime endT)
{
    QVector<ScheduleDateRangeInfo> scheduleDateInfo;
    ScheduleDateRangeInfo scheduleDInfo;

    for (int i = 0; i < scheduleinfo.count(); i++) {
        for (int j = 0; j < scheduleinfo.at(i).vData.count(); j++) {
            for (int k = 0; k < weekDay.count(); k++) {
                if (scheduleinfo.at(i).vData.at(j).beginDateTime.date().dayOfWeek() == weekDay[k]) {
                    if (weekDay[k] == dayofweek) {
                        if (scheduleinfo.at(i).vData.at(j).beginDateTime.time() <= endT
                            && scheduleinfo.at(i).vData.at(j).endDateTime.time() >= beginT) {
                            scheduleDInfo.vData.append(scheduleinfo.at(i).vData.at(j));
                        }
                    } else {
                        scheduleDInfo.vData.append(scheduleinfo.at(i).vData.at(j));
                    }
                    break;
                }
            }
        }
        if (scheduleDInfo.vData.count() > 0)
            scheduleDateInfo.append(scheduleDInfo);
        scheduleDInfo.vData.clear();
    }

    return scheduleDateInfo;
}

QVector<ScheduleDateRangeInfo> viewschedulewidget::queryScheduleWithMonth(QVector<ScheduleDateRangeInfo> &scheduleinfo, QVector<int> monthDay, int dayofmonth, QTime beginT, QTime endT)
{
    QVector<ScheduleDateRangeInfo> scheduleDateInfo;
    ScheduleDateRangeInfo scheduleDInfo;

    for (int i = 0; i < scheduleinfo.count(); i++) {
        for (int j = 0; j < scheduleinfo.at(i).vData.count(); j++) {
            for (int k = 0; k < monthDay.count(); k++) {
                if (scheduleinfo.at(i).vData.at(j).beginDateTime.date().day() == monthDay[k]) {
                    if (monthDay[k] == dayofmonth) {
                        if (scheduleinfo.at(i).vData.at(j).beginDateTime.time() <= endT
                            && scheduleinfo.at(i).vData.at(j).endDateTime.time() >= beginT) {
                            scheduleDInfo.vData.append(scheduleinfo.at(i).vData.at(j));
                        }
                    } else {
                        scheduleDInfo.vData.append(scheduleinfo.at(i).vData.at(j));
                    }
                    break;
                }
            }
        }
        if (scheduleDInfo.vData.count() > 0)
            scheduleDateInfo.append(scheduleDInfo);
        scheduleDInfo.vData.clear();
    }

    return scheduleDateInfo;
}

QVector<ScheduleDateRangeInfo> viewschedulewidget::getAllScheduleInfo()
{
    ScheduleDateRangeInfo showdate;
    QVector<ScheduleDateRangeInfo> showDate;

    for (int i = 0; i < m_scheduleDateInfo.count(); i++) {
        showdate.date = m_scheduleDateInfo.at(i).date;
        for (int j = 0; j < m_scheduleDateInfo.at(i).vData.count(); j++) {
            if (!(m_scheduleDateInfo.at(i).vData.at(j).type.ID == 4)) {
                if (m_scheduleDateInfo.at(i).vData.at(j).beginDateTime.date() == m_scheduleDateInfo.at(i).date /*||
                                !showdate.vData.contains(m_scheduleDateInfo.at(i).vData.at(j))*/
                )
                    showdate.vData.append(m_scheduleDateInfo.at(i).vData.at(j));
            }
        }
        if (showdate.vData.count() > 0) {
            showDate.append(showdate);
        }
        showdate.vData.clear();
    }

    for (int i = 0; i < showDate.count(); i++) {
        for (int j = 0; j < showDate.count() - i - 1; j++) {
            if (showDate[j].date > showDate[j + 1].date)
                std::swap(showDate[j], showDate[j + 1]);
        }
    }
    return showDate;
}
