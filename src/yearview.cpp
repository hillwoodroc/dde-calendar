
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

#include "calendardbus.h"

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QEvent>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QQueue>
#include "yearview.h"
#include <QSpacerItem>
#include <DPalette>
#include <QLocale>
#include <DHiDPIHelper>
#include "scheduledatamanage.h"
#include "yearschceduleview.h"
#include <QDesktopWidget>
#include <DApplication>
#include <QPainterPath>
#include <QMouseEvent>

#include <DArrowRectangle>
DGUI_USE_NAMESPACE
CYearSchceduleOutView      *CYearView::m_Scheduleview = nullptr;
CYearView::CYearView(QWidget *parent) : CustomFrame(parent)
{
    m_dayNumFont.setFamily("Helvetica");
    m_dayNumFont.setPixelSize(12);
    setMouseTracking(true);
    //m_dayNumFont.setWeight(QFont::Light);

    //setStyleSheet("QWidget { background: rgba(0, 0, 0, 0) }");

    //add separator line
    m_currentMouth = new CustomFrame();
    m_currentMouth->setFixedHeight(24);
    m_currentMouth->setContentsMargins(0, 0, 0, 0);
    //m_currentMouth->setMinimumWidth(100);
    m_currentMouth->setTextAlign(Qt::AlignLeft);
    //m_currentMouth->setStyleSheet("border: 1px solid rgba(0, 0, 0, 0.05);");

    m_momthFont.setFamily("SourceHanSansSC");
    m_momthFont.setWeight(QFont::Medium);
    m_momthFont.setPixelSize(16);
    m_currentMouth->setTextFont(m_momthFont);
    //DPalette Lunadpa = m_currentMouth->palette();
    //Lunadpa.setColor(DPalette::WindowText, QColor("#CF0059"));
    //m_currentMouth->setPalette(Lunadpa);
    QHBoxLayout *separatorLineLayout = new QHBoxLayout;
    separatorLineLayout->setMargin(0);
    separatorLineLayout->setSpacing(0);
    separatorLineLayout->setContentsMargins(0, 0, 0, 0);
    //separatorLineLayout->addSpacing(13);
    separatorLineLayout->addWidget(m_currentMouth);
    separatorLineLayout->addStretch();
    // QSpacerItem *t_spaceitem = new QSpacerItem(30, 32, QSizePolicy::Expanding, QSizePolicy::Fixed);
    //separatorLineLayout->addSpacerItem(t_spaceitem);
    m_currentMouth->installEventFilter(this);

    // cells grid
    m_gridLayout = new QGridLayout;
    m_gridLayout->setMargin(0);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setHorizontalSpacing(6);
    m_gridLayout->setVerticalSpacing(3);
    for (int r = 0; r != 6; ++r) {
        for (int c = 0; c != 7; ++c) {
            QWidget *cell = new QWidget(this);
//            cell->setFixedSize(cellwidth, cellheight);
            cell->installEventFilter(this);
            //cell->setFocusPolicy(Qt::ClickFocus);
            cell->setFocusPolicy(Qt::NoFocus);
            m_gridLayout->addWidget(cell, r, c);
            m_cellList.append(cell);
        }
    }
    m_hhLayout = new QVBoxLayout;
    m_hhLayout->addLayout(separatorLineLayout);
    m_hhLayout->addLayout(m_gridLayout);
    m_hhLayout->setMargin(0);
    m_hhLayout->setSpacing(0);
    m_hhLayout->setContentsMargins(13, 10, 10, 10);

    setLayout(m_hhLayout);
    connect(this, &CYearView::dateSelected, this, &CYearView::handleCurrentDateChanged);
    m_hightFont.setFamily("Helvetica");
    m_hightFont.setPixelSize(12);

    createYearSchceduleView(parent);

    lightColor.hoverColor = "#000000";
    lightColor.hoverColor.setAlphaF(0.05);
    lightColor.pressColor = "#000000";
    lightColor.pressColor.setAlphaF(0.2);
    lightColor.normalColor = "#FFFFFF";
    lightColor.normalColor.setAlphaF(1);

    darkColor.hoverColor = "#FFFFFF";
    darkColor.hoverColor.setAlphaF(0.1);
    darkColor.pressColor = "#252525";
    darkColor.pressColor.setAlphaF(1);
    darkColor.normalColor = "#000000";
    darkColor.normalColor.setAlphaF(0.05);
    // m_Scheduleview = new CYearSchceduleView(this);


//    static DArrowRectangle *arr = new DArrowRectangle(DArrowRectangle::ArrowLeft, DArrowRectangle::FloatWidget, parent);
//    arr->setFixedSize(200, 100);
    //m_Scheduleview->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    //m_Scheduleview->setAttribute(Qt::WA_TranslucentBackground);
    //m_Scheduleview->setWindowFlags(m_Scheduleview->windowFlags()& Qt::WindowStaysOnTopHint);
    //m_Scheduleview->setVisible(false);
}
void CYearView::handleCurrentDateChanged(const QDate date, const CaLunarDayInfo &detail)
{
    return;
    Q_UNUSED(detail);


    if (date != m_currentDate) {
        setCurrentDate(date);
    }
}

