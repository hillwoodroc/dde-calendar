/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
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
#include "alldayschceduleweekview.h"
#include <QAction>
#include <QMenu>
#include <QListWidget>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QStylePainter>
#include <QRect>
#include "schceduledlg.h"
#include "myschceduleview.h"
#include "scheduledatamanage.h"
#include <DMessageBox>
#include <DPushButton>
#include <DHiDPIHelper>
#include <DPalette>
#include <QGraphicsOpacityEffect>
#include "schedulecoormanage.h"
#include "schcedulectrldlg.h"

DGUI_USE_NAMESPACE
CAllDaySchceduleWeekWidgetItem::CAllDaySchceduleWeekWidgetItem(QWidget *parent /*= nullptr*/, int edittype)
    : DPushButton(parent)
{
    m_editType = edittype;
    //setMargin(0);
    m_editAction = new QAction(tr("Edit"), this);
    m_deleteAction = new QAction(tr("Delete"), this);
    connect(m_editAction, SIGNAL(triggered(bool)), this, SLOT(slotEdit()));
    connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(slotDelete()));
    m_createAction = new QAction(tr("New event"), this);
    connect(m_createAction, &QAction::triggered, this, &CAllDaySchceduleWeekWidgetItem::slotCreate);
    m_item = nullptr;
}

void CAllDaySchceduleWeekWidgetItem::setColor(QColor color1, QColor color2, bool GradientFlag /*= false*/)
{
    m_color1 = color1;
    m_color2 = color2;
    m_GradientFlag = GradientFlag;
}

void CAllDaySchceduleWeekWidgetItem::setText(QColor tcolor, QFont font, QPoint pos, bool avgeflag)
{
    m_textcolor = tcolor;
    m_font = font;
    m_pos = pos;
    m_avgeflag = avgeflag;
}

void CAllDaySchceduleWeekWidgetItem::getColor(QColor &color1, QColor &color2, bool &GradientFlag)
{
    color2 = m_color2;
    color1 = m_color1;
    GradientFlag = m_GradientFlag;
}

void CAllDaySchceduleWeekWidgetItem::getText(QColor &tcolor, QFont &font, QPoint &pos)
{
    tcolor = m_textcolor;
    font = m_font;
    pos = m_pos;
}

void CAllDaySchceduleWeekWidgetItem::setData(ScheduleDtailInfo vScheduleInfo)
{
    m_ScheduleInfo = vScheduleInfo;
    // setToolTip(m_ScheduleInfo.titleName);
    update();
}

void CAllDaySchceduleWeekWidgetItem::setTransparentB(bool t, QColor tcolor)
{
    m_transparentcolor = tcolor;
    m_transparentf = t;
}
void CAllDaySchceduleWeekWidgetItem::slotCreate()
{
    emit signalViewtransparentFrame(1);
    CSchceduleDlg dlg(1, this);
    QDateTime tDatatime;
    tDatatime.setDate(m_dianjiDay);
    if (m_dianjiDay == QDate::currentDate()) {
        tDatatime.setTime(QTime::currentTime());
    } else {
        tDatatime.setTime(QTime(8, 0));
    }
    dlg.setDate(tDatatime);
    dlg.setAllDay(true);
    if (dlg.exec() == DDialog::Accepted) {
        emit signalsEdit(this, 1);
    }
    emit signalViewtransparentFrame(0);
}

void CAllDaySchceduleWeekWidgetItem::slotEdit()
{
    emit signalViewtransparentFrame(1);
    CSchceduleDlg dlg(0, this);
    dlg.setData(m_ScheduleInfo);
    if (dlg.exec() == DDialog::Accepted) {

        emit signalsEdit(this, 1);
    }
    emit signalViewtransparentFrame(0);
}

