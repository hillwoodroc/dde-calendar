/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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

#include "monthview.h"
#include "calendardbus.h"

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QEvent>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QQueue>
#include "schceduledlg.h"
#include <QMenu>
#include "scheduledatamanage.h"
#include "monthschceduleview.h"
#include <DHiDPIHelper>
#include <DPalette>
#include <QShortcut>
#include <DHiDPIHelper>
#include <QApplication>
#include <QMouseEvent>

DGUI_USE_NAMESPACE
void CMonthView::setTheMe(int type)
{
    m_themetype = type;
    if (type == 0 || type == 1) {
        m_topBorderColor = Qt::red;
        m_backgroundCircleColor = "#0081FF";
        m_backgroundShowColor = "#2CA7F8";
        m_backgroundShowColor.setAlphaF(0.4);

        m_defaultTextColor = Qt::black;
        m_currentDayTextColor = "#FFFFFF";
        m_weekendsTextColor = Qt::black;
        m_selectedTextColor = Qt::white;
        m_festivalTextColor = Qt::black;
        m_notCurrentTextColor = "#b2b2b2";


        m_defaultLunarColor = "#5E5E5E";
        m_defaultLunarColor.setAlphaF(0.5);
        m_currentDayLunarColor = m_currentDayTextColor;
        m_weekendsLunarColor = m_defaultLunarColor;
        m_selectedLunarColor = Qt::white;
        m_festivalLunarColor = m_defaultLunarColor;
        m_notCurrentLunarColor = "#dfdfdf";
        m_solofestivalLunarColor = "#FF7272";
        m_solofestivalLunarColor.setAlphaF(0.3);
        m_wrectColor = "#000000";
        m_wrectColor.setAlphaF(0.05);
        m_fillColor = Qt::white;
        m_banColor = "#FF7171";
        m_banColor.setAlphaF(0.1);
        m_xiuColor = "#ADFF71";
        m_xiuColor.setAlphaF(0.1);

        m_pressColor = "#000000";
        m_pressColor.setAlphaF(0.2);

        m_hoverColor = "#000000";
        m_hoverColor.setAlphaF(0.05);

        //设置今天day字体的颜色
        m_dayNumCurrentColor = "#FFFFFF";
    } else if (type == 2) {

        m_topBorderColor = Qt::red;
        m_backgroundCircleColor = "#0059D2";
        m_backgroundShowColor = "#002AAF";
        m_backgroundShowColor.setAlphaF(0.4);

        m_defaultTextColor = "#C0C6D4";
        m_currentDayTextColor = "#C0C6D4";
        m_weekendsTextColor = m_defaultTextColor;
        m_selectedTextColor = "#B8D3FF";
        m_festivalTextColor = "#C0C6D4";
        m_notCurrentTextColor = "#C0C6D4";
        m_notCurrentTextColor.setAlphaF(0.6);

        m_defaultLunarColor = "#ABDAFF";
        m_defaultLunarColor.setAlphaF(0.5);
        m_currentDayLunarColor = m_currentDayTextColor;
        m_weekendsLunarColor = m_defaultLunarColor;
        m_selectedLunarColor = Qt::white;
        m_festivalLunarColor = m_defaultLunarColor;
        m_notCurrentLunarColor = "#ABDAFF";
        m_notCurrentLunarColor.setAlphaF(0.3);
        m_solofestivalLunarColor = "#4DFF7272";
        QColor wcolor = "#282828";
        //wcolor.setAlphaF(0.5);
        m_wrectColor = wcolor;
        m_fillColor = "#000000";
        m_fillColor.setAlphaF(0.05);
        m_banColor = "#FF7171";
        m_banColor.setAlphaF(0.1);
        m_xiuColor = "#ADFF71";
        m_xiuColor.setAlphaF(0.1);

        m_pressColor = "#000000";
        m_pressColor.setAlphaF(0.5);

        m_hoverColor = "#FFFFFF";
        m_hoverColor.setAlphaF(0.05);

        //设置今天day字体的颜色
        m_dayNumCurrentColor = "#B8D3FF";
    }
    DPalette tooltippa = m_tooltipview->palette();
    tooltippa.setColor(DPalette::WindowText, m_defaultTextColor);
    m_tooltipview->setPalette(tooltippa);
    m_tooltipview->setForegroundRole(DPalette::WindowText);

    m_weekIndicator->setTheMe(type);
    m_MonthSchceduleView->setTheMe(type);
}

