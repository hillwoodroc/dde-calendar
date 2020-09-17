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
#include "graphicsview.h"
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>

#include "scheduleitem.h"
#include "schedulecoormanage.h"
#include "schceduledlg.h"
#include <QMenu>
#include "scheduledatamanage.h"
#include <DMessageBox>
#include <DPushButton>
#include <DHiDPIHelper>
#include <QPainterPath>
#include <DPalette>
#include "schcedulectrldlg.h"
#include "myschceduleview.h"
#include <QShortcut>
#include <QTimer>
#include <QWheelEvent>
DGUI_USE_NAMESPACE
CGraphicsView::CGraphicsView(QWidget *parent, int viewType)
    : DGraphicsView(parent), m_viewType(viewType)
{
    setContentsMargins(0, 0, 0, 0);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_coorManage = new CScheduleCoorManage;

    m_LRPen.setColor(QColor(255, 255, 255));
    m_LRPen.setStyle(Qt::SolidLine);
    m_TBPen.setColor(QColor(255, 255, 255));
    m_TBPen.setStyle(Qt::SolidLine);
    m_LRFlag = true;
    m_TBFlag = true;
    m_margins = QMargins(0, 0, 0, 0);

//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_graphicsScene = new QGraphicsScene(this);
    m_graphicsScene->setSceneRect(0, 0, width(), height());
    setScene(m_graphicsScene);

    m_dayInterval = width();
    m_timeInterval = height() / 24.0;

    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(viewWidth / 2,  viewHeight / 2 - 1000);
    centerOn(mapToScene(newCenter));

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    scrollBarValueChangedSlot();

    m_editAction = new QAction(tr("Edit"), this);
    m_deleteAction = new QAction(tr("Delete"), this);
    m_createAction = new QAction(tr("New event"), this);
    setMouseTracking(true);
    viewport()->setMouseTracking(true);

    setLineWidth(0);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(scrollBarValueChangedSlot()));
    m_timer->start(60000);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChangedSlot()));
    connect(this->verticalScrollBar(),
            &QScrollBar::sliderPressed,
            this,
            &CGraphicsView::slotScrollBar);
}

CGraphicsView::~CGraphicsView()
{
    m_timer->stop();
    m_timer->deleteLater();
    clearSchdule();
}

void CGraphicsView::setMargins(int left, int top, int right, int bottom)
{
    m_margins = QMargins(left, 0, right, bottom);
    setViewportMargins(m_margins);
}

void CGraphicsView::setTheMe(int type)
{
    if (type == 0 || type == 1) {
        m_weekcolor = "#00429A";
        m_weekcolor.setAlphaF(0.05);
        QColor linecolor = "#000000";
        linecolor.setAlphaF(0.1);
        m_LRPen.setColor(linecolor);
        m_TBPen.setColor(linecolor);
        m_LRPen.setStyle(Qt::SolidLine);
        m_TBPen.setStyle(Qt::SolidLine);

    } else if (type == 2) {
        m_weekcolor = "#4F9BFF";
        m_weekcolor.setAlphaF(0.1);
        QColor linecolor = "#000000";
        linecolor.setAlphaF(0.1);
        m_LRPen.setColor(linecolor);
        m_TBPen.setColor(linecolor);
    }
    scene()->update();
    update();
}

void CGraphicsView::updateHigh()
{
    scene()->update();
    update();
}

void CGraphicsView::setRange( int w, int h, QDate begindate, QDate enddate, int rightmagin)
{
    w = w - rightmagin;
    m_graphicsScene->setSceneRect(0, 0, w, h);
    m_coorManage->setRange(w, h, begindate, enddate, rightmagin);
    m_rightmagin = rightmagin;
    int totalDay = begindate.daysTo(enddate) + 1;
    m_dayInterval = w * 1.0 / totalDay;
    m_timeInterval = h / 24.0;
    m_totalDay = totalDay;
    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(viewWidth / 2,  viewHeight / 2 - 2000);
    centerOn(mapToScene(newCenter));

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    scrollBarValueChangedSlot();
}