void CAllDaySchceduleWeekWidgetItem::slotDelete()
{
    emit signalViewtransparentFrame(1);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    if (m_ScheduleInfo.rpeat == 0) {
        CSchceduleCtrlDlg msgBox(this);
        //msgBox.setWindowFlags(Qt::FramelessWindowHint);
        // msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34, 34) * devicePixelRatioF()));

        msgBox.setText(tr("You are deleting an event."));
        msgBox.setInformativeText(tr("Are you sure you want to delete this event?"));
        //设置按钮文字
        msgBox.addPushButton(tr("Cancel"), true);
        msgBox.addWaringButton(tr("Delete"), true);
        msgBox.exec();
        if (msgBox.clickButton() == 0) {
            emit signalViewtransparentFrame(0);
            return;
        } else if (msgBox.clickButton() == 1) {
            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(m_ScheduleInfo.id);
        }
    } else {
        if (m_ScheduleInfo.RecurID == 0) {
            CSchceduleCtrlDlg msgBox(this);
            //msgBox.setWindowFlags(Qt::FramelessWindowHint);
            //msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34, 34) * devicePixelRatioF()));

            msgBox.setText(tr("You are deleting an event."));
            msgBox.setInformativeText(tr("Do you want to delete all occurrences of this event, or only the selected occurrence?"));
            //设置按钮文字
            msgBox.addPushButton(tr("Cancel"));
            msgBox.addPushButton(tr("Delete All"));
            msgBox.addWaringButton(tr("Delete Only This Event"));
            msgBox.exec();
            //各个按钮功能
            if (msgBox.clickButton() == 0) {
                emit signalViewtransparentFrame(0);
                return;
            } else if (msgBox.clickButton() == 1) {
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(m_ScheduleInfo.id);
            } else if (msgBox.clickButton() == 2) {
                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_ScheduleInfo.id, newschedule);
                newschedule.ignore.append(m_ScheduleInfo.beginDateTime);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            }
        } else {
            CSchceduleCtrlDlg msgBox(this);
            //msgBox.setWindowFlags(Qt::FramelessWindowHint);
            //msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34, 34) * devicePixelRatioF()));
            msgBox.setText(tr("You are deleting an event."));
            msgBox.setInformativeText(tr("Do you want to delete this and all future occurrences of this event, or only the selected occurrence?"));
            //设置按钮文字
            msgBox.addPushButton(tr("Cancel"));
            msgBox.addPushButton(tr("Delete All Future Events"));
            msgBox.addWaringButton(tr("Delete Only This Event"));
            msgBox.exec();
            //各个按钮功能
            if (msgBox.clickButton() == 0) {
                emit signalViewtransparentFrame(0);
                return;
            } else if (msgBox.clickButton() == 1) {
                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_ScheduleInfo.id, newschedule);
                newschedule.enddata.type = 2;
                newschedule.enddata.date = m_ScheduleInfo.beginDateTime.addDays(-1);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            } else if (msgBox.clickButton() == 2) {
                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_ScheduleInfo.id, newschedule);
                newschedule.ignore.append(m_ScheduleInfo.beginDateTime);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            }
        }
    }
    emit signalViewtransparentFrame(0);
    emit signalsDelete(this);
}
//有问题
void CAllDaySchceduleWeekWidgetItem::slotDoubleEvent(int type)
{
    emit signalsEdit(this, 1);
}