void CYearView::updateInfoWIndow(bool flag)
{
    if (flag) {
        m_Scheduleview->show(0, 0);
    } else {
        m_Scheduleview->hide();
    }
}

void CYearView::slotupdateSchedule(const int id)
{
    setCurrentDate(m_currentDate, id);
}

void CYearView::setFirstWeekday(int weekday)
{
    m_firstWeekDay = weekday;
    updateDate();
}


void CYearView::updateSelectState()
{
    m_selectedCell = -1;
    for (int i(0); i != 42; ++i) {
        m_cellList.at(i)->update();
    }
}

void CYearView::setTheMe(int type)
{
    m_themetype = type;
    if (type == 0 || type == 1) {
        DPalette bpa = palette();
        bpa.setColor(DPalette::Background, Qt::white);
        setPalette(bpa);
        setBackgroundRole(DPalette::Background);
        // setBColor( Qt::white);
        m_currentMouth->setTextColor(QColor("#CF0059"));

        m_bnormalColor = "#FFFFFF";
        m_topBorderColor = Qt::red;
        m_backgroundCircleColor = "#0081FF";

        m_defaultTextColor = Qt::black;
        m_currentDayTextColor = "#0081FF";
        m_weekendsTextColor = Qt::black;
        m_selectedTextColor = Qt::white;
        m_festivalTextColor = Qt::black;
        m_notCurrentTextColor = "#b2b2b2";
        m_ceventColor = QColor(255, 93, 0);

    } else if (type == 2) {

        m_bnormalColor = "#FFFFFF";
        m_bnormalColor.setAlphaF(0.05);
        //setBColor(m_bnormalColor);
        m_currentMouth->setTextColor(QColor("#BF1D63"));
        // m_currentMouth->setBColor(framecolor);

        m_topBorderColor = Qt::red;
        m_backgroundCircleColor = "#0059D2";

        m_defaultTextColor = "#C0C6D4";
        m_currentDayTextColor = "#0059D2";
        m_weekendsTextColor = Qt::black;
        m_selectedTextColor = Qt::white;
        m_festivalTextColor = Qt::black;
        m_notCurrentTextColor = "#C0C6D4";
        m_notCurrentTextColor.setAlphaF(0.5);
        m_ceventColor = QColor(204, 77, 3);
    }
    QColor monthcolor = Qt::white;
    monthcolor.setAlphaF(0);
    m_currentMouth->setBColor(monthcolor);
    m_Scheduleview->setTheMe(type);
}

void CYearView::updateHigh()
{
    for (int i = 0; i < 42; i++) {
        m_cellList.at(i)->update();
    }
}

void CYearView::SchceduleViewHide()
{
//    if(m_Scheduleview->)
    m_Scheduleview->hide();
}

