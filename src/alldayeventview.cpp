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
#include "alldayeventview.h"
#include <QAction>
#include <QMenu>
#include <QListWidget>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>
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
CAllDayEventWidgetItem::CAllDayEventWidgetItem(QRect rect, QGraphicsItem *parent /*= nullptr*/, int edittype)
    : QGraphicsRectItem(parent),
      m_rect(rect)
{
    setZValue(0);
    setAcceptHoverEvents(true);
    setRect(rect);
    m_editType = edittype;
    m_font.setFamily("PingFangSC-Light");
    const int duration = 200;
    m_properAnimationFirst = new  QPropertyAnimation(this, "offset", this);
    m_properANimationSecond  = new QPropertyAnimation(this, "offset", this);
    m_properAnimationFirst->setDuration(duration);
    m_properANimationSecond->setDuration(duration);
    m_properAnimationFirst->setEasingCurve(QEasingCurve::InOutQuad);
    m_properANimationSecond->setEasingCurve(QEasingCurve::InOutQuad);
    m_Group = new QSequentialAnimationGroup(this);
    m_Group->addAnimation(m_properAnimationFirst);
    m_Group->addAnimation(m_properANimationSecond);
}

void CAllDayEventWidgetItem::setData(const ScheduleDtailInfo &vScheduleInfo)
{
    m_vScheduleInfo = vScheduleInfo;
    m_vSelectflag = false;
    m_vHoverflag = false;
    m_vHighflag = false;
    update();
}

ScheduleDtailInfo CAllDayEventWidgetItem::getData() const
{
    return m_vScheduleInfo;
}

int CAllDayEventWidgetItem::getEventByPos(QPoint pos)
{
    int tindex = -1;
//    for (int i = 0; i  < m_vScheduleInfo.count(); i++) {
//        QRect drawrect = m_coorManage->getAllDayDrawRegion(m_vScheduleInfo.at(i).beginDateTime.date(), m_vScheduleInfo.at(i).endDateTime.date());
//        if (drawrect.contains(pos)) {
//            tindex = i;
//            break;
//        }
//    }
    return tindex;
}

void CAllDayEventWidgetItem::updateitem()
{

}

void CAllDayEventWidgetItem::setPressFlag(const bool flag)
{
    m_press = flag;
    update();
}

void CAllDayEventWidgetItem::setFont(DFontSizeManager::SizeType type)
{
    m_sizeType = type;
}

void CAllDayEventWidgetItem::setOffset(const int &offset)
{
    setRect(QRect(m_rect.x() - offset,
                  m_rect.y() - offset / 2,
                  m_rect.width() + offset * 2,
                  m_rect.height() + offset));
    setZValue(offset);
    update();
}

bool CAllDayEventWidgetItem::hasSelectSchedule(const ScheduleDtailInfo &info)
{
    return info == m_vScheduleInfo;
}

void CAllDayEventWidgetItem::setStartValue(const int value)
{
    m_properAnimationFirst->setStartValue(value);
    m_properANimationSecond->setEndValue(value);
}

void CAllDayEventWidgetItem::setEndValue(const int value)
{
    m_properAnimationFirst->setEndValue(value);
    m_properANimationSecond->setStartValue(value);
}

void CAllDayEventWidgetItem::startAnimation()
{
    if (m_Group->state() != QAnimationGroup::Running) {
        m_Group->start();
    }
}

void CAllDayEventWidgetItem::slotCreate()
{
    emit signalViewtransparentFrame(1);
    CSchceduleDlg dlg(1, nullptr);
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

void CAllDayEventWidgetItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_vHoverflag = true;
    update();
}

void CAllDayEventWidgetItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_vHoverflag = false;
//    m_hoverPressMove = false;
    m_press = false;
    update();
}

void CAllDayEventWidgetItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    m_font = DFontSizeManager::instance()->get(m_sizeType, m_font);
    painter->setRenderHints(QPainter::Antialiasing);
    CSchedulesColor gdcolor = CScheduleDataManage::getScheduleDataManage()->getScheduleColorByType(m_vScheduleInfo.type.ID);
    m_vHighflag = CScheduleDataManage::getScheduleDataManage()->getSearchResult(m_vScheduleInfo);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    QRectF drawrect = this->rect();


    QLinearGradient linearGradient(drawrect.topLeft().x(), 0, drawrect.topRight().x(), 0);

    QColor color1 = gdcolor.gradientFromC;
    QColor color2 = gdcolor.gradientToC;
    QColor textcolor = gdcolor.textColor;
    if (m_vHoverflag) {
        color1 = gdcolor.hovergradientFromC;
        color2 = gdcolor.hovergradientToC;
    } else if (m_vHighflag) {
        color1 = gdcolor.hightlightgradientFromC;
        color2 = gdcolor.hightlightgradientToC;
    }
    if (m_vSelectflag || m_press) {
        color1 = gdcolor.pressgradientFromC;
        color2 = gdcolor.pressgradientToC;
        textcolor.setAlphaF(0.6);
    }
    linearGradient.setColorAt(0, color1);
    linearGradient.setColorAt(1, color2);
    QRectF fillRect = drawrect;
    //将直线开始点设为0，终点设为1，然后分段设置颜色
    painter->setBrush(linearGradient);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(fillRect, 8, 8);

    painter->setFont(m_font);
    painter->setPen(textcolor);
    QFontMetrics fm = painter->fontMetrics();
    QString tStitlename = m_vScheduleInfo.titleName;
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

    painter->drawText(QRect(fillRect.topLeft().x() + 13, fillRect.y(), fillRect.width(), fillRect.height()),
                      Qt::AlignLeft | Qt::AlignVCenter, tstr);
    if (m_vHoverflag && !m_vSelectflag) {
        QRectF trect = QRectF(fillRect.x() + 0.5, fillRect.y() + 0.5, fillRect.width() - 1, fillRect.height() - 1);
        painter->save();

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
        painter->setBrush(Qt::NoBrush);
        painter->setPen(pen);
        painter->drawRoundedRect(trect, 8, 8);
        painter->restore();
    }
    if (m_vSelectflag) {
        QColor selcolor = "#000000";
        selcolor.setAlphaF(0.05);
        painter->setBrush(selcolor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(fillRect, 8, 8);
    }
}

void CAllDayEventWidgetItem::slotEdit()
{
    emit signalViewtransparentFrame(1);
    CSchceduleDlg dlg(0, nullptr);
    dlg.setData(m_vScheduleInfo);
    if (dlg.exec() == DDialog::Accepted) {

        emit signalsEdit(this, 1);
    }
    emit signalViewtransparentFrame(0);
}

void CAllDayEventWidgetItem::slotDelete()
{
    emit signalViewtransparentFrame(1);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();
    ScheduleDtailInfo tinfo = m_vScheduleInfo;
    if (tinfo.rpeat == 0) {
        CSchceduleCtrlDlg msgBox(nullptr);
        msgBox.setText(tr("You are deleting an event."));
        msgBox.setInformativeText(tr("Are you sure you want to delete this event?"));
        //设置按钮文字
        msgBox.addPushButton(tr("Cancel"), true);
        msgBox.addWaringButton(tr("Delete"), true);
        msgBox.exec();
        //是否删除
        if (msgBox.clickButton() == 0) {
            emit signalViewtransparentFrame(0);
            return;
        } else if (msgBox.clickButton() == 1) {
            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(tinfo.id);
        }
    } else {
        if (tinfo.RecurID == 0) {
            CSchceduleCtrlDlg msgBox(nullptr);
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
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(tinfo.id);
            } else if (msgBox.clickButton() == 2) {
                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(tinfo.id, newschedule);
                newschedule.ignore.append(tinfo.beginDateTime);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            }
        } else {
            CSchceduleCtrlDlg msgBox(nullptr);
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
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(tinfo.id, newschedule);
                newschedule.enddata.type = 2;
                newschedule.enddata.date = tinfo.beginDateTime.addDays(-1);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            } else if (msgBox.clickButton() == 2) {
                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(tinfo.id, newschedule);
                newschedule.ignore.append(tinfo.beginDateTime);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            }
        }
    }
    emit signalViewtransparentFrame(0);
    emit signalsDelete(this);
}
//有问题
void CAllDayEventWidgetItem::slotDoubleEvent(int type)
{
    emit signalsEdit(this, 1);
}




void CAllDayEventWeekView::setTheMe(int type)
{
    updateDateShow();
}

void CAllDayEventWeekView::setRange(int w, int h, QDate begindate, QDate enddate, int rightmagin)
{
    m_coorManage->setRange(w, h, begindate, enddate, rightmagin);
    m_Scene->setSceneRect(0, 0, w, h);
    m_rightmagin = rightmagin;
    updateDateShow();
}

void CAllDayEventWeekView::setLunarVisible(bool state)
{
    m_LunarVisible = state;
}

void CAllDayEventWeekView::updateHigh()
{
    for (int i = 0; i < m_baseShowItem.count(); i++) {
        m_baseShowItem.at(i)->update();
    }
}

void CAllDayEventWeekView::setSelectSchedule(const ScheduleDtailInfo &info)
{
    for (int i = 0; i < m_baseShowItem.size(); ++i) {
        CAllDayEventWidgetItem *item = m_baseShowItem.at(i);
        if (item->hasSelectSchedule(info)) {
            QRectF rect = item->rect();
            centerOn(0, rect.y());
            setTransformationAnchor(QGraphicsView::AnchorViewCenter);
            item->setStartValue(0);
            item->setEndValue(4);
            item->startAnimation();
        }
    }
}