void CAllDaySchceduleWeekWidgetItem::paintEvent(QPaintEvent *e)
{
    int labelwidth = width();
    int labelheight = height();
    float avge = 1;
    if (m_avgeflag) {
        avge = 0.5;
    }

    CSchedulesColor gdcolor = CScheduleDataManage::getScheduleDataManage()->getScheduleColorByType(m_ScheduleInfo.type.ID);
    m_highflag = CScheduleDataManage::getScheduleDataManage()->getSearchResult(m_ScheduleInfo);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    QRect drawrect = m_coorManage->getAllDayDrawRegion(m_ScheduleInfo.beginDateTime.date(), m_ScheduleInfo.endDateTime.date());
    QPainter painter(this);
    if (m_GradientFlag) {

        QLinearGradient linearGradient(drawrect.topLeft().x(), 0, drawrect.topRight().x(), 0);

        QColor color1 = gdcolor.gradientFromC;
        QColor color2 = gdcolor.gradientToC;
        QColor textcolor = gdcolor.textColor;
        if (m_hoverflag) {
            color1 = gdcolor.hovergradientFromC;
            color2 = gdcolor.hovergradientToC;
        } else if (m_highflag) {
            color1 = gdcolor.hightlightgradientFromC;
            color2 = gdcolor.hightlightgradientToC;
        }
        if (m_selectflag) {
            color1 = gdcolor.pressgradientFromC;
            color2 = gdcolor.pressgradientToC;
            textcolor.setAlphaF(0.6);
        }
        linearGradient.setColorAt(0, color1);
        linearGradient.setColorAt(1, color2);
        QRect fillRect = drawrect;
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(linearGradient);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(fillRect, 8, 8);

        painter.setFont(m_font);
        painter.setPen(textcolor);
        QFontMetrics fm = painter.fontMetrics();
        QString tStitlename = m_ScheduleInfo.titleName;
        tStitlename.replace("\n", "");
        QString str = tStitlename;

        QString tstr;
        for (int i = 0; i < str.count(); i++) {
            tstr.append(str.at(i));
            int widthT = fm.width(tstr) + 5;
            if (widthT >= fillRect.width() - 13) {
                tstr.chop(2);
                break;
            }
        }
        if (tstr != str) {
            tstr = tstr + "...";
        }

        painter.drawText(QRect(fillRect.topLeft().x() + 13, 2, fillRect.width(), fillRect.height()), Qt::AlignLeft, tstr);
        //if (m_transparentf) {
        //  painter.setBrush(m_transparentcolor);
        //  painter.setPen(Qt::NoPen);
        //  painter.drawRoundedRect(fillRect, 8, 8);
        // }
        if (m_hoverflag && !m_selectflag) {
            QRectF trect = QRectF(fillRect.x() + 0.5, fillRect.y() + 0.5, fillRect.width() - 1, fillRect.height() - 1);
            painter.save();
            painter.setRenderHints(QPainter::Antialiasing);
            QPen pen;
            QColor selcolor;

            if (themetype == 2) {
                selcolor = "#FFFFFF";
            } else {
                selcolor = "#000000";
            }
            selcolor.setAlphaF(0.08);

            pen.setColor(selcolor);
            pen.setWidthF(1);
            pen.setStyle(Qt::SolidLine);
            painter.setBrush(Qt::NoBrush);
            painter.setPen(pen);
            painter.drawRoundedRect(trect, 8, 8);
            painter.restore();
        }
        if (m_selectflag) {
            QColor selcolor = "#000000";
            selcolor.setAlphaF(0.05);
            painter.setBrush(selcolor);
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(fillRect, 8, 8);
        }
    } else {
        QRect fillRect = QRect(2, 2 * avge, labelwidth - 2, labelheight - 2 * avge);
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(m_color1);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(fillRect, 3, 3);

        painter.setFont(m_font);
        painter.setPen(m_textcolor);
        QFontMetrics fm = painter.fontMetrics();
        QString str = m_ScheduleInfo.titleName;
        if (fm.width(str) > width()) {
            int widthT = fm.width(str);
            int singlecharw = widthT * 1.0 / str.count();
            int rcharcount = width() * 1.0 / singlecharw;
            QString tstr;
            for (int i = 0; i < rcharcount - 8; i++) {
                tstr.append(str.at(i));
            }
            str = tstr + "...";
        }
        painter.drawText(QRect(m_pos.x(), m_pos.y(), labelwidth - m_pos.x(), labelheight), Qt::AlignLeft, str);
    }
}
void CAllDaySchceduleWeekWidgetItem::contextMenuEvent(QContextMenuEvent *event)
{
    QRect drawrect = m_coorManage->getAllDayDrawRegion(m_ScheduleInfo.beginDateTime.date(), m_ScheduleInfo.endDateTime.date());
    if (drawrect.contains(event->pos())) {
        DMenu Context(this);
        Context.addAction(m_editAction);
        Context.addAction(m_deleteAction);
        Context.exec(QCursor::pos());
    } else {
        DMenu Context(this);
        Context.addAction(m_createAction);
        m_dianjiDay = m_coorManage->getsDate(mapFrom(this, event->pos()));
        Context.exec(QCursor::pos());
    }
}

void CAllDaySchceduleWeekWidgetItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_editType == 0) return;
    QRect drawrect = m_coorManage->getAllDayDrawRegion(m_ScheduleInfo.beginDateTime.date(), m_ScheduleInfo.endDateTime.date());
    if (drawrect.contains(event->pos())) {
        emit signalViewtransparentFrame(1);
        CMySchceduleView dlg(m_ScheduleInfo, this);
        //        dlg.setSchedules(m_ScheduleInfo);
        connect(&dlg, &CMySchceduleView::signalsEditorDelete, this, &CAllDaySchceduleWeekWidgetItem::slotDoubleEvent);
        dlg.exec();
        emit signalViewtransparentFrame(0);
        disconnect(&dlg, &CMySchceduleView::signalsEditorDelete, this, &CAllDaySchceduleWeekWidgetItem::slotDoubleEvent);
    } else {
        m_dianjiDay = m_coorManage->getsDate(mapFrom(this, event->pos()));
        emit signalViewtransparentFrame(1);
        CSchceduleDlg dlg(1, this);
        QDateTime tDatatime;
        tDatatime.setDate(m_dianjiDay);
        tDatatime.setTime(QTime::currentTime());
        dlg.setDate(tDatatime);
        dlg.setAllDay(true);
        if (dlg.exec() == DDialog::Accepted) {
            emit signalsEdit(this, 1);
        }
        emit signalViewtransparentFrame(0);
    }

}

void CAllDaySchceduleWeekWidgetItem::mousePressEvent(QMouseEvent *event)
{
    QRect drawrect = m_coorManage->getAllDayDrawRegion(m_ScheduleInfo.beginDateTime.date(), m_ScheduleInfo.endDateTime.date());
    if (drawrect.contains(event->pos())) {

        if (event->button() == Qt::LeftButton) {
            m_selectflag = true;
            update();
            emit signalsPress(this);
        }
    }
}

void CAllDaySchceduleWeekWidgetItem::mouseReleaseEvent(QMouseEvent *event)
{
    QRect drawrect = m_coorManage->getAllDayDrawRegion(m_ScheduleInfo.beginDateTime.date(), m_ScheduleInfo.endDateTime.date());
    if (drawrect.contains(event->pos())) {
        if (event->button() == Qt::LeftButton) {
            m_selectflag = false;
            update();
        }
    }
}

void CAllDaySchceduleWeekWidgetItem::focusOutEvent(QFocusEvent *event)
{
    //m_highflag = false;
    update();
}

void CAllDaySchceduleWeekWidgetItem::enterEvent(QEvent *event)
{
    m_hoverflag = true;
    update();
}

void CAllDaySchceduleWeekWidgetItem::leaveEvent(QEvent *event)
{
    m_hoverflag = false;
    update();
}


void CAllDaySchceduleWeekView::setTheMe(int type)
{
    updateDateShow();
}

void CAllDaySchceduleWeekView::setRange(int w, int h, QDate begindate, QDate enddate, int rightmagin)
{
    m_coorManage->setRange(w, h, begindate, enddate, rightmagin);
    updateDateShow();
}

void CAllDaySchceduleWeekView::setLunarVisible(bool state)
{
    m_LunarVisible = state;
}