void CMonthView::updateHigh()
{
    m_MonthSchceduleView->updateHigh();
}

CMonthView::CMonthView(QWidget *parent) : DWidget(parent)
{
    m_DBusInter = new CalendarDBus("com.deepin.api.LunarCalendar",
                                   "/com/deepin/api/LunarCalendar",
                                   QDBusConnection::sessionBus(), this);
    if (!queue)
        queue = new QQueue<int>;
    if (!lunarCache)
        lunarCache = new QMap<QDate, CaLunarDayInfo>;
    if (!emptyCaLunarDayInfo)
        emptyCaLunarDayInfo = new CaLunarDayInfo;

    m_tooltipview = new DLabel(parent);
    m_tooltipview->setFixedHeight(22);
    m_tooltipview->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    m_tooltipview->setAttribute(Qt::WA_TranslucentBackground);
    m_dayNumFont.setFamily("Avenir");
    m_dayNumFont.setPixelSize(24);
    m_dayNumFont.setWeight(QFont::Light);

    m_dayLunarFont.setFamily("PingFangSC-Regular");
    m_dayLunarFont.setPixelSize(12);

    m_solofestivalLunarColor.setAlphaF(0.3);
    //setStyleSheet("QWidget { background: rgba(0, 0, 0, 0) }");

    m_weekIndicator = new CMonthWeekView;
    m_MonthSchceduleView = new CMonthSchceduleView(this);

    // cells grid
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    for (int r = 0; r != 6; ++r) {
        for (int c = 0; c != 7; ++c) {
            QWidget *cell = new QWidget(this);
            cell->setFixedSize(cellwidth, cellheight);
            cell->installEventFilter(this);
            cell->setFocusPolicy(Qt::ClickFocus);

            gridLayout->addWidget(cell, r, c);
            m_cellList.append(cell);

        }
    }
    connect(m_MonthSchceduleView, &CMonthSchceduleView::signalsUpdateShcedule, this, &CMonthView::slotSchceduleUpdate);
    connect(m_MonthSchceduleView, &CMonthSchceduleView::signalsUpdateShcedule, this, &CMonthView::slotdelete);
    connect(m_MonthSchceduleView, &CMonthSchceduleView::signalsCurrentScheduleDate, this, &CMonthView::signalsCurrentScheduleDate);
    connect(m_MonthSchceduleView, &CMonthSchceduleView::signalViewtransparentFrame, this, &CMonthView::signalViewtransparentFrame);
    connect(m_MonthSchceduleView, &CMonthSchceduleView::signalUpdateUI, this, &CMonthView::slotUpdateUI);
    connect(m_MonthSchceduleView
            , &CMonthSchceduleView::signalPressScheduleShow
            , this
            , &CMonthView::slotScheduleRemindWidget);

    DFrame *gridWidget = new DFrame;
    gridWidget->setFrameRounded(false);
    gridWidget->setLineWidth(0);
    gridWidget->setLayout(gridLayout);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(10, 0, 0, 10);
    m_mainLayout->addWidget(m_weekIndicator);
    m_mainLayout->addWidget(gridWidget);
    for (int i = 0; i < 42; i++) {
        m_cellhoverflag[i] = false;
        m_cellfoceflag[i] = false;
    }
    m_banColor = "#F85566";
    m_banColor.setAlphaF(0.2);
    m_xiuColor = "#6FFF00";
    m_xiuColor.setAlphaF(0.2);
    setLayout(m_mainLayout);
    CScheduleDataCtrl  *scheduleDataCtrl = CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl();
    connect(this, &CMonthView::dateSelected, this, &CMonthView::handleCurrentDateChanged);
    m_createAction = new QAction(tr("New event"), this);

    m_RemindWidget = new SchecduleRemindWidget(this);

    QShortcut *shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(QLatin1String("Ctrl+N")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(slotCreate()));
    connect(m_createAction, &QAction::triggered, this, &CMonthView::slotCreate);
    connect(scheduleDataCtrl, &CScheduleDataCtrl::signalsupdatescheduleD, this, &CMonthView::slotsupdatescheduleD);
    connect(this, &CMonthView::signalsupdatescheduleD, scheduleDataCtrl, &CScheduleDataCtrl::slotupdatescheduleD);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void CMonthView::handleCurrentDateChanged(const QDate date, const CaLunarDayInfo &detail)
{
    Q_UNUSED(detail);

    if (date != m_currentDate) {
        setCurrentDate(date);
    }
}

void CMonthView::slotCtrlSchceduleUpdate(QDate date, int type)
{
    parentWidget()->setEnabled(false);
    emit signalsupdatescheduleD(this, m_days[0], m_days[41]);
}

void CMonthView::slotSchceduleUpdate(int id)
{
    parentWidget()->setEnabled(false);
    emit signalsupdatescheduleD(this, m_days[0], m_days[41]);
}

void CMonthView::slotUpdateUI(int type)
{
    if (type == 0) {
        m_sflag = false;
    } else {
        m_sflag = true;
        slotSchceduleUpdate();
    }
}

void CMonthView::setSelectSchedule(const ScheduleDtailInfo &scheduleInfo)
{
    QVector<DPushButton *> mscheduleShowBtn = m_MonthSchceduleView->getScheduleShowItem();
    for (int i = 0; i < mscheduleShowBtn.size(); ++i) {
        CMonthSchceduleWidgetItem *titem = qobject_cast<CMonthSchceduleWidgetItem *>(mscheduleShowBtn.at(i));
        if (titem == nullptr) continue;
        bool isAnimation = false;
        if (scheduleInfo.type.ID == 4) {
            isAnimation =
                scheduleInfo.titleName == titem->getData().titleName &&
                scheduleInfo.type.ID == titem->getData().type.ID &&
                scheduleInfo.beginDateTime == titem->getData().beginDateTime &&
                scheduleInfo.endDateTime == titem->getData().endDateTime ? true : false;
        } else {
            if (titem->getData() == scheduleInfo) {
                isAnimation = true;
            }
        }
        if (isAnimation) {
            const  int offset = 4;
            titem->setStartValue(0);
            titem->setEndValue(offset);
            titem->startAnimation();
        }

    }

}

void CMonthView::slotsupdatescheduleD(QWidget *w, QVector<ScheduleDateRangeInfo> &data)
{
    if (w != this) return;
    if (m_sflag) {
        m_shceludelistdata = data;
        int h = m_MonthSchceduleView->getSchceduleHeight();
        m_MonthSchceduleView->setallsize(width(), height(), m_leftmaagin, m_weekIndicator->height() + m_topmagin, m_topmagin, h);
        m_MonthSchceduleView->setData(data, m_currentDate.month());
        for (int i(0); i != 42; ++i) {
            m_cellList.at(i)->update();
        }
    }
    parentWidget()->setEnabled(true);
}

void CMonthView::slotdelete(int id)
{
    emit signalsSchceduleUpdate(0);
}

void CMonthView::slotScheduleRemindWidget(const bool isShow, const ScheduleDtailInfo &out)
{
    if (isShow) {
        QPoint pos22 = QCursor::pos();
        CSchedulesColor gdcolor = CScheduleDataManage::getScheduleDataManage()->getScheduleColorByType(
                                      out.type.ID);
        m_RemindWidget->setData(out, gdcolor);
        m_RemindWidget->show(pos22.x() + 10, pos22.y());
        QDesktopWidget *w = QApplication::desktop();
        if ((pos22.x() + m_RemindWidget->width() + 10) > w->width()) {
            m_RemindWidget->setArrowDirection(DArrowRectangle::ArrowRight);
            m_RemindWidget->show(pos22.x() - 10, pos22.y());
        } else {
            m_RemindWidget->setArrowDirection(DArrowRectangle::ArrowLeft);
            m_RemindWidget->show(pos22.x() + 10, pos22.y());
        }

    } else {
        m_RemindWidget->hide();
    }
}

void CMonthView::resizeEvent(QResizeEvent *event)
{
    //cellwidth = width() * 0.1395 + 0.5;
    //cellheight = height() * 0.1428 + 0.5;


    int leftmagin = 10;
    int rightmagin = leftmagin;
    int topmagin = height() * 0.0193 + 0.5;
    topmagin = 10;
    int buttonmagin = topmagin;
    m_leftmaagin = leftmagin;
    m_topmagin = topmagin;
    m_mainLayout->setContentsMargins(leftmagin, topmagin, 0, 10);
    m_weekIndicator->setFixedSize(width() - leftmagin, height() * 0.1042 + 0.5);
    cellwidth = (width() - 10) / 7.0 + 0.5;
    cellheight = (height() - 20 - m_weekIndicator->height()) / 6.0 + 0.5;
    for (int i(0); i != 42; ++i) {
        m_cellList.at(i)->setFixedSize(cellwidth, cellheight);
        m_cellList.at(i)->update();
    }
    int h = m_MonthSchceduleView->getSchceduleHeight();
    m_MonthSchceduleView->setallsize(width(), height(), leftmagin, m_weekIndicator->height() + topmagin, buttonmagin, h);
    // m_MonthSchceduleView->setallsize(width(), height(), leftmagin, height() - m_weekIndicator->height() - cellheight * 6, buttonmagin);
    m_MonthSchceduleView->updateData();
    DWidget::resizeEvent(event);
}

void CMonthView::focusOutEvent(QFocusEvent *event)
{
    m_fouceFlag = false;
    DWidget::focusOutEvent(event);
}

void CMonthView::focusInEvent(QFocusEvent *event)
{
    m_fouceFlag = true;
    DWidget::focusInEvent(event);
}

void CMonthView::mousePressEvent(QMouseEvent *event)
{
    slotScheduleRemindWidget(false);
}

void CMonthView::mouseMoveEvent(QMouseEvent *event)
{

}

void CMonthView::setFirstWeekday(int weekday)
{
    m_firstWeekDay = weekday;

    m_weekIndicator->setList(weekday);
    m_weekIndicator->updateWeek();
    updateDate();
}

int CMonthView::getDateType(const QDate &date)
{
    const int currentIndex = getDateIndex(date);
    const CaLunarDayInfo info = getCaLunarDayInfo(currentIndex);

    const int dayOfWeek = date.dayOfWeek();
    bool weekends = dayOfWeek == 6 || dayOfWeek == 7;
    bool isCurrentMonth = m_currentDate.month() == date.month();
    bool isFestival = !info.mSolarFestival.isEmpty() || !info.mLunarFestival.isEmpty();

    int resultFlag = 0;
    if (!isCurrentMonth)
        resultFlag |= SO_MNotCurrentMonth;
    if (isFestival)
        resultFlag |= SO_MFestival;
    if (weekends)
        resultFlag |= SO_MWeekends;

    return resultFlag;
}

void CMonthView::setCurrentDate(const QDate date)
{
    slotScheduleRemindWidget(false);
    qDebug() << "set current date " << date;
    if (date.year() < 1900) return;
    if (date == m_currentDate) {
        slotSchceduleUpdate();
//        return;
    }
    bool flag = false;
    if (date.month() != m_currentDate.month()) {
        m_festivallist.clear();
        m_DBusInter->GetFestivalMonth(date.year(), date.addMonths(-1).month(), m_festivallist);
        m_DBusInter->GetFestivalMonth(date.year(), date.month(), m_festivallist);
        m_DBusInter->GetFestivalMonth(date.year(), date.addMonths(1).month(), m_festivallist);
        flag = true;
    }
    m_currentDate = date;
    updateDate();

    // to refresh lunar calendar
    updateCurrentLunar(getCaLunarDayInfo(getDateIndex(m_currentDate)));
    emit currentDateChanged(date.year(), date.month());
    emit signalcurrentDateChanged(m_currentDate);
    m_weekIndicator->updateWeek();
    if (flag)
        slotSchceduleUpdate();
}

void CMonthView::setLunarVisible(bool visible)
{
    //  visible = false;
    int state = int(m_showState);

    if (visible)
        state |= ShowLunar;
    else
        state &= ~ShowLunar;

    m_showState = ShowState(state);
    update();
}

void CMonthView::setLunarFestivalHighlight(bool highlight)
{
    highlight = true;
    int state = int(m_showState);
    //

    if (highlight)
        state |= ShowLunarFestivalHighlight;
    else
        state &= ~ShowLunarFestivalHighlight;

    m_showState = ShowState(state);
    update();
}

void CMonthView::setCellSelectable(bool selectable)
{
    if (selectable == m_cellSelectable)
        return;
    m_cellSelectable = selectable;

    emit cellSelectableChanged(m_cellSelectable);
}

int CMonthView::getDateIndex(const QDate &date) const
{
    for (int i = 0; i != 42; ++i)
        if (m_days[i] == date)
            return i;

    return 0;
}

bool CMonthView::eventFilter(QObject *o, QEvent *e)
{
    QWidget *cell = qobject_cast<QWidget *>(o);

    if (cell && m_cellList.contains(cell)) {
        if (e->type() == QEvent::Paint) {
            paintCell(cell);
        } else if (e->type() == QEvent::MouseButtonPress) {
            slotScheduleRemindWidget(false);
            QMouseEvent *rightevent = dynamic_cast<QMouseEvent *>(e);
            if (rightevent->button() == Qt::RightButton)
                m_updateflag = false;
            if (rightevent->button() == Qt::LeftButton) {
                const int pos = m_cellList.indexOf(cell);
                m_cellfoceflag[pos] = true;
                cellClicked(cell);
                m_cellList[pos]->update();
            }
        } else if (e->type() == QEvent::ContextMenu) {
            DMenu Context(this);
            Context.addAction(m_createAction);
            const int pos = m_cellList.indexOf(cell);
            m_createDate = m_days[pos];
            Context.exec(QCursor::pos());
        } else if (e->type() == QEvent::MouseButtonDblClick) {
            const int pos = m_cellList.indexOf(cell);
            QRect fillRect = QRect(0, 0, cell->width(), 36);
            QPoint ss = QCursor::pos();
            ss = cell->mapFromGlobal(QCursor::pos());
            if (m_days[pos].year() >= 1900) {
                if (!fillRect.contains(ss)) {
                    m_createDate = m_days[pos];
                    slotCreate();
                } else {
                    emit signalsViewSelectDate(m_days[pos]);
                }
            }

        } else if (e->type() == QEvent::MouseButtonRelease) {
            const int pos = m_cellList.indexOf(cell);
            m_cellfoceflag[pos] = false;
            m_cellList[pos]->update();
            m_updateflag = true;
        } //else if (e->type() == QEvent::FocusIn) {
        //  const int pos = m_cellList.indexOf(cell);
        //  m_cellfoceflag[pos] = true;
        //  m_cellList[pos]->update();
        //}
        else if (e->type() == QEvent::Leave) {
            const int pos = m_cellList.indexOf(cell);
            m_cellhoverflag[pos] = false;
            m_cellList[pos]->update();
            m_tooltipview->hide();
        } else if (e->type() == QEvent::Enter) {
            const int pos = m_cellList.indexOf(cell);
            m_cellhoverflag[pos] = true;
            m_cellList[pos]->update();
        } /*else if (e->type() == QEvent::ToolTip) {
            if (m_showState & ShowLunar) {
                const int pos = m_cellList.indexOf(cell);
                if (getShowSolarDayByDate(m_days[pos])) {
                    QRect fillRect = QRect(6, 34, cell->width() - 12, 22);
                    QPoint ss = QCursor::pos();
                    ss = cell->mapFromGlobal(QCursor::pos());
                    if (fillRect.contains(ss)) {
                        CaLunarDayInfo dayInfo = getCaLunarDayInfo(pos);
                        if (!dayInfo.mSolarFestival.isEmpty() || !dayInfo.mLunarFestival.isEmpty()) {
                            QString str =  dayInfo.mSolarFestival;
                            if (!dayInfo.mSolarFestival.isEmpty() && dayInfo.mLunarFestival.isEmpty()) {
                                str = dayInfo.mSolarFestival;
                            } else if (!dayInfo.mSolarFestival.isEmpty() && !dayInfo.mLunarFestival.isEmpty()) {
                                str = dayInfo.mSolarFestival + " " + dayInfo.mLunarFestival;
                            } else {
                                str = dayInfo.mLunarFestival;
                            }
                            m_tooltipview->setText(str);
                            int px = cell->x();
                            int py = cell->y();
                            QPoint pos22 = mapToGlobal(QPoint(px, py));
                            m_tooltipview->move(pos22.x() + ss.x(), pos22.y() + m_weekIndicator->height() + ss.y() + 22);
                            m_tooltipview->show();
                        }
                    }
                }
            }
        }*/
    }

    return false;
}
void CMonthView::slotCreate()
{
    emit signalViewtransparentFrame(1);
    CSchceduleDlg dlg(1, this);
    QDateTime tDatatime;
    tDatatime.setDate(m_createDate);
    if (m_createDate == QDate::currentDate()) {
        tDatatime.setTime(QTime::currentTime());
    } else {
        tDatatime.setTime(QTime(8, 0));
    }
    dlg.setDate(tDatatime);
    dlg.setAllDay(true);
    if (dlg.exec() == DDialog::Accepted) {
        slotSchceduleUpdate();
        emit signalsSchceduleUpdate(0);
    }
    emit signalViewtransparentFrame(0);
}
void CMonthView::updateDate()
{
    const QDate firstDay(m_currentDate.year(), m_currentDate.month(), 1);
    const int day = (firstDay.dayOfWeek() + m_firstWeekDay) % 7;
    const int currentIndex = day + m_currentDate.day() - 1;

    if (currentIndex < 0) {
        return;
    }
    for (int i(0); i != 42; ++i) {
        m_days[i] = firstDay.addDays(i - day);
        if (m_days[i].month() != m_currentDate.month()) continue;
        //更新日程
        //m_cellScheduleList[i]->setDate(m_days[i]);
    }
    if (m_updateflag) {
        parentWidget()->setEnabled(false);
        emit signalsupdatescheduleD(this, m_days[0], m_days[41]);
    }
    setSelectedCell(currentIndex);
    update();
}

void CMonthView::updateCurrentLunar(const CaLunarDayInfo &info)
{
    if (!info.mLunarFestival.isEmpty()) {
        emit currentFestivalChanged(info.mLunarFestival);
    } else if (!info.mTerm.isEmpty()) {
        emit currentFestivalChanged(info.mTerm);
    } else if (!info.mSolarFestival.isEmpty()) {
        QStringList tmpFestival = info.mSolarFestival.split(" ");

        if (tmpFestival.length() >= 3) {
            emit currentFestivalChanged(QString("%1 %2").arg(tmpFestival[0]).arg(tmpFestival[1]));
        } else {
            emit currentFestivalChanged(info.mSolarFestival);
        }
    } else {
        emit currentFestivalChanged("");
    }
    emit signalcurrentLunarDateChanged(m_currentDate, getCaLunarDayInfo(getDateIndex(m_currentDate)), 1);
    updateDate();
}

char CMonthView::getFestivalInfoByDate(const QDate &date)
{
    for (int i = 0; i < m_festivallist.count(); i++) {
//        if (date.year() == m_festivallist[i].year && date.month() == m_festivallist[i].month) {
        for (int j = 0; j < m_festivallist[i].listHoliday.count(); j++) {

            if (m_festivallist[i].listHoliday[j].date == date) {
                return m_festivallist[i].listHoliday[j].status;
            }
        }
//        }
    }
    return 0;
}

bool CMonthView::getShowSolarDayByDate(const QDate &date)
{
    bool tflag = true;
    for (int i = 0; i < m_shceludelistdata.count(); i++) {
        if (date == m_shceludelistdata.at(i).date) {
            tflag = m_shceludelistdata.at(i).vData.isEmpty();
            break;
        }
    }
    return tflag;
}

const QString CMonthView::getCellDayNum(int pos)
{
    //if (m_days[pos].day() ==  1) {
    // return QString::number(m_days[pos].month()) + "/" + QString::number(m_days[pos].day());
    //}
    return QString::number(m_days[pos].day());
}

const QDate CMonthView::getCellDate(int pos)
{
    return m_days[pos];
}

const QString CMonthView::getLunar(int pos)
{
    CaLunarDayInfo info = getCaLunarDayInfo(pos);

    if (info.mLunarDayName == "初一") {
        info.mLunarDayName = info.mLunarMonthName + info.mLunarDayName;
    }

    if (info.mTerm.isEmpty())
        return info.mLunarDayName;

    return info.mTerm;
}

const CaLunarDayInfo CMonthView::getCaLunarDayInfo(int pos)
{
    const QDate date = m_days[pos];

    if (lunarCache->contains(date)) {
        return lunarCache->value(date);
    }

    if (lunarCache->size() > 60)
        lunarCache->clear();

//    QTimer::singleShot(500, [this, pos] {getDbusData(pos);});
    queue->push_back(pos);

    QTimer::singleShot(300, this, SLOT(getDbusData()));
    return *emptyCaLunarDayInfo;
}

void CMonthView::getDbusData()
{
    if (queue->isEmpty())
        return;

    const int pos = queue->head();
    queue->pop_front();
    const QDate date = m_days[pos];
    if (!date.isValid()) {
        return;
    }

    CaLunarDayInfo currentDayInfo;
    if (!lunarCache->contains(date)) {
        bool o1 = true;
        QDBusReply<CaLunarMonthInfo> reply = m_DBusInter->GetLunarMonthCalendar(date.year(), date.month(), false, o1);
//        QDBusReply<CaLunarDayInfo> replydd = m_DBusInter->GetLunarInfoBySolar(date.year(), date.month(), date.day(), o1);

        QDate cacheDate;
        cacheDate.setDate(date.year(), date.month(), 1);
        foreach (const CaLunarDayInfo &dayInfo, reply.value().mCaLunarDayInfo) {
            lunarCache->insert(cacheDate, dayInfo);
            if (date == m_currentDate) {
                currentDayInfo = dayInfo;
            }
            cacheDate = cacheDate.addDays(1);
        }
    } else {
        currentDayInfo = lunarCache->value(date);
    }

    m_cellList.at(pos)->update();

    // refresh   lunar info
    if (date == m_currentDate) {
        updateCurrentLunar(currentDayInfo);
    }
}

void CMonthView::paintCell(QWidget *cell)
{
    const QRect rect(0, 0, cellwidth, cellheight);

    const int pos = m_cellList.indexOf(cell);
    const int type = getDateType(m_days[pos]);

    const bool isCurrentDay = getCellDate(pos) == QDate::currentDate();

    //设置今天日期背景色为系统活动色
    m_currentColor = CScheduleDataManage::getScheduleDataManage()->getSystemActiveColor();

    QPainter painter(cell);

    int ftype = getFestivalInfoByDate(m_days[pos]);
    painter.save();
    if (m_showState & ShowLunar) {
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        if (ftype == 2) {
            painter.setBrush(QBrush(m_banColor));
        } else if (ftype == 1) {
            painter.setBrush(QBrush(m_xiuColor));
        } else {
            painter.setBrush(QBrush(m_fillColor));
        }

        if (m_currentDate.month() != m_days[pos].month()) {
            painter.setOpacity(0.4);
        }
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect);//画矩形
    } else {
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(QBrush(m_fillColor));
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect);//画矩形
    }
    painter.restore();
    painter.save();
    if (m_cellfoceflag[pos]) {
        QRect fillRect = QRect(0, 0, cellwidth, cellheight);
        painter.setBrush(m_pressColor);
        painter.setPen(Qt::NoPen);
        //painter.drawRoundedRect(fillRect, 8, 8);
        painter.drawRect(fillRect);
    } else if (m_cellhoverflag[pos]) {
        QRect fillRect = QRect(0, 0, cellwidth, cellheight);
        painter.setBrush(m_hoverColor);
        painter.setPen(Qt::NoPen);
        //painter.drawRoundedRect(fillRect, 8, 8);
        painter.drawRect(fillRect);
    }
    painter.restore();

    painter.setPen(Qt::SolidLine);
    painter.setPen(m_wrectColor);
    //painter.setPen(Qt::SolidLine);
    int rectindex = pos % 7;
    if (rectindex == 0) {
        painter.drawLine(0, 0, cellwidth, 0); //画矩形
        painter.drawLine(0, cellheight, cellwidth, cellheight); //画矩形
    } else {
        painter.drawRect(rect);//画矩形
    }