void CAllDayEventWeekView::setMargins(int left, int top, int right, int bottom)
{
    setViewportMargins(QMargins(left, top, right, bottom));
}

CAllDayEventWeekView::CAllDayEventWeekView(QWidget *parent, int edittype)
    : DGraphicsView(parent)
    , m_Scene(new QGraphicsScene(this))
{
    setScene(m_Scene);
    m_editType = edittype;
    m_widgetFlag = false;
    setContentsMargins(0, 0, 0, 0);
    QPalette pal = palette();
    pal.setColor(QPalette::Light, QColor(0, 0, 0, 0));
    pal.setColor(QPalette::Dark, QColor(0, 0, 0, 0));
    pal.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    setPalette(pal);

    m_coorManage = new CScheduleCoorManage;

    m_editAction = new QAction(tr("Edit"), this);
    m_deleteAction = new QAction(tr("Delete"), this);
    m_createAction = new QAction(tr("New event"), this);
    connect(m_createAction, &QAction::triggered, this, &CAllDayEventWeekView::slotCreate);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setViewportMargins(0, 0, 0, 0);
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

CAllDayEventWeekView::~CAllDayEventWeekView()
{

}

void CAllDayEventWeekView::setDayData(const QVector<QVector<ScheduleDtailInfo> > &vlistData, int type)
{
    m_vlistData = vlistData;
    m_type = type;
    m_widgetFlag = false;
    updateDateShow();
}

void CAllDayEventWeekView::slotCreate()
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

void CAllDayEventWeekView::slotDoubleEvent()
{
    m_updateDflag  = true;
    emit signalsUpdateShcedule(0);
}


void CAllDayEventWeekView::mousePressEvent(QMouseEvent *event)
{
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    if (event->button() == Qt::RightButton) {
        emit signalScheduleShow(false);
        CAllDayEventWidgetItem *item = dynamic_cast<CAllDayEventWidgetItem *>(itemAt(event->pos()));
        if (item == nullptr) {
            DMenu Context(this);
            Context.addAction(m_createAction);
            m_dianjiDay = m_coorManage->getsDate(mapFrom(this, event->pos()));
            Context.exec(QCursor::pos());

        } else {
            if (item->getData().type.ID == 4)
                return;
            DMenu menu(this);
            menu.addAction(m_editAction);
            menu.addAction(m_deleteAction);
            QAction *action_t = menu.exec(QCursor::pos());
            if (action_t == m_editAction) {
                emit signalViewtransparentFrame(1);
                CSchceduleDlg dlg(0, this);
                dlg.setData(item->getData());
                if (dlg.exec() == DDialog::Accepted) {
                    emit signalsUpdateShcedule(0);
                }
                emit signalViewtransparentFrame(0);
            } else if (action_t == m_deleteAction) {
                emit signalViewtransparentFrame(1);
                ScheduleDtailInfo info = item->getData();
                if (info.rpeat == 0) {
                    CSchceduleCtrlDlg msgBox(this);
                    msgBox.setText(tr("You are deleting an event."));
                    msgBox.setInformativeText(tr("Are you sure you want to delete this event?"));
                    //设置按钮文字
                    msgBox.addPushButton(tr("Cancel"), true);
                    msgBox.addWaringButton(tr("Delete"), true);
                    msgBox.exec();
                    //是否删除
                    if (msgBox.clickButton() == 0) {
                        emit signalViewtransparentFrame(0);
                        return;
                    } else if (msgBox.clickButton() == 1) {
                        CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(info.id);
                    }
                } else {
                    if (info.RecurID == 0) {
                        CSchceduleCtrlDlg msgBox(this);
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
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(info.id);
                        } else if (msgBox.clickButton() == 2) {
                            ScheduleDtailInfo newschedule;
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(info.id, newschedule);
                            newschedule.ignore.append(info.beginDateTime);
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
                        }
                    } else {
                        CSchceduleCtrlDlg msgBox(this);
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
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(info.id, newschedule);
                            newschedule.enddata.type = 2;
                            newschedule.enddata.date = info.beginDateTime.addDays(-1);
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);

                        } else if (msgBox.clickButton() == 2) {
                            ScheduleDtailInfo newschedule;
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(info.id, newschedule);
                            newschedule.ignore.append(info.beginDateTime);
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
                        }
                    }
                }
                emit signalsUpdateShcedule(item->getData().id);
                emit signalViewtransparentFrame(0);
            }
        }
    } else if (event->button() == Qt::LeftButton) {
        CAllDayEventWidgetItem *item = dynamic_cast<CAllDayEventWidgetItem *>(itemAt(event->pos()));
        if (item != nullptr) {
            m_currentitem = item;
            m_press = true;
            item->setPressFlag(true);
            emit signalScheduleShow(true, item->getData());
            emit signalsitem(this);
        } else {
            emit signalScheduleShow(false);
        }
    }
}