void CAllDaySchceduleWeekView::updateHigh()
{
    for (int i = 0; i < m_baseShowItem.count(); i++) {
        m_baseShowItem.at(i)->update();
    }
}

CAllDaySchceduleWeekView::CAllDaySchceduleWeekView(QWidget *parent, int edittype)
    : DListWidget(parent)
{


    //setWindowFlags(Qt::FramelessWindowHint);
    //setAttribute(Qt::WA_TranslucentBackground, true);
    m_editType = edittype;
    m_widgetFlag = false;
    setContentsMargins(0, 0, 0, 0);
    //setSpacing(1);
    //DPalette mainpa = this->palette();
    // this->setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Light, QColor(0, 0, 0, 0));
    pal.setColor(QPalette::Dark, QColor(0, 0, 0, 0));
    pal.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    //pal.setBrush(backgroundRole(), QColor(0, 0, 0, 0));
    setPalette(pal);

    //setWindowFlags(Qt::FramelessWindowHint);
    //setAttribute(Qt::WA_TranslucentBackground, true);
    setFrameShape(QListWidget::NoFrame);
    //setWindowOpacity(0);
    setMouseTracking(true);
    setFocusPolicy(Qt::NoFocus);

    m_coorManage = new CScheduleCoorManage;

    m_createAction = new QAction(tr("New event"), this);
    connect(m_createAction, &QAction::triggered, this, &CAllDaySchceduleWeekView::slotCreate);
    //setStyleSheet("background-color:transparent");
    setSelectionMode(QAbstractItemView::NoSelection);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setSelectionModel(nullptr);
}

CAllDaySchceduleWeekView::~CAllDaySchceduleWeekView()
{

}

void CAllDaySchceduleWeekView::setDayData(const QVector<ScheduleDtailInfo> &vlistData, int type)
{
    m_vlistData = vlistData;
    m_type = type;
    m_widgetFlag = false;
    updateDateShow();
}

void CAllDaySchceduleWeekView::setsolarDayData(QVector<QString> vSolarInfo, QVector<QDate> date)
{
    m_vSolarDayInfo = vSolarInfo;
    m_widgetFlag = false;
    m_vDate = date;
    updateDateShow();
}
void CAllDaySchceduleWeekView::slotCreate()
{
    emit signalViewtransparentFrame(1);
    CSchceduleDlg dlg(1, this);
    QDateTime tDatatime;
    tDatatime.setDate(m_dianjiDay);
    if (m_dianjiDay == QDate::currentDate()) {
        tDatatime.setTime(QTime::currentTime());
    } else {
        tDatatime.setTime(QTime(8, 0));
    }
    dlg.setDate(tDatatime);
    dlg.setAllDay(true);
    if (dlg.exec() == DDialog::Accepted) {
        emit signalsUpdateShcedule(0);
    }
    emit signalViewtransparentFrame(0);
}

void CAllDaySchceduleWeekView::contextMenuEvent(QContextMenuEvent *event)
{
    //if (m_vlistData.isEmpty()) {
    QMenu Context(this);
    Context.addAction(m_createAction);
    m_dianjiDay = m_coorManage->getsDate(mapFrom(this, event->pos()));
    Context.exec(QCursor::pos());
    //}
}

void CAllDaySchceduleWeekView::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_dianjiDay = m_coorManage->getsDate(mapFrom(this, event->pos()));
    emit signalViewtransparentFrame(1);
    CSchceduleDlg dlg(1, this);
    QDateTime tDatatime;
    tDatatime.setDate(m_dianjiDay);
    tDatatime.setTime(QTime::currentTime());
    dlg.setDate(tDatatime);
    dlg.setAllDay(true);
    if (dlg.exec() == DDialog::Accepted) {
        emit signalsUpdateShcedule(0);
    }
    emit signalViewtransparentFrame(0);
}