void CGraphicsView::addSchduleItem( const ScheduleDtailInfo &info, QDate date, int index, int totalNum, int type, int viewtype, int maxnum)
{
    m_currentItem = nullptr;
    CScheduleItem *item = new CScheduleItem(m_coorManage, nullptr, m_graphicsScene, type);
    item->setData(info, date, index, totalNum, viewtype, maxnum);
    //用于修改联动bug
    connect(item, &CScheduleItem::signalsHoverUpdateState, this, &CGraphicsView::slotHoverUpdateState);
    connect(item, &CScheduleItem::signalsSelectUpdateState, this, &CGraphicsView::slotSelectUpdateState);
    m_vScheduleItem.append(item);

}

void CGraphicsView::deleteSchduleItem( CScheduleItem *item )
{
    int id = item->getData().id;
    for (int i = 0; i < m_vScheduleItem.size(); i++) {
        if (m_vScheduleItem[i]->getData().id == id) {
            disconnect(item, &CScheduleItem::signalsHoverUpdateState, this, &CGraphicsView::slotHoverUpdateState);
            disconnect(item, &CScheduleItem::signalsSelectUpdateState, this, &CGraphicsView::slotSelectUpdateState);

            m_vScheduleItem.remove(i);
            m_graphicsScene->removeItem(item);
            delete m_vScheduleItem[i];
            m_vScheduleItem[i] = nullptr;
            i--;
        }
    }
}

void CGraphicsView::setSelectSchedule(const ScheduleDtailInfo &info)
{
    setTime(info.beginDateTime.time());
    for (int i = 0 ; i < m_vScheduleItem.size(); ++i) {
        if (m_vScheduleItem.at(i)->getType() == 1)
            continue;
        if (m_vScheduleItem.at(i)->hasSelectSchedule(info)) {

            m_vScheduleItem.at(i)->setStartValue(0);
            m_vScheduleItem.at(i)->setEndValue(10);
            m_vScheduleItem.at(i)->startAnimation();
        }
    }
}

void CGraphicsView::clearSchdule()
{
    for (int i = 0; i < m_vScheduleItem.size(); i++) {
        disconnect(m_vScheduleItem.at(i), &CScheduleItem::signalsHoverUpdateState, this, &CGraphicsView::slotHoverUpdateState);
        disconnect(m_vScheduleItem.at(i), &CScheduleItem::signalsSelectUpdateState, this, &CGraphicsView::slotSelectUpdateState);
        m_graphicsScene->removeItem(m_vScheduleItem.at(i));
        delete m_vScheduleItem[i];
        m_vScheduleItem[i] = nullptr;
    }
    m_vScheduleItem.clear();
    m_updateDflag = true;
}