void CYearView::setCurrentDate(const QDate date, int type)
{
    qDebug() << "set current date " << date;

    //if (date == m_currentDate) {
    //   return;
    // }

    m_currentDate = date;
    CScheduleDataManage *tdataManage = CScheduleDataManage::getScheduleDataManage();
    QLocale locale;
    if (locale.language() == QLocale::Chinese) {
        m_vlineflag = tdataManage->getHuangliDayDataManage()->getDayFlag(m_currentDate);
    } else {
        m_vlineflag.resize(42);
        m_vlineflag.fill(false);
    }
    m_currentMouth->setTextStr(locale.monthName(date.month(), QLocale::ShortFormat));
    updateDate();

    QVector<ScheduleDateRangeInfo> out;
    if (tdataManage->getscheduleDataCtrl()->getScheduleInfo(m_days[0], m_days[41], out)) {
        if (out.count() == 42)
            for (int i = 0; i < 42; i++) {
                if (!out.at(i).vData.isEmpty()) {
                    m_vlineflag[i] = true;
                }
            }
    }
    if (type == 1)
        setSelectedCell(getDateIndex(date));
}

void CYearView::setCellSelectable(bool selectable)
{
    if (selectable == m_cellSelectable)
        return;
    m_cellSelectable = selectable;
}

int CYearView::getDateIndex(const QDate &date) const
{
    for (int i = 0; i != 42; ++i)
        if (m_days[i] == date)
            return i;

    return 0;
}

bool CYearView::eventFilter(QObject *o, QEvent *e)
{
    QWidget *cell = qobject_cast<QWidget *>(o);
    QMouseEvent *event = dynamic_cast<QMouseEvent *>(e);

    int ps = m_cellList.indexOf(cell);
    if (m_days[ps].year() < 1900) return true;
    if (cell && m_cellList.contains(cell)) {
        if (e->type() == QEvent::Paint) {
            paintCell(cell);
        } else if (e->type() == QEvent::MouseButtonPress) {
            if (event->button() == Qt::RightButton)
                return false;

            m_selectFlag = true;
            cellClicked(cell);
            const int pos = m_cellList.indexOf(cell);
            m_cellEventType[pos] = CellPress;
            cell->update();
            m_selectDate = m_days[pos];
            if (1) {
                emit signalHideInfo();
                emit signalSelectInfo(true);
                m_Scheduleview->hide();
                m_Scheduleview->clearData();
                CScheduleDataManage *tdataManage = CScheduleDataManage::getScheduleDataManage();
                QString soloday;
                QLocale locale;
//                if (locale.language() == QLocale::Chinese) {
//                    if (tdataManage->getHuangliDayDataManage()->getSoloDay(m_days[pos], soloday)) {
//                        m_Scheduleview->setSoloDay(soloday);
//                    }
//                }
                m_Scheduleview->setCurrentDate(m_days[pos]);
                QVector<ScheduleDateRangeInfo> out;
                if (tdataManage->getscheduleDataCtrl()->getScheduleInfo(m_days[pos], m_days[pos], out)) {
                    if (!out.isEmpty()) {
                        m_Scheduleview->setData(out[0].vData);
                    }
                }
                //QPoint pos22 = mapToGlobal(QPoint(px, py));
                QPoint pos22 = QCursor::pos();

                QDesktopWidget *w = QApplication::desktop();
                QRect wR = w->screenGeometry(w->primaryScreen());

                m_Scheduleview->showWindow();
//                int lfetorright = 0;
//                int mw = pos22.x() + 10 + m_Scheduleview->width();
//                if (mw > wR.width()) {
//                    mw = pos22.x() - 10 - m_Scheduleview->width();
//                    lfetorright = 1;
//                } else {
//                    mw = pos22.x() + 10;
//                    lfetorright = 0;
//                }
//                int mh = pos22.y() + m_Scheduleview->height();
//                if (mh > wR.height()) {
//                    mh = wR.height() - m_Scheduleview->height();
//                    m_Scheduleview->setDtype(lfetorright, pos22.y() - mh);
//                } else {
//                    mh = pos22.y() - m_Scheduleview->height() / 2;
//                    m_Scheduleview->setDtype(lfetorright, m_Scheduleview->height() / 2);
//                }
//                m_Scheduleview->move(mw, mh);
                if (pos22.x() + 10 + m_Scheduleview->width() < w->width()) {
                    m_Scheduleview->setArrowDirection(DArrowRectangle::ArrowLeft);
                    m_Scheduleview->show(pos22.x() + 10, pos22.y());
                } else {
                    m_Scheduleview->adjustPosition(true);
                    m_Scheduleview->setArrowDirection(DArrowRectangle::ArrowRight);
                    m_Scheduleview->show(pos22.x() - 10, pos22.y());
                }


                cell->update();
            } else {
                m_Scheduleview->show(0, 0);

            }

        } else if (e->type() == QEvent::MouseButtonRelease) {
            const int pos = m_cellList.indexOf(cell);
            m_selectFlag = false;
            emit signalSelectInfo(m_selectFlag);
            cell->update();

        } else if (e->type() == QEvent::MouseButtonDblClick) {
            const int pos = m_cellList.indexOf(cell);
            if (pos != -1 && event->button() == Qt::LeftButton) {
                emit signaldoubleclickDate(m_days[pos]);
            }
        }
        if (e->type() == QEvent::Leave) {
            const int pos = m_cellList.indexOf(cell);
            m_cellEventType[pos] = CellNormal;
            m_cellList[pos]->update();
            // m_Scheduleview->hide();
            // m_Scheduleview->clearData();
        } else if (e->type() == QEvent::ToolTip) {
        }
        if (e->type() == QEvent::Enter) {
            const int pos = m_cellList.indexOf(cell);
            m_cellEventType[pos] = Cellhover;
            cell->update();
        }
    }
    if (cell == m_currentMouth) {
        if (e->type() == QEvent::MouseButtonDblClick) {
            //emit signalselectMonth(QDate(m_currentDate.year(), m_currentDate.month(), 1));
            emit signalselectMonth(m_currentDate);
        }
    }
    return false;
}