void CAllDaySchceduleWeekView::updateDateShow()
{
    m_currentitem = NULL;
    //remove
    for (int i = 0; i < this->count(); i++) {
        QListWidgetItem *item11 = this->takeItem(i);
        this->removeItemWidget(item11);
    }
    this->clear();
    for (int i = 0; i < m_baseShowItem.count(); i++) {
        m_baseShowItem[i]->deleteLater();
    }
    m_baseShowItem.clear();
    if (m_vSolarDayInfo.isEmpty() || !m_LunarVisible) {
        for (int i = 0; i < m_vlistData.size(); ++i) {
            CAllDaySchceduleWeekWidgetItem *gwi = createItemWidget(i);
            QListWidgetItem *listItem = new QListWidgetItem;
            listItem->setSizeHint(QSize(gwi->width(), 23)); //每次改变Item的高度
            //listItem->setBackgroundColor(Qt::white);
            listItem->setFlags(Qt::ItemIsTristate);
            addItem(listItem);
            setItemWidget(listItem, gwi);
            gwi->setItem(listItem);
        }

    } else {
        CAllSolarDayWeekWidgetItem *solargwi = createItemWidget(m_vSolarDayInfo, m_vDate);
        QListWidgetItem *solarlistItem = new QListWidgetItem;
        addItem(solarlistItem);
        setItemWidget(solarlistItem, solargwi);
        m_baseShowItem.append(solargwi);
        for (int i = 0; i < m_vlistData.size(); ++i) {
            CAllDaySchceduleWeekWidgetItem *gwi = createItemWidget(i, false);
            QListWidgetItem *listItem = new QListWidgetItem;
            listItem->setSizeHint(QSize(gwi->width(), 23)); //每次改变Item的高度
            //listItem->setBackgroundColor(Qt::white);
            listItem->setFlags(Qt::ItemIsTristate);
            addItem(listItem);
            setItemWidget(listItem, gwi);
            gwi->setItem(listItem);
        }

    }
}

CAllDaySchceduleWeekWidgetItem *CAllDaySchceduleWeekView::createItemWidget(int index, bool average)
{
    const ScheduleDtailInfo &gd = m_vlistData.at(index);
    CSchedulesColor gdcolor = CScheduleDataManage::getScheduleDataManage()->getScheduleColorByType(gd.type.ID);
    CAllDaySchceduleWeekWidgetItem *gwi = new CAllDaySchceduleWeekWidgetItem(this, m_editType);
    gwi->setCoorManage(m_coorManage);
    if (m_type == 0) {
        gwi->setColor(gdcolor.gradientFromC, gdcolor.gradientToC, true);
        QFont font("PingFangSC-Light");
        if (average) {
            font.setPixelSize(6);
        } else {
            font.setPixelSize(12);
        }
        gwi->setData(gd);
        if (average) {
            gwi->setFixedSize(width(), 22);
            gwi->setText(gdcolor.textColor, font, QPoint(23, 2), average);
        } else {
            gwi->setFixedSize(width(), 22);
            gwi->setText(gdcolor.textColor, font, QPoint(23, 2), average);
        }
        QColor TransparentC = "#000000";
        TransparentC.setAlphaF(0.05);
        gwi->setTransparentB(false, TransparentC);
        gwi->setItem(NULL);
    } else {
        gwi->setColor(gdcolor.gradientFromC, gdcolor.gradientToC, true);
        QFont font("PingFangSC-Light");
        if (average) {
            font.setPixelSize(6);
        } else {
            font.setPixelSize(12);
        }
        if (average) {
            gwi->setFixedSize(width(), 22);
            gwi->setText(gdcolor.textColor, font, QPoint(8, 2), average);
        } else {
            gwi->setFixedSize(width(), 22);
            gwi->setText(gdcolor.textColor, font, QPoint(8, 2), average);
        }
        QColor TransparentC = "#000000";
        TransparentC.setAlphaF(0.05);
        gwi->setTransparentB(false, TransparentC);
        gwi->setData(gd);
        gwi->setItem(NULL);
    }
    connect(gwi, &CAllDaySchceduleWeekWidgetItem::signalsDelete, this, &CAllDaySchceduleWeekView::slotdeleteitem);
    connect(gwi, &CAllDaySchceduleWeekWidgetItem::signalsEdit, this, &CAllDaySchceduleWeekView::slotedititem);
    connect(gwi, &CAllDaySchceduleWeekWidgetItem::signalsPress, this, &CAllDaySchceduleWeekView::slotupdateItem);
    connect(gwi, &CAllDaySchceduleWeekWidgetItem::signalViewtransparentFrame, this, &CAllDaySchceduleWeekView::signalViewtransparentFrame);

    return gwi;
}