void CAllDayEventWeekView::mouseReleaseEvent(QMouseEvent *event)
{
    m_press = false;
    CAllDayEventWidgetItem *item = dynamic_cast<CAllDayEventWidgetItem *>(itemAt(event->pos()));
    if (item != nullptr) {
        item->setPressFlag(false);
    }
}

void CAllDayEventWeekView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        return;
    }
    emit signalScheduleShow(false);
    DGraphicsView::mouseDoubleClickEvent(event);
    CAllDayEventWidgetItem *item = dynamic_cast<CAllDayEventWidgetItem *>(itemAt(event->pos()));
    if (item == nullptr) {
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
    } else {
        emit signalViewtransparentFrame(1);
        m_updateDflag  = false;
        CMySchceduleView dlg(item->getData(), this);
//        dlg.setSchedules(item->getData());
        connect(&dlg, &CMySchceduleView::signalsEditorDelete, this, &CAllDayEventWeekView::slotDoubleEvent);
        dlg.exec();
        emit signalViewtransparentFrame(0);
        disconnect(&dlg, &CMySchceduleView::signalsEditorDelete, this, &CAllDayEventWeekView::slotDoubleEvent);
        if (item == nullptr) {
            return;
        }
        if (!m_updateDflag)
            item->updateitem();
    }
}

void CAllDayEventWeekView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_press) {
        emit signalScheduleShow(false);
        m_press = false;
    }
    DGraphicsView::mouseMoveEvent(event);
}

void CAllDayEventWeekView::wheelEvent(QWheelEvent *event)
{
    emit signalScheduleShow(false);
    DGraphicsView::wheelEvent(event);
}

void CAllDayEventWeekView::paintEvent(QPaintEvent *event)
{
    DGraphicsView::paintEvent(event);
}

void CAllDayEventWeekView::updateDateShow()
{
    m_currentitem = nullptr;
    m_Scene->setSceneRect(0,
                          0,
                          m_Scene->width(),
                          (itemHeight + 1)*m_vlistData.size());

    for (int i = 0; i < m_baseShowItem.count(); i++) {
        delete  m_baseShowItem[i];
    }
    m_baseShowItem.clear();
    for (int i = 0; i < m_vlistData.size(); ++i) {
        createItemWidget(i);
    }
//    if (m_vSolarDayInfo.isEmpty() || !m_LunarVisible) {
//        for (int i = 0; i < m_vlistData.size(); ++i) {
//            createItemWidget(i);
//        }

//    } else {
//        for (int i = 0; i < m_vlistData.size(); ++i) {
//            createItemWidget(i, false);
//        }

//    }
}

void CAllDayEventWeekView::createItemWidget(int index, bool average)
{
    Q_UNUSED(average)
    for (int i = 0; i < m_vlistData[index].size(); ++i) {
        const ScheduleDtailInfo &info = m_vlistData[index].at(i);

        QRect drawrect = m_coorManage->getAllDayDrawRegion(info.beginDateTime.date(), info.endDateTime.date());
        drawrect.setY(3 + (itemHeight + 1)*index);
        drawrect.setHeight(itemHeight);

        CAllDayEventWidgetItem *gwi = new CAllDayEventWidgetItem(drawrect, nullptr, m_editType);
        gwi->setCoorManage(m_coorManage);
        gwi->setData(m_vlistData[index].at(i));
        m_Scene->addItem(gwi);
        m_baseShowItem.append(gwi);
        connect(gwi, &CAllDayEventWidgetItem::signalsDelete, this, &CAllDayEventWeekView::slotdeleteitem);
        connect(gwi, &CAllDayEventWidgetItem::signalsEdit, this, &CAllDayEventWeekView::slotedititem);
        connect(gwi, &CAllDayEventWidgetItem::signalsPress, this, &CAllDayEventWeekView::slotupdateItem);
        connect(gwi, &CAllDayEventWidgetItem::signalViewtransparentFrame, this, &CAllDayEventWeekView::signalViewtransparentFrame);
    }
}

void CAllDayEventWeekView::slotdeleteitem(CAllDayEventWidgetItem *item)
{
    emit signalsUpdateShcedule(0);
    updateDateShow();
    update();
}

void CAllDayEventWeekView::slotedititem(CAllDayEventWidgetItem *item, int type)
{
    emit signalsUpdateShcedule(0);
    updateDateShow();
    update();
}

void CAllDayEventWeekView::slotupdateItem(CAllDayEventWidgetItem *item)
{
    m_currentitem = item;
    emit signalsitem(this);
}

void CAllDayEventWeekView::slotDeleteItem()
{
    if (m_currentitem != nullptr) {
        if (m_currentitem->getData().type.ID == 4)
            return;
        m_currentitem->slotDelete();
    }
}