//    painter.drawRoundedRect(cell->rect(), 4, 4);

    // draw selected cell background circle


    painter.setPen(Qt::SolidLine);

    const QString dayNum = getCellDayNum(pos);
    const QString dayLunar = getLunar(pos);

    int hh = 36;
    if (isCurrentDay) {
        //今天
        QFont tfont = m_dayNumFont;
        tfont.setPixelSize(20);
        painter.setFont(tfont);
        painter.setPen(m_dayNumCurrentColor);

        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(QBrush(m_currentColor));
        painter.setPen(Qt::NoPen);
        //根据是否有阴历信息，在不同的位置绘制今天的背景圆
        if (m_showState & ShowLunar)
            painter.drawEllipse(QRectF(this->rect().x() + 7, this->rect().y() + 4, hh - 8, hh - 8));
        else
            painter.drawEllipse(QRectF((this->rect().width() - hh + 8) / 2 + this->rect().x(), this->rect().y() + 4, hh - 8, hh - 8));
        painter.restore();
    }

    QColor daynumcolor;
    if (isCurrentDay) {
        daynumcolor = m_currentDayTextColor;
    } else {
        const int tType = type & 0xff;
        if (tType & SO_MNotCurrentMonth)
            daynumcolor = m_notCurrentTextColor;
        else if (type == SO_MWeekends)
            daynumcolor = m_weekendsTextColor;
        else
            daynumcolor = m_defaultTextColor;
    }
    if (m_cellfoceflag[pos]) {
        daynumcolor.setAlphaF(0.6);
    }
    painter.setPen(daynumcolor);