CAllSolarDayWeekWidgetItem *CAllDaySchceduleWeekView::createItemWidget(QVector<QString> vSolarInfo, QVector<QDate> date, bool average)
{
    CAllSolarDayWeekWidgetItem *gwi = new CAllSolarDayWeekWidgetItem(this, m_editType);
    gwi->setCoorManage(m_coorManage);
    QColor color1 = m_soloColor;
    color1.setAlphaF(0.3);
    CSchedulesColor gdcolor = CScheduleDataManage::getScheduleDataManage()->getScheduleColorByType(1);
    if (m_type == 0) {
        gwi->setColor(color1, color1, true);
        QFont font("PingFangSC-Light");
        if (average) {
            font.setPixelSize(6);
        } else {
            font.setPixelSize(12);
        }
        gwi->setData(vSolarInfo, date);
        int w = width();
        if (average) {
            gwi->setFixedSize(width(), 22);
            gwi->setText(gdcolor.textColor, font, QPoint(13, 2), average);
        } else {
            gwi->setFixedSize(width(), 22);
            gwi->setText(gdcolor.textColor, font, QPoint(13, 2), average);
        }
    } else {
        gwi->setColor(color1, color1, true);
        QFont font("PingFangSC-Light");
        if (average) {
            font.setPixelSize(6);
        } else {
            font.setPixelSize(12);
        }
        gwi->setData(vSolarInfo, date);
        if (average) {
            gwi->setFixedSize(width(), 22);
            gwi->setText(gdcolor.textColor, font, QPoint(13, 2), average);
        } else {
            gwi->setFixedSize(width(), 22);
            gwi->setText(gdcolor.textColor, font, QPoint(13, 2), average);
        }
    }
    return gwi;
}

void CAllDaySchceduleWeekView::slotdeleteitem(CAllDaySchceduleWeekWidgetItem *item)
{
    emit signalsUpdateShcedule(item->getData().id);
    updateDateShow();
    update();
}

void CAllDaySchceduleWeekView::slotedititem(CAllDaySchceduleWeekWidgetItem *item, int type)
{

    emit signalsUpdateShcedule(item->getData().id);
    updateDateShow();
    update();
}

void CAllDaySchceduleWeekView::slotupdateItem(CAllDaySchceduleWeekWidgetItem *item)
{
    m_currentitem = item;
    emit signalsitem(this);
}

void CAllDaySchceduleWeekView::slotDeleteItem()
{
    if (m_currentitem != NULL) {
        m_currentitem->slotDelete();
    }
}


CAllSolarDayWeekWidgetItem::CAllSolarDayWeekWidgetItem(QWidget *parent, int edittype)
{

}

void CAllSolarDayWeekWidgetItem::setColor(QColor color1, QColor color2, bool GradientFlag)
{
    m_color1 = color1;
    m_color2 = color2;
    m_GradientFlag = GradientFlag;
}

void CAllSolarDayWeekWidgetItem::setText(QColor tcolor, QFont font, QPoint pos, bool avgeflag)
{
    m_textcolor = tcolor;
    m_font = font;
    m_pos = pos;
}