///************************************************************************
//Function:       onViewEvent()
//Description:    执行父类事件
//Input:          event 事件
//Output:         无
//Return:         无
//Others:         无
//************************************************************************/
//void CGraphicsView::onViewEvent( QEvent *event )
//{
//    //防止为空
//    if (nullptr == event)
//        return;
//    switch (event->type()) {
//    case QEvent::MouseButtonPress: {
//        QMouseEvent *t_event = dynamic_cast<QMouseEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::mousePressEvent(t_event);
//    }
//    break;
//    case QEvent::MouseMove: {
//        QMouseEvent *t_event = dynamic_cast<QMouseEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::mouseMoveEvent(t_event);
//    }
//    break;
//    case QEvent::MouseButtonRelease: {
//        QMouseEvent *t_event = dynamic_cast<QMouseEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::mouseReleaseEvent(t_event);
//    }
//    break;
//    case QEvent::MouseButtonDblClick: {
//        QMouseEvent *t_event = dynamic_cast<QMouseEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::mouseDoubleClickEvent(t_event);
//    }
//    break;
//    case QEvent::Wheel: {
//        QWheelEvent *t_event = dynamic_cast<QWheelEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::wheelEvent(t_event);
//    }
//    break;
//    case QEvent::KeyPress: {
//        QKeyEvent *t_event = dynamic_cast<QKeyEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::keyPressEvent(t_event);
//    }
//    break;
//    case QEvent::KeyRelease: {
//        QKeyEvent *t_event = dynamic_cast<QKeyEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::keyReleaseEvent(t_event);
//    }
//    break;
//    case QEvent::FocusIn: {
//        QFocusEvent *t_event = dynamic_cast<QFocusEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::focusInEvent(t_event);
//    }
//    break;
//    case QEvent::FocusOut: {
//        QFocusEvent *t_event = dynamic_cast<QFocusEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::focusOutEvent(t_event);
//    }
//    break;
//    case QEvent::Enter: {
//        QGraphicsView::enterEvent(event);
//    }
//    break;
//    case QEvent::Leave: {
//        QGraphicsView::leaveEvent(event);
//    }
//    break;
//    case QEvent::ContextMenu: {
//        QContextMenuEvent *t_event = dynamic_cast<QContextMenuEvent *>(event);
//        if (nullptr != t_event)
//            QGraphicsView::contextMenuEvent(t_event);
//    }
//    break;
//    default:
//        break;
//    }
//}