//    painter.drawRect(rect);
    QRect test;
    painter.setFont(m_dayNumFont);

    if (isCurrentDay) {

        QFont tfont = m_dayNumFont;
        tfont.setPixelSize(20);
        painter.setFont(tfont);
        if (m_showState & ShowLunar) {
            painter.drawText(QRect(3, -2, hh, hh), Qt::AlignCenter, dayNum, &test);
        } else {
            painter.drawText(QRect(0, -2, cell->width(), hh), Qt::AlignCenter, dayNum, &test);
        }

    } else {
        if (m_showState & ShowLunar) {
            painter.drawText(QRect(3, -2, hh, hh), Qt::AlignCenter, dayNum);
        } else {
            painter.drawText(QRect(0, -2, cell->width(), hh), Qt::AlignCenter, dayNum, &test);
        }
    }
// draw text of day type
    if (m_showState & ShowLunar) {
        //if (isCurrentDay) {
        // painter.setPen(m_currentDayLunarColor);
        if (m_showState & ShowLunarFestivalHighlight) {
            const int tType = type & 0xff;
            if (tType & SO_MNotCurrentMonth)
                painter.setPen(m_notCurrentLunarColor);
            else if (tType & SO_MFestival)
                painter.setPen(m_festivalLunarColor);
            else if (tType & SO_MWeekends)
                painter.setPen(m_weekendsLunarColor);
            else
                painter.setPen(m_defaultLunarColor);
        }
        painter.setFont(m_dayLunarFont);
        painter.save();
        QFontMetrics metrics(m_dayLunarFont);
        int Lunarwidth = metrics.width(dayLunar);
        int filleRectX = cell->width() - 12 - 3 - (58 + Lunarwidth) / 2;
        QRect fillRect(filleRectX, 9, 12, 12);
        if (filleRectX > 36) {
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::HighQualityAntialiasing);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            if (m_currentDate.month() != m_days[pos].month()) {
                painter.setOpacity(0.4);
            }
            if (ftype == 2) {
                QPixmap  pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/ban.svg");
                pixmap.setDevicePixelRatio(devicePixelRatioF());
                painter.drawPixmap(fillRect, pixmap);
            } else if (ftype == 1) {
                QPixmap pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/xiu.svg");
                pixmap.setDevicePixelRatio(devicePixelRatioF());
                painter.drawPixmap(fillRect, pixmap);
            }
        }

        painter.restore();
        painter.drawText(QRect(cell->width() - 58, 6, 58, 18), Qt::AlignCenter, dayLunar);
    }

    painter.end();
}

void CMonthView::cellClicked(QWidget *cell)
{
    if (!m_cellSelectable)
        return;

    const int pos = m_cellList.indexOf(cell);
    if (pos == -1)
        return;

    setSelectedCell(pos);
//    emit signalcurrentLunarDateChanged(m_days[pos], getCaLunarDayInfo(getDateIndex(m_days[pos])), 0);
//    // my gift eggs
//    static int gift = 0;
//    if (m_days[pos] == QDate(1993, 7, 28))
//        if (++gift == 10)
//            QMessageBox::about(this, "LinuxDeepin", "by shibowen <sbw@sbw.so> :P");
}

void CMonthView::setSelectedCell(int index)
{
//    if (m_selectedCell == index)
//        return;

    const int prevPos = m_selectedCell;
    m_selectedCell = index;

    m_cellList.at(prevPos)->update();
    m_cellList.at(index)->update();
    if (m_days[index].year() < 1900) return;
    QDate  clickDate = m_days[index];
    emit signalcurrentLunarDateChanged(clickDate, getCaLunarDayInfo(getDateIndex(clickDate)), 0);
    emit dateSelected(clickDate, getCaLunarDayInfo(index));
}