void CAllSolarDayWeekWidgetItem::setData(QVector<QString> vSolarInfo, QVector<QDate> date)
{
    m_vhover.clear();
    m_vselectflag.clear();
    m_vSolarDayInfo = vSolarInfo;
    m_vDate = date;
    m_vhover.resize(m_vDate.count());
    m_vselectflag.resize(m_vDate.count());
    m_vhover.fill(false);
    m_vselectflag.fill(false);
}
void CAllSolarDayWeekWidgetItem::paintEvent(QPaintEvent *e)
{
    int labelwidth = width();
    int labelheight = height();
    QPainter painter(this);
    for (int i = 0; i < m_vDate.count(); i++) {
        QRect drawrect = m_coorManage->getAllDayDrawRegion(m_vDate.at(i), m_vDate.at(i));
        if (drawrect.width() < 0) return;

        if (m_GradientFlag) {
            QRect fillRect = QRect(2, 1, labelwidth - 2, labelheight - 1);
            //将直线开始点设为0，终点设为1，然后分段设置颜色
            painter.save();
            painter.setRenderHints(QPainter::HighQualityAntialiasing);
            painter.setBrush(QColor(0, 0, 0, 0));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(fillRect, 8, 8);
            painter.restore();
            painter.save();
            QLinearGradient linearGradient(0, 0, labelwidth, 0);
            QColor color1 = m_color1;
            QColor color2 = m_color2;
            QColor textcolor = m_textcolor;

            if (m_vhover.at(i)) {
                color1.setAlphaF(0.4);
                color2.setAlphaF(0.4);
                //textcolor.setAlphaF(textcolor.alphaF() * 0.94);
            }
            linearGradient.setColorAt(0, color1);
            linearGradient.setColorAt(1, color2);
            QRect drawrect2 = drawrect;
            //将直线开始点设为0，终点设为1，然后分段设置颜色
            painter.setRenderHints(QPainter::HighQualityAntialiasing);
            painter.setBrush(linearGradient);
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(drawrect2, 8, 8);
            painter.restore();
            painter.setFont(m_font);
            QFont solofont = m_font;
            QFontMetrics fm = painter.fontMetrics();
            QString str =  m_vSolarDayInfo.at(i);
            QString tstr;
            for (int i = 0; i < str.count(); i++) {
                tstr.append(str.at(i));
                int widthT = fm.width(tstr);
                if (widthT >= drawrect.width() - 6) {
                    tstr.chop(2);
                    break;
                }
            }
            if (tstr != str) {
                tstr = tstr + "...";
            }
            painter.setPen(textcolor);
            painter.drawText(QRect(drawrect.topLeft().x() + 2, drawrect.topLeft().y() + (drawrect.height() - fm.height()) / 2, drawrect.width(), drawrect.height()), Qt::AlignLeft, tstr);
        }
    }
}
void CAllSolarDayWeekWidgetItem::mousePressEvent(QMouseEvent *event)
{
    m_vselectflag.fill(false);
    /*for (int i = 0; i < m_vDate.count(); i++) {
        QRect drawrect = m_coorManage->getAllDayDrawRegion(m_vDate.at(i), m_vDate.at(i));
        if (drawrect.contains(mapFromGlobal(QCursor::pos()))) {
            m_vhover[i] = true;
        }
    }*/
    update();
}

void CAllSolarDayWeekWidgetItem::focusOutEvent(QFocusEvent *event)
{
    m_vselectflag.fill(false);
    update();
}

void CAllSolarDayWeekWidgetItem::enterEvent(QEvent *event)
{
    m_vhover.fill(false);
    /*for (int i = 0; i < m_vDate.count(); i++) {
        QRect drawrect = m_coorManage->getAllDayDrawRegion(m_vDate.at(i), m_vDate.at(i));
        if (drawrect.contains(mapFromGlobal(QCursor::pos()))) {
            m_vhover[i] = true;
        }
    }*/
    update();
}

void CAllSolarDayWeekWidgetItem::leaveEvent(QEvent *event)
{
    m_vhover.fill(false);
    update();
}