/************************************************************************
Function:       mousePressEvent()
Description:    鼠标按下事件
Input:          event 鼠标事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::mousePressEvent( QMouseEvent *event )
{
    emit signalScheduleShow(false);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    if (event->button() == Qt::RightButton) {
        CScheduleItem *item = dynamic_cast<CScheduleItem *>(itemAt(event->pos()));
        if (item == nullptr) {
            DMenu menu(this);
            menu.addAction(m_createAction);
            QAction *action_t = menu.exec(QCursor::pos());
            if (action_t == m_createAction) {
                QPointF senceposs = mapToScene(event->pos());
                emit signalViewtransparentFrame(1);
                CSchceduleDlg dlg(1, this);
                QDateTime tDatatime = m_coorManage->getDate(senceposs);
                dlg.setDate(tDatatime);
                if (dlg.exec() == DDialog::Accepted) {
                    emit signalsUpdateShcedule(0);
                }
                emit signalViewtransparentFrame(0);
            }
        } else {
            if (item->getType() == 1) return;
            DMenu menu(this);
            menu.addAction(m_editAction);
            menu.addAction(m_deleteAction);
            QAction *action_t = menu.exec(QCursor::pos());
            if (action_t == m_editAction) {
                emit signalViewtransparentFrame(1);
                CSchceduleDlg dlg(0, this, false);
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
                    DPushButton *noButton = msgBox.addPushButton(tr("Cancel"));
                    DPushButton *yesButton = msgBox.addPushButton(tr("Delete"), 1);
                    msgBox.updatesize();
                    DPalette pa = yesButton->palette();
                    if (themetype == 0 || themetype == 1) {
                        pa.setColor(DPalette::ButtonText, Qt::red);

                    } else {
                        pa.setColor(DPalette::ButtonText, "#FF5736");

                    }
                    yesButton->setPalette(pa);
                    msgBox.exec();

                    if (msgBox.clickButton() == noButton) {
                        emit signalViewtransparentFrame(0);
                        return;
                    } else if (msgBox.clickButton() == yesButton) {
                        CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(info.id);
                    }
                } else {
                    if (info.RecurID == 0) {
                        CSchceduleCtrlDlg msgBox(this);
                        msgBox.setText(tr("You are deleting an event."));
                        msgBox.setInformativeText(tr("Do you want to delete all occurrences of this event, or only the selected occurrence?"));
                        DPushButton *noButton = msgBox.addPushButton(tr("Cancel"));
                        DPushButton *yesallbutton = msgBox.addPushButton(tr("Delete All"));
                        DPushButton *yesButton = msgBox.addPushButton(tr("Delete Only This Event"));
                        msgBox.updatesize();
                        DPalette pa = yesButton->palette();
                        if (themetype == 0 || themetype == 1) {
                            pa.setColor(DPalette::ButtonText, Qt::white);
                            pa.setColor(DPalette::Dark, QColor("#25B7FF"));
                            pa.setColor(DPalette::Light, QColor("#0098FF"));
                        } else {
                            pa.setColor(DPalette::ButtonText, "#B8D3FF");
                            pa.setColor(DPalette::Dark, QColor("#0056C1"));
                            pa.setColor(DPalette::Light, QColor("#004C9C"));
                        }
                        yesButton->setPalette(pa);
                        msgBox.exec();

                        if (msgBox.clickButton() == noButton) {
                            emit signalViewtransparentFrame(0);
                            return;
                        } else if (msgBox.clickButton() == yesallbutton) {
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(info.id);
                        } else if (msgBox.clickButton() == yesButton) {

                            ScheduleDtailInfo newschedule;
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(info.id, newschedule);
                            newschedule.ignore.append(info.beginDateTime);
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
                        }
                    } else {
                        CSchceduleCtrlDlg msgBox(this);
                        msgBox.setText(tr("You are deleting an event."));
                        msgBox.setInformativeText(tr("Do you want to delete this and all future occurrences of this event, or only the selected occurrence?"));
                        DPushButton *noButton = msgBox.addPushButton(tr("Cancel"));
                        DPushButton *yesallbutton = msgBox.addPushButton(tr("Delete All Future Events"));
                        DPushButton *yesButton = msgBox.addPushButton(tr("Delete Only This Event"));
                        msgBox.updatesize();
                        DPalette pa = yesButton->palette();
                        if (themetype == 0 || themetype == 1) {
                            pa.setColor(DPalette::ButtonText, Qt::white);
                            pa.setColor(DPalette::Dark, QColor("#25B7FF"));
                            pa.setColor(DPalette::Light, QColor("#0098FF"));
                        } else {
                            pa.setColor(DPalette::ButtonText, "#B8D3FF");
                            pa.setColor(DPalette::Dark, QColor("#0056C1"));
                            pa.setColor(DPalette::Light, QColor("#004C9C"));
                        }
                        yesButton->setPalette(pa);
                        msgBox.exec();

                        if (msgBox.clickButton() == noButton) {
                            emit signalViewtransparentFrame(0);
                            return;
                        } else if (msgBox.clickButton() == yesallbutton) {
                            ScheduleDtailInfo newschedule;
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(info.id, newschedule);
                            newschedule.enddata.type = 2;
                            newschedule.enddata.date = info.beginDateTime.addDays(-1);
                            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);

                        } else if (msgBox.clickButton() == yesButton) {

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
        CScheduleItem *item = dynamic_cast<CScheduleItem *>(itemAt(event->pos()));
        if (item != nullptr) {
            if (item->getType() == 1)
                return;
            m_currentItem = item;
            m_press = true;

            emit signalScheduleShow(true, item->getData());

            emit signalsitem(this);
        }
    }
    DGraphicsView::mousePressEvent(event);
}

void CGraphicsView::mouseReleaseEvent( QMouseEvent *event )
{
    DGraphicsView::mouseReleaseEvent(event);
    m_press = false;
}


void CGraphicsView::mouseDoubleClickEvent( QMouseEvent *event )
{
    emit signalScheduleShow(false);
    DGraphicsView::mouseDoubleClickEvent(event);
    CScheduleItem *item = dynamic_cast<CScheduleItem *>(itemAt(event->pos()));
    if (item == nullptr) {
        QPointF senceposs = mapToScene(event->pos());
        emit signalViewtransparentFrame(1);
        CSchceduleDlg dlg(1, this);
        QDateTime tDatatime = m_coorManage->getDate(senceposs);
        dlg.setDate(tDatatime);
        if (dlg.exec() == DDialog::Accepted) {
            emit signalsUpdateShcedule(0);
        }
        emit signalViewtransparentFrame(0);
        return;
    }
    if (item->getType() == 1) {
        emit signalsCurrentScheduleDate(item->getdate());
        return;
    }
    emit signalViewtransparentFrame(1);
    m_updateDflag  = false;
    CMySchceduleView dlg(item->getData(), this);
//    dlg.setSchedules(item->getData());
    connect(&dlg, &CMySchceduleView::signalsEditorDelete, this, &CGraphicsView::slotDoubleEvent);
    dlg.exec();
    emit signalViewtransparentFrame(0);
    disconnect(&dlg, &CMySchceduleView::signalsEditorDelete, this, &CGraphicsView::slotDoubleEvent);
    if (item == nullptr) {
        return;
    }
    if (!m_updateDflag)
        item->updateitem();
}
void CGraphicsView::slotDoubleEvent(int type)
{
    m_updateDflag  = true;
    emit signalsUpdateShcedule(0);
}

void CGraphicsView::slotDeleteItem()
{
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();
    if (m_currentItem == nullptr) return;

    ScheduleDtailInfo info = m_currentItem->getData();
    if (info.rpeat == 0) {
        CSchceduleCtrlDlg msgBox(this);
        msgBox.setText(tr("You are deleting an event."));
        msgBox.setInformativeText(tr("Are you sure you want to delete this event?"));
        DPushButton *noButton = msgBox.addPushButton(tr("Cancel"));
        DPushButton *yesButton = msgBox.addPushButton(tr("Delete"), 1);
        msgBox.updatesize();
        DPalette pa = yesButton->palette();
        if (themetype == 0 || themetype == 1) {
            pa.setColor(DPalette::ButtonText, Qt::red);

        } else {
            pa.setColor(DPalette::ButtonText, "#FF5736");

        }
        yesButton->setPalette(pa);
        msgBox.exec();

        if (msgBox.clickButton() == noButton) {
            emit signalViewtransparentFrame(0);
            return;
        } else if (msgBox.clickButton() == yesButton) {
            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(info.id);
        }
    } else {
        if (info.RecurID == 0) {
            CSchceduleCtrlDlg msgBox(this);
            msgBox.setText(tr("You are deleting an event."));
            msgBox.setInformativeText(tr("Do you want to delete all occurrences of this event, or only the selected occurrence?"));
            DPushButton *noButton = msgBox.addPushButton(tr("Cancel"));
            DPushButton *yesallbutton = msgBox.addPushButton(tr("Delete All"));
            DPushButton *yesButton = msgBox.addPushButton(tr("Delete Only This Event"));
            msgBox.updatesize();
            DPalette pa = yesButton->palette();
            if (themetype == 0 || themetype == 1) {
                pa.setColor(DPalette::ButtonText, Qt::white);
                pa.setColor(DPalette::Dark, QColor("#25B7FF"));
                pa.setColor(DPalette::Light, QColor("#0098FF"));
            } else {
                pa.setColor(DPalette::ButtonText, "#B8D3FF");
                pa.setColor(DPalette::Dark, QColor("#0056C1"));
                pa.setColor(DPalette::Light, QColor("#004C9C"));
            }
            yesButton->setPalette(pa);
            msgBox.exec();

            if (msgBox.clickButton() == noButton) {
                emit signalViewtransparentFrame(0);
                return;
            } else if (msgBox.clickButton() == yesallbutton) {
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(info.id);
            } else if (msgBox.clickButton() == yesButton) {

                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(info.id, newschedule);
                newschedule.ignore.append(info.beginDateTime);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            }
        } else {
            CSchceduleCtrlDlg msgBox(this);
            msgBox.setText(tr("You are deleting an event."));
            msgBox.setInformativeText(tr("Do you want to delete this and all future occurrences of this event, or only the selected occurrence?"));
            DPushButton *noButton = msgBox.addPushButton(tr("Cancel"));
            DPushButton *yesallbutton = msgBox.addPushButton(tr("Delete All Future Events"));
            DPushButton *yesButton = msgBox.addPushButton(tr("Delete Only This Event"));
            msgBox.updatesize();
            DPalette pa = yesButton->palette();
            if (themetype == 0 || themetype == 1) {
                pa.setColor(DPalette::ButtonText, Qt::white);
                pa.setColor(DPalette::Dark, QColor("#25B7FF"));
                pa.setColor(DPalette::Light, QColor("#0098FF"));
            } else {
                pa.setColor(DPalette::ButtonText, "#B8D3FF");
                pa.setColor(DPalette::Dark, QColor("#0056C1"));
                pa.setColor(DPalette::Light, QColor("#004C9C"));
            }
            yesButton->setPalette(pa);
            msgBox.exec();

            if (msgBox.clickButton() == noButton) {
                emit signalViewtransparentFrame(0);
                return;
            } else if (msgBox.clickButton() == yesallbutton) {
                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(info.id, newschedule);
                newschedule.enddata.type = 2;
                newschedule.enddata.date = info.beginDateTime.addDays(-1);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);

            } else if (msgBox.clickButton() == yesButton) {

                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(info.id, newschedule);
                newschedule.ignore.append(info.beginDateTime);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            }
        }
    }
    emit signalsUpdateShcedule(m_currentItem->getData().id);
}

void CGraphicsView::slotHoverUpdateState(CScheduleItem *item, int state)
{
    if (item == nullptr) return;
    if (item->getType() == 1) return;
    ScheduleDtailInfo baseinfo = item->getData();
    for (int i = 0; i < m_vScheduleItem.size(); i++) {
        if (item == m_vScheduleItem.at(i)) continue;
        ScheduleDtailInfo info = m_vScheduleItem.at(i)->getData();
        if (baseinfo.id == info.id && baseinfo.RecurID == info.RecurID) {
            if (m_vScheduleItem.at(i)->getType() == 1) continue;
            m_vScheduleItem.at(i)->UpdateHoverState(state);
        }
    }
}

void CGraphicsView::slotSelectUpdateState(CScheduleItem *item, int state)
{
    if (item == nullptr) return;
    ScheduleDtailInfo baseinfo = item->getData();
    for (int i = 0; i < m_vScheduleItem.size(); i++) {
        if (item == m_vScheduleItem.at(i)) continue;
        ScheduleDtailInfo info = m_vScheduleItem.at(i)->getData();
        if (baseinfo.id == info.id && baseinfo.RecurID == info.RecurID) {
            if (m_vScheduleItem.at(i)->getType() == 1) continue;
            m_vScheduleItem.at(i)->UpdateSelectState(state);
        }
    }
}

void CGraphicsView::slotScrollBar()
{
    emit signalScheduleShow(false);
}

void CGraphicsView::mouseMoveEvent( QMouseEvent *event )
{
    if (m_press) {
        emit signalScheduleShow(false);
        m_press = false;
    }
    DGraphicsView::mouseMoveEvent(event);
}

#ifndef QT_NO_WHEELEVENT
/************************************************************************
Function:       wheelEvent()
Description:    鼠标滚轮事件
Input:          event 滚轮事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::wheelEvent( QWheelEvent *event )
{
    emit signalScheduleShow(false);
    int test = event -> delta();
    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    QPoint newCenter(viewWidth / 2,  viewHeight / 2 - test);
    QPointF centerpos = mapToScene(newCenter);
    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    centerOn(centerpos.x(), centerpos.y());
    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    scrollBarValueChangedSlot();
//    //DGraphicsView::wheelEvent(event);
}
#endif

/************************************************************************
Function:       keyPressEvent()
Description:    键盘按下事件
Input:          event 键盘事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::keyPressEvent( QKeyEvent *event )
{
    DGraphicsView::keyPressEvent(event);
}

/************************************************************************
Function:       keyReleaseEvent()
Description:    键盘释放事件
Input:          event 键盘事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::keyReleaseEvent( QKeyEvent *event )
{
    DGraphicsView::keyReleaseEvent(event);
}

/************************************************************************
Function:       focusInEvent()
Description:    焦点进入事件
Input:          event 焦点事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::focusInEvent( QFocusEvent *event )
{
    DGraphicsView::focusInEvent(event);
}

/************************************************************************
Function:       focusOutEvent()
Description:    焦点离开事件
Input:          event 焦点事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::focusOutEvent( QFocusEvent *event )
{
    DGraphicsView::focusOutEvent(event);
}

/************************************************************************
Function:       enterEvent()
Description:    进入事件
Input:          event 事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::enterEvent( QEvent *event )
{
    DGraphicsView::enterEvent(event);
}

/************************************************************************
Function:       leaveEvent()
Description:    离开事件
Input:          event 事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::leaveEvent( QEvent *event )
{
    DGraphicsView::leaveEvent(event);
}

/************************************************************************
Function:       contextMenuEvent()
Description:    右键菜单事件
Input:          event 右键菜单事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::contextMenuEvent( QContextMenuEvent *event )
{
    DGraphicsView::contextMenuEvent(event);
}

/************************************************************************
Function:       resizeEvent()
Description:    窗口大小改变事件
Input:          event 窗口大小改变事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::resizeEvent( QResizeEvent *event )
{
    scrollBarValueChangedSlot();
    QGraphicsView::resizeEvent(event);
    viewport()->update();
    update();
}
/************************************************************************
Function:       setLargeScale()
Description:    设置大刻度
Input:          vLRLarge 左右刻度，vTBLarge 上下刻度
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::setLargeScale(const QVector<int> &vLRLarge, const QVector<int> &vTBLarge)
{
    m_vLRLarge = vLRLarge;
    m_vTBLarge = vTBLarge;
    update();
}

/************************************************************************
Function:       paintEvent()
Description:    绘制事件
Input:          event 事件
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::paintEvent(QPaintEvent *event)
{
    QPainter t_painter(viewport());
    //t_painter.setCompositionMode(QPainter::CompositionMode_Difference  ); //设置混合模式
    int t_width = viewport()->width()  + 2;
    int t_height = viewport()->height();
    //绘制垂直线
    if (m_TBFlag) {
        t_painter.save();
        t_painter.setPen(m_TBPen);
        for (int i = 0; i < m_vTBLarge.size(); ++i)
            t_painter.drawLine(QPoint(m_vTBLarge[i] - 1, 0), QPoint(m_vTBLarge[i] - 1, t_height));
        t_painter.restore();
        if (m_totalDay == 7) {
            t_painter.save();
            for (int i = 0; i != 7; ++i) {

                int d = checkDay(i - m_firstWeekDay);

                t_painter.setBrush(QBrush(m_weekcolor));
                t_painter.setPen(Qt::NoPen);
                if ( d == 7) {
                    t_painter.drawRect(QRect(0 + i * m_dayInterval, 0, m_dayInterval + 0, t_height));
                }
                if (d == 6 ) {
                    t_painter.drawRect(QRect(0 + i * m_dayInterval, 0, m_dayInterval + 5, t_height));
                }
            }
            t_painter.restore();
        }
    }
    //绘制水平线
    if (m_LRFlag) {
        if (m_cuttrnttimetype == 0) {
            t_painter.save();
            t_painter.setPen(m_LRPen);
            for (int i = 0; i < m_vLRLarge.size(); ++i)
                t_painter.drawLine(QPoint(0, m_vLRLarge[i] - 1), QPoint(t_width, m_vLRLarge[i] - 1));
            t_painter.restore();
        } else {
            t_painter.save();
            t_painter.setPen(m_LRPen);
            for (int i = 0; i < m_vLRLarge.size() - 1; ++i)
                t_painter.drawLine(QPoint(0, m_vLRLarge[i] - 1), QPoint(t_width, m_vLRLarge[i] - 1));
            t_painter.restore();
            if (m_viewType == 1 && m_coorManage->getBegindate() == QDate::currentDate()) {
                t_painter.save();
                QPen pen = m_LRPen;
                pen.setColor(m_currenttimecolor);
                t_painter.setPen(pen);
                int index = m_vLRLarge.count() - 1;
                t_painter.drawLine(QPoint(0, m_vLRLarge[index] - 1), QPoint(t_width, m_vLRLarge[index] - 1));
                t_painter.restore();
            }

        }
    }
    QGraphicsView::paintEvent(event);
}

void CGraphicsView::scrollBarValueChangedSlot()
{
    emit signalScheduleShow(false);
    QMutexLocker locker(&m_Mutex);
    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    //QPoint newCenter(viewWidth / 2,  viewHeight / 2 );
    //QPointF centerpos = mapToScene(newCenter);
    //setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //centerOn(centerpos.x(), centerpos.y());
    //setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    m_vLRLarge.clear();
    m_vTBLarge.clear();
    QPointF leftToprealPos = mapToScene(QPoint(0, 0));
    QPointF leftBttomrealPos = mapToScene(QPoint(0, viewHeight));

    for (float i = m_dayInterval; i < scene()->width(); i = i + m_dayInterval) {
        m_vTBLarge.append(i);
    }
    float beginpos = (int)(leftToprealPos.y() / m_timeInterval) * m_timeInterval;
    if (beginpos < leftToprealPos.y()) {
        beginpos = (beginpos / m_timeInterval + 1) * m_timeInterval ;
    }
    QVector<int> vHours;
    for (float i = beginpos; i < leftBttomrealPos.y(); i = i + m_timeInterval) {
        QPoint point = mapFromScene(leftBttomrealPos.x(), i);
        m_vLRLarge.append(point.y());
        vHours.append(i / m_timeInterval + 0.5);
    }
    float currentTime =  m_coorManage->getHeight(QTime::currentTime());
    if (currentTime >= beginpos && currentTime <= leftBttomrealPos.y()) {
        //if (0) {
        m_cuttrnttimetype = 1;
        QPoint point = mapFromScene(leftBttomrealPos.x(), currentTime);
        m_vLRLarge.append(point.y());
        vHours.append(currentTime / m_timeInterval + 0.5);
        emit signalsPosHours(m_vLRLarge, vHours, m_cuttrnttimetype);
    } else {
        m_cuttrnttimetype = 0;
        emit signalsPosHours(m_vLRLarge, vHours, m_cuttrnttimetype);
    }
    scene()->update();
    update();
}

int CGraphicsView::checkDay(int weekday)
{
    if (weekday <= 0)
        return weekday += 7;

    if (weekday > 7)
        return weekday -= 7;

    return weekday;
}

/************************************************************************
Function:       setLargeScaleInfo()
Description:    设置大刻度显示
Input:          LRFlag 水平刻度，TBFlag 垂直刻度
Output:         无
Return:         无
Others:         无
************************************************************************/
void CGraphicsView::setLargeScaleFlag(const bool &LRFlag, const bool &TBFlag)
{
    m_LRFlag = LRFlag;
    m_TBFlag = TBFlag;
    scene()->update();
}

void CGraphicsView::getLargeScaleFlag(bool &LRFlag, bool &TBFlag)
{
    LRFlag = m_LRFlag;
    TBFlag = m_TBFlag;
}

void CGraphicsView::setLargeScalePen(const QPen &LRPen, const QPen &TBPen)
{
    m_LRPen = LRPen;
    m_TBPen = TBPen;
    update();
}

void CGraphicsView::setFirstWeekday(int weekday)
{
    m_firstWeekDay = weekday;
}

void CGraphicsView::setTime(QTime time)
{
    int viewWidth = viewport()->width();
    int viewHeight = viewport()->height();
    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    QPoint newCenter(viewWidth / 2,  viewHeight / 2);
    QPointF centerpos = mapToScene(newCenter);
    centerpos = QPointF(centerpos.x(), m_coorManage->getHeight(time));
    centerOn(centerpos.x(), centerpos.y());

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);

    scrollBarValueChangedSlot();
}