void CYearView::updateDate()
{
    const QDate firstDay(m_currentDate.year(), m_currentDate.month(), 1);
    const int day = (firstDay.dayOfWeek() + m_firstWeekDay) % 7;
    const int currentIndex = day + m_currentDate.day() - 1;

    if (currentIndex < 0) {
        return;
    }

    for (int i(0); i != 42; ++i) {
        m_days[i] = firstDay.addDays(i - day);
        m_cellList.at(i)->update();
    }

    //setSelectedCell(currentIndex);
    update();
}

void CYearView::createYearSchceduleView(QWidget *parent)
{
    if (m_Scheduleview == nullptr) {
        m_Scheduleview = new CYearSchceduleOutView(parent);
    }

}
const QString CYearView::getCellDayNum(int pos)
{
    return QString::number(m_days[pos].day());
}

const QDate CYearView::getCellDate(int pos)
{
    return m_days[pos];
}
void CYearView::paintCell(QWidget *cell)
{
    //高亮的颜色
    m_highColor = CScheduleDataManage::getScheduleDataManage()->getSystemActiveColor();
    const QRect rect(0, 0, cell->width(), cell->height());

    const int pos = m_cellList.indexOf(cell);
    // bool isSelectedCell = pos == m_selectedCell;
    const bool isCurrentDay = getCellDate(pos) == QDate::currentDate() && getCellDate(pos).month() == m_currentDate.month();
    bool isSelectedCell  = false;
    if (m_days[pos] == QDate::currentDate()) {
        isSelectedCell = true;
    } /*else if (m_selectFlag) {
        if (m_days[pos] == m_selectDate) {
            isSelectedCell = true;
        }
    }*/

    QPainter painter(cell);
    painter.setRenderHints(QPainter::Antialiasing);
//    m_cellBackgroundColor =

    CellColor currentColor;
    const int r = cell->width() > cell->height() ? cell->height() * 0.9 : cell->width() * 0.9 ;
    int fontsize = 12 + (r - 18) * 6 / 17;
    if (fontsize < 12) {
        fontsize = 12;
    }
    m_dayNumFont.setPixelSize(fontsize);
    m_hightFont.setPixelSize(fontsize);
    const int x = (cell->width() - r) / 2;
    const int y = (cell->height() - r) / 2;
    QRect fillRect = QRect(x, y, r, r);

    if (m_themetype == 2) {
        currentColor = darkColor;
    } else {
        currentColor = lightColor;
    }
    if (m_cellEventType[pos] == CellPress) {
        m_cellBackgroundColor = currentColor.pressColor;
    } else if (m_cellEventType[pos] == Cellhover) {
        m_cellBackgroundColor = currentColor.hoverColor;
    } else {
        m_cellBackgroundColor = currentColor.normalColor;
    }
    if (m_themetype == 2 &&  m_cellEventType[pos] != CellPress) {

    } else {
        painter.setBrush(m_cellBackgroundColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(fillRect);
    }
    if (m_themetype == 2 &&  m_cellEventType[pos] == Cellhover) {
        painter.setBrush(m_cellBackgroundColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(fillRect);
    }


    bool highflag = false;
    if (getCellDate(pos).month() == m_currentDate.month()) {
        highflag = CScheduleDataManage::getScheduleDataManage()->getSearchResult(m_days[pos]);
    }
    if (highflag) {
        int hh = 0;
        QRect fillRect;
        int t_pix = (height() - 159) / 22.33;
        if (cell->width() > cell->height()) {
            hh = cell->height() - t_pix;
            fillRect = QRect((cell->width() - hh) / 2.0 + 0.5 + t_pix / 2, 0, hh, hh);
        } else {
            hh = cell->width() - t_pix;
            fillRect = QRect(t_pix / 2, (cell->height() - hh) / 2.0 + 0.5, hh, hh);
        }

        painter.setBrush(QBrush(m_highColor));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(fillRect);
        painter.setPen(Qt::SolidLine);

        const QString dayNum = getCellDayNum(pos);
        painter.setPen(m_highTextColor);
        painter.setFont(m_hightFont);
        painter.drawText(fillRect, Qt::AlignCenter, dayNum);

    } else {
        // draw selected cell background circle
        if (isSelectedCell) {
            //如果选择了该日期，则设置高亮
            painter.save();
            painter.setBrush(QBrush(m_highColor));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(fillRect);
            painter.restore();
        }

        painter.setPen(Qt::SolidLine);

        const QString dayNum = getCellDayNum(pos);

        // draw text of day
        if (isSelectedCell) {
            painter.setPen(m_selectedTextColor);
        } else if (isCurrentDay) {
            painter.setPen(m_currentDayTextColor);
        } else {
            if (m_currentDate.month() == getCellDate(pos).month())
                painter.setPen(m_defaultTextColor);
            else
                painter.setPen(m_notCurrentTextColor);
        }
        painter.setFont(m_dayNumFont);
        painter.drawText(fillRect, Qt::AlignCenter, dayNum);
        if (m_vlineflag.count() == 42) {
            if (m_vlineflag[pos]) {
                painter.save();
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setRenderHint(QPainter::HighQualityAntialiasing);
                painter.setRenderHint(QPainter::SmoothPixmapTransform);
                QPen pen;
                pen.setWidth(2);
                pen.setColor(m_ceventColor);
                painter.setPen(pen);
                painter.setBrush(QBrush(m_ceventColor));
                painter.setPen(Qt::NoPen);
                int r = cell->width() * (4 / 25);
                if (r < 4) {
                    r = 4;
                } else if (r > 7) {
                    r = 7;
                }
                painter.drawEllipse(cell->width() - r, 0, r, r);
                painter.restore();
            }
        }
    }
    painter.end();
}

void CYearView::cellClicked(QWidget *cell)
{
    if (!m_cellSelectable)
        return;

    const int pos = m_cellList.indexOf(cell);
    if (pos == -1)
        return;

    setSelectedCell(pos);
    emit signalcurrentDateChanged(m_days[pos]);
}

void CYearView::setSelectedCell(int index)
{
    if (m_selectedCell == index)
        return;
    m_selectedCell = index;
    updateDate();
    if (m_days[index].year() < 1900) return;
    emit singanleActiveW(this);
}

void CYearView::resizeEvent(QResizeEvent *event)
{
    //cellwidth = width() * 0.099 + 0.5;
    //cellheight = height() * 0.1257 + 0.5;

    m_gridLayout->setHorizontalSpacing(width() * 0.0297 + 0.5);
    m_gridLayout->setVerticalSpacing(height() * 0.0034 + 0.5);
    int leftmagin = width() * 0.06435 + 0.5;
    int rightmagin = leftmagin;
    int topmagin = height() * 0.02955 + 0.5;
    int buttonmagin = height() * 0.044 + 0.5;
//    cellwidth = (width() - 2 * leftmagin) / 7;
//    cellheight = (height() -  (24 + (height() - 159) / 12 - topmagin - buttonmagin)) / 6;
    m_hhLayout->setContentsMargins(leftmagin, topmagin, rightmagin, buttonmagin);
//    m_dayNumFont.setPixelSize(12 + (height() - 159) / 22.33);
//    m_hightFont.setPixelSize(12 + (height() - 159) / 22.33);
    m_momthFont.setPixelSize(16 + (height() - 159) / 16.75);
    m_currentMouth->setTextFont(m_momthFont);
    m_currentMouth->setFixedHeight(24 + (height() - 159) / 12);
    m_currentMouth->update();
//    for (int i(0); i != 42; ++i) {
//        m_cellList.at(i)->setFixedSize(cellwidth, cellheight);
//    }
    QFrame::resizeEvent(event);
}

void CYearView::mousePressEvent(QMouseEvent *event)
{
    if (m_selectFlag) return;
    if (event->button() == Qt::LeftButton) {
        emit signalHideInfo();
    }
    CustomFrame::mousePressEvent(event);
}

void CYearView::mouseMoveEvent(QMouseEvent *event)
{
    if (mouse_press) {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        //this->move(move_point);
    }
    //CustomFrame::mouseMoveEvent(event);
}

void CYearView::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_press = false;
    CustomFrame::mouseReleaseEvent(event);
}

void CYearView::paintEvent(QPaintEvent *e)
{
    int labelwidth = width() - 2 * m_borderframew;
    int labelheight = height() - 2 * m_borderframew;

    QPainter painter(this);
    if (1) {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
        painter.setBrush(QBrush(m_bnormalColor));
        painter.setPen(Qt::NoPen);
        QPainterPath painterPath;
        painterPath.moveTo(m_radius, m_borderframew);
        painterPath.arcTo(QRect(m_borderframew, m_borderframew, m_radius * 2, m_radius * 2), 90, 90);

        painterPath.lineTo(0, labelheight - m_radius);
        painterPath.arcTo(QRect(m_borderframew, labelheight - m_radius * 2, m_radius * 2, m_radius * 2), 180, 90);

        painterPath.lineTo(labelwidth - m_radius, labelheight);
        painterPath.arcTo(QRect(labelwidth - m_radius * 2, labelheight - m_radius * 2, m_radius * 2, m_radius * 2), 270, 90);

        painterPath.lineTo(labelwidth, m_radius);

        painterPath.arcTo(QRect(labelwidth - m_radius * 2, m_borderframew, m_radius * 2, m_radius * 2), 0, 90);

        painterPath.lineTo(m_radius, m_borderframew);
        painterPath.closeSubpath();
        painter.drawPath(painterPath);
        painter.restore();
    }
}
