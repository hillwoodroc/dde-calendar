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
#include "monthschceduleview.h"
#include <QAction>
#include <QMenu>
#include <QListWidget>
#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>
#include <QStylePainter>
#include <QPainterPath>
#include <QRect>
#include "schceduledlg.h"
#include "myschceduleview.h"
#include <DMessageBox>
#include <DPushButton>
#include <DHiDPIHelper>
#include <DPalette>
#include "schcedulectrldlg.h"
#include <QShortcut>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include "SchecduleRemindWidget.h"
#include "monthview.h"

DGUI_USE_NAMESPACE

CMonthSchceduleWidgetItem::CMonthSchceduleWidgetItem( QWidget *parent /*= nullptr*/, int edittype): DPushButton(parent)
{
    m_editType = edittype;
    //setMargin(0);
    setMouseTracking(true);
    m_editAction = new QAction(tr("Edit"), this);
    m_deleteAction = new QAction(tr("Delete"), this);
    connect(m_editAction, SIGNAL(triggered(bool)), this, SLOT(slotEdit()));
    connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(slotDelete()));
    connect(this, SIGNAL(pressed()), this, SLOT(slotPress()));
    setAttribute(Qt::WA_DeleteOnClose, true);
    const int duration = 200;
    m_properAnimationFirst = new QPropertyAnimation( this, "setRectOffset", this);
    m_properAnimationFirst->setObjectName("First");
    m_properAnimationSecond = new QPropertyAnimation( this, "setRectOffset", this);
    m_properAnimationSecond->setObjectName("Second");
    m_properAnimationFirst->setDuration(duration);
    m_properAnimationSecond->setDuration(duration);
    m_properAnimationFirst->setEasingCurve(QEasingCurve::InOutQuad);
    m_properAnimationSecond->setEasingCurve(QEasingCurve::InOutQuad);
    m_Group = new QSequentialAnimationGroup(this);
    m_Group->addAnimation(m_properAnimationFirst);
    m_Group->addAnimation(m_properAnimationSecond);
    connect(m_Group
            , &QPropertyAnimation::finished
            , this
            , &CMonthSchceduleWidgetItem::animationFinished);
}

CMonthSchceduleWidgetItem::~CMonthSchceduleWidgetItem()
{
    disconnect(m_editAction, SIGNAL(triggered(bool)), this, SLOT(slotEdit()));
    disconnect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(slotDelete()));
}

void CMonthSchceduleWidgetItem::setColor( QColor color1, QColor color2, bool GradientFlag /*= false*/ )
{
    m_color1 = color1;
    m_color2 = color2;
    m_GradientFlag = GradientFlag;
}

void CMonthSchceduleWidgetItem::setSizeType(DFontSizeManager::SizeType sizeType)
{
    m_SizeType = sizeType;
}

void CMonthSchceduleWidgetItem::setText( QColor tcolor, QFont font, QPoint pos)
{
    m_textcolor = tcolor;
    m_font = font;
    m_pos = pos;
}

void CMonthSchceduleWidgetItem::getColor(QColor &color1, QColor &color2, bool &GradientFlag)
{
    color2 = m_color2;
    color1 = m_color1;
    GradientFlag = m_GradientFlag;
}

void CMonthSchceduleWidgetItem::getText(QColor &tcolor, QFont &font, QPoint &pos)
{
    tcolor = m_textcolor;
    font = m_font;
    pos = m_pos;
}

void CMonthSchceduleWidgetItem::setTransparentB(bool t, QColor tcolor)
{
    m_transparentcolor = tcolor;
    m_transparentf = t;
}

void CMonthSchceduleWidgetItem::setTransparentB(bool t)
{
    m_transparentf = t;
    update();
}

void CMonthSchceduleWidgetItem::setData( ScheduleDtailInfo vScheduleInfo )
{
    m_ScheduleInfo = vScheduleInfo;
    update();
}

void CMonthSchceduleWidgetItem::setRect(int x, int y, int w, int h)
{
    m_rect = QRect(x,y,w,h);
}

void CMonthSchceduleWidgetItem::setRectOffset(int offset)
{
    QRect rect(m_rect.x() - offset
               , m_rect.y() - offset / 2
               , m_rect.width() + offset * 2
               , m_rect.height() + offset);
    m_widthoffset = offset*2;
    this->setGeometry(rect);
    this->setFixedSize(rect.width(), rect.height());
}

void CMonthSchceduleWidgetItem::startAnimation()
{
    if (isAnimation)
        return;
    m_Group->start();
    isAnimation = true;

}

void CMonthSchceduleWidgetItem::setStartValue(int offset)
{
    m_properAnimationFirst->setStartValue(offset);
    m_properAnimationSecond->setEndValue(offset);
//    m_rect = this->geometry();
}

void CMonthSchceduleWidgetItem::setEndValue(int offset)
{
    m_properAnimationFirst->setEndValue(offset);
    m_properAnimationSecond->setStartValue(offset);
}

void CMonthSchceduleWidgetItem::slotEdit()
{
    CMonthView *w = qobject_cast<CMonthView *>(this->parent());
    CSchceduleDlg dlg(0, w);
    connect(&dlg, &CSchceduleDlg::signalScheduleUpdate, w, &CMonthView::slotSchceduleUpdate);
    connect(&dlg, &CSchceduleDlg::signalScheduleUpdate, w, &CMonthView::slotdelete);
    connect(&dlg, &CSchceduleDlg::signalViewtransparentFrame,
            w, &CMonthView::signalViewtransparentFrame);
    dlg.setData(m_ScheduleInfo);
    dlg.exec();
}

void CMonthSchceduleWidgetItem::slotDelete()
{
    emit signalViewtransparentFrame(1);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();
    if (m_ScheduleInfo.rpeat == 0) {
        CSchceduleCtrlDlg msgBox;
        //msgBox.setWindowFlags(Qt::FramelessWindowHint);
        //msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34, 34) * devicePixelRatioF()));

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
            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(m_ScheduleInfo.id);
        }
    } else {
        if (m_ScheduleInfo.RecurID == 0) {
            CSchceduleCtrlDlg msgBox;
            //msgBox.setWindowFlags(Qt::FramelessWindowHint);
            //msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34, 34) * devicePixelRatioF()));

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
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(m_ScheduleInfo.id);
            } else if (msgBox.clickButton() == yesButton) {

                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_ScheduleInfo.id, newschedule);
                newschedule.ignore.append(m_ScheduleInfo.beginDateTime);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            }
        } else {
            CSchceduleCtrlDlg msgBox;
            //msgBox.setWindowFlags(Qt::FramelessWindowHint);
            //msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34, 34) * devicePixelRatioF()));
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
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_ScheduleInfo.id, newschedule);
                newschedule.enddata.type = 2;
                newschedule.enddata.date = m_ScheduleInfo.beginDateTime.addDays(-1);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);

            } else if (msgBox.clickButton() == yesButton) {

                ScheduleDtailInfo newschedule;
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_ScheduleInfo.id, newschedule);
                newschedule.ignore.append(m_ScheduleInfo.beginDateTime);
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            }
        }
    }
    emit signalsDelete(this);
    emit signalViewtransparentFrame(0);


}
//有问题
void CMonthSchceduleWidgetItem::slotDoubleEvent(int type)
{
    emit signalsEdit(this, 1);
}

void CMonthSchceduleWidgetItem::slotPress()
{
    // m_transparentf = true;
    emit signalsPress(this);
}

void CMonthSchceduleWidgetItem::animationFinished()
{
    isAnimation = false;
}

void CMonthSchceduleWidgetItem::paintEvent( QPaintEvent *e )
{
//    QRect ur=e->rect();//得到组件尺寸
//    QPixmap pix(this->size());//以此为参数创建一个位图变量
//    pix.fill(qRgba(255,255,255,0));
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    int labelwidth = width();
    int labelheight = height();
    float avge = 1;
    m_font = DFontSizeManager::instance()->get(m_SizeType, m_font);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();
    gdcolor = CScheduleDataManage::getScheduleDataManage()->getScheduleColorByType(m_ScheduleInfo.type.ID);
    m_highflag = CScheduleDataManage::getScheduleDataManage()->getSearchResult(m_ScheduleInfo);
    if (m_GradientFlag) {
        QLinearGradient linearGradient(2, 0, labelwidth - 2, 0);
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

        QRectF fillRect = QRectF(2, 2 * avge, labelwidth - 2, labelheight - 2 * avge);
        painter.save();
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setBrush(linearGradient);
        painter.setPen(Qt::NoPen);
//        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.drawRoundedRect(fillRect, 8, 8);
//        painter.fillRect(fillRect, linearGradient);
        painter.restore();
        painter.setFont(m_font);
        painter.setPen(textcolor);
        QFontMetrics fm = painter.fontMetrics();
        if (he != fm.height()) {
            he = fm.height();
            emit signalupdatehe(fm.height());
        }

        QString tStitlename = m_ScheduleInfo.titleName;
        tStitlename.replace("\n", "");
        QString str = tStitlename;
        QString tstr;
        for (int i = 0; i < str.count(); i++) {
            tstr.append(str.at(i));
            int widthT = fm.width(tstr) + 5;
            if (widthT >= labelwidth - m_pos.x()) {
                tstr.chop(2);
                break;
            }
        }
        if (tstr != str) {
            tstr = tstr + "...";
        }

        painter.drawText(QRect(m_pos.x(), 1, labelwidth - m_pos.x()-m_widthoffset,
                               labelheight - m_pos.y() + 3 * avge ),
                         Qt::AlignLeft | Qt::AlignVCenter, tstr);

        if (m_hoverflag && !m_selectflag) {
            QRectF trect = QRectF(2.5, 2.5, labelwidth - 3, labelheight - 3);
            // trect.setHeight(fillRect.height());
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
        painter.drawText(QRect(m_pos.x(), m_pos.y(), labelwidth - m_pos.x(), labelheight - m_pos.y() + 4 * avge), Qt::AlignLeft, str);
    }
    painter.end();
//    QPainter painteritem(this);
//    painteritem.drawPixmap(this->rect(),pix);
//    painteritem.end();
}
void CMonthSchceduleWidgetItem::contextMenuEvent( QContextMenuEvent *event )
{
    emit signalPressScheduleShow(false);
    if (m_ScheduleInfo.type.ID == 4) {

    } else {
        emit signalUpdateUI(0);
        DMenu Context(qobject_cast<QWidget *>(this->parent()));
        Context.setAttribute(Qt::WA_DeleteOnClose);
        Context.addAction(m_editAction);
        Context.addAction(m_deleteAction);
        Context.exec(QCursor::pos());
        emit signalUpdateUI(1);
    }
}

void CMonthSchceduleWidgetItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    //if (m_editType == 0) return;
    if (event->button() == Qt::LeftButton) {
        emit signalPressScheduleShow(false);
        CMonthView *w = qobject_cast<CMonthView *>(this->parent());
        CMySchceduleView dlg(m_ScheduleInfo, w);

        connect(&dlg, &CMySchceduleView::signalsEditorDelete, w, &CMonthView::slotSchceduleUpdate);
        connect(&dlg, &CMySchceduleView::signalsEditorDelete, w, &CMonthView::slotdelete);
        connect(&dlg, &CMySchceduleView::signalViewtransparentFrame,
                w, &CMonthView::signalViewtransparentFrame);
        dlg.exec();
    }
}

void CMonthSchceduleWidgetItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_selectflag = true;
        m_pressRemindTag = true;
        m_pressMove = true;
        m_pressPos = event->pos();
        update();
        slotPress();
        emit signalPressScheduleShow(true, m_ScheduleInfo);
    }
}

void CMonthSchceduleWidgetItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_selectflag = false;
        m_pressRemindTag = false;
        m_pressMove = false;
        update();
    }
}

void CMonthSchceduleWidgetItem::focusOutEvent(QFocusEvent *event)
{
    //m_selectflag = false;
    update();
}

void CMonthSchceduleWidgetItem::enterEvent(QEvent *event)
{
    m_hoverflag = true;
    update();
}

void CMonthSchceduleWidgetItem::leaveEvent(QEvent *event)
{
    m_hoverflag = false;
    m_pressRemindTag = false;
    update();
}

void CMonthSchceduleWidgetItem::mouseMoveEvent(QMouseEvent *e)
{
    if (m_pressRemindTag) {
        emit signalPressScheduleShow(false);
        m_pressRemindTag = false;
    }
    if (m_pressMove) {
    }
}


CMonthSchceduleNumButton::CMonthSchceduleNumButton(QWidget *parent /*= nullptr*/): DPushButton(parent)
{

}

CMonthSchceduleNumButton::~CMonthSchceduleNumButton()
{

}

void CMonthSchceduleNumButton::setColor( QColor color1, QColor color2, bool GradientFlag /*= false*/ )
{
    m_color1 = color1;
    m_color2 = color2;
    m_GradientFlag = GradientFlag;
}

void CMonthSchceduleNumButton::setText( QColor tcolor, QFont font, QPoint pos)
{
    m_textcolor = tcolor;
    m_font = font;
    m_pos = pos;
}

void CMonthSchceduleNumButton::setSizeType(DFontSizeManager::SizeType sizeType)
{
    m_SizeType = sizeType;
}

void CMonthSchceduleNumButton::setTransparentB(bool t, QColor tcolor)
{
    m_transparentcolor = tcolor;
    m_transparentf = t;
}

void CMonthSchceduleNumButton::paintEvent(QPaintEvent *e)
{
    int labelwidth = width();
    int labelheight = height() - 6;
    int type = CScheduleDataManage::getScheduleDataManage()->getTheme();
    m_font = DFontSizeManager::instance()->get(m_SizeType, m_font);
    QPainter painter(this);
    if (m_GradientFlag) {
        QLinearGradient linearGradient(0, 0, labelwidth, 0);
        linearGradient.setColorAt(0, m_color1);
        linearGradient.setColorAt(1, m_color2);
        QRect fillRect = QRect(2, 1, labelwidth - 2, labelheight - 1);
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(linearGradient);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(fillRect, 3, 2);

        painter.setFont(m_font);
        painter.setPen(m_textcolor);

        QString str =  QString(tr("%1 more")).arg(m_num) + "...";
        QFont tfont = m_font;
        QFontMetrics fm = painter.fontMetrics();

        QString tstr;
        for (int i = 0; i < str.count(); i++) {
            tstr.append(str.at(i));
            int widthT = fm.width(tstr) + 5;
            if (widthT >= labelwidth - m_pos.x()) {
                tstr.chop(2);
                break;
            }
        }
        if (tstr != str) {
            tstr = tstr + "...";
        }

        //while (fm.width(str) > labelwidth - m_pos.x()) {
        //   tfont.setPixelSize(tfont.pixelSize() - 1);
        //   painter.setFont(tfont);
        //   fm = painter.fontMetrics();
        //}

        painter.drawText(QRect(m_pos.x(), m_pos.y(), labelwidth - m_pos.x(), labelheight + 4), Qt::AlignCenter, tstr);
        //if (m_transparentf) {
        //  painter.setBrush(m_transparentcolor);
        //  painter.setPen(Qt::NoPen);
        //   painter.drawRoundedRect(fillRect, 3, 2);
        // }
    } else {
        QRect fillRect = QRect(2, 1, labelwidth - 2, labelheight - 1);
        //将直线开始点设为0，终点设为1，然后分段设置颜色
        painter.setRenderHints(QPainter::HighQualityAntialiasing);
        painter.setBrush(m_color1);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(fillRect, 3, 2);

        painter.setFont(m_font);
        painter.setPen(m_textcolor);
        painter.drawText(QRect(m_pos.x(), m_pos.y(), labelwidth - m_pos.x(), labelheight + 4), Qt::AlignLeft, QString(tr("%1 more")).arg(m_num));
    }
}

void CMonthSchceduleNumButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_date.isValid()) {
        emit  signalsCurrentScheduleDate(m_date);
    }
}

void CMonthSchceduleNumButton::mousePressEvent(QMouseEvent *event)
{
    signalPressScheduleShow(false);
}

void CMonthSchceduleView::setTheMe(int type)
{
    updateData();
}

CMonthSchceduleView::CMonthSchceduleView(QWidget *parent) : QObject (parent), m_parernt(parent)
{
    QShortcut *shortcut = new QShortcut(parent);
    shortcut->setKey(QKeySequence(QLatin1String("Delete")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(slotDeleteItem()));

    for (int i = 0; i < 6; ++i) {
        CWeekScheduleView *weekSchedule = new CWeekScheduleView(this);
        m_weekSchedule.append(weekSchedule);
    }
}

CMonthSchceduleView::~CMonthSchceduleView()
{

}

void CMonthSchceduleView::setallsize(int w, int h, int left, int top, int buttom, int itemHeight)
{
    m_width = w;
    m_height = h;
    m_buttommagin = buttom;
    m_leftMagin = left;
    m_topMagin = top;

    m_cNum = ((m_height - m_topMagin - m_buttommagin) / 6.0 + 0.5  - 27) / (itemHeight + 1);
}

void CMonthSchceduleView::setData(QVector<ScheduleDateRangeInfo> &data, int currentMonth)
{
    m_data = data;
    m_currentMonth = currentMonth;
    updateData();
}

void CMonthSchceduleView::slotdeleteitem( CMonthSchceduleWidgetItem *item)
{
    emit signalsUpdateShcedule(item->getData().id);
}

void CMonthSchceduleView::slotedititem(CMonthSchceduleWidgetItem *item, int type)
{
    emit signalsUpdateShcedule(item->getData().id);
}

void CMonthSchceduleView::slotupdateItem(CMonthSchceduleWidgetItem *item)
{
    m_currentitem = item;
    return;
}

void CMonthSchceduleView::slotDeleteItem()
{
    if (m_currentitem != nullptr) {
        CMonthSchceduleWidgetItem *titem = qobject_cast<CMonthSchceduleWidgetItem *>(m_currentitem);
        if (titem != nullptr) {
            titem->slotDelete();
        }
    }
}

void CMonthSchceduleView::slotUpdatehe(int h)
{
    if (he != h) {
        if (h < 22 ) {
            m_cNum = ((m_height - m_topMagin - m_buttommagin) / 6.0 + 0.5  - 27) / 23;
        } else {
            he = h;
            m_cNum = ((m_height - m_topMagin - m_buttommagin) / 6.0 + 0.5  - 27) / (he + 1);
            updateData();
        }
    }
}

bool MScheduleDateThan(const MScheduleDateRangeInfo &s1, const MScheduleDateRangeInfo &s2)
{
    if (s1.bdate != s1.edate && s2.bdate == s2.edate) {
        return true;
    } else if (s1.bdate == s1.edate && s2.bdate != s2.edate) {
        return false;
    } else if (s1.bdate != s1.edate && s2.bdate != s2.edate) {
        if (s1.tData.beginDateTime == s2.tData.beginDateTime) {
            return s1.bdate.daysTo(s1.edate) > s2.bdate.daysTo(s2.edate);
        } else {
            return s1.bdate < s2.bdate;
        }
    } else {
        if (s1.tData.beginDateTime == s2.tData.beginDateTime) {
            return s1.tData.titleName < s2.tData.titleName;
        } else {
            return s1.tData.beginDateTime < s2.tData.beginDateTime;
        }
    }
}
bool MScheduleDaysThan(const MScheduleDateRangeInfo &s1, const MScheduleDateRangeInfo &s2)
{
    return s1.bdate.daysTo(s1.edate) > s2.bdate.daysTo(s2.edate);
}
void CMonthSchceduleView::updateData()
{
    //保护数据防止越界
    if (m_data.count() != 42 || m_cNum < 1) return;
    //开始结束时间
    QDate begindate = m_data.begin()->date;
    QDate enddate = m_data[m_data.count() - 1].date;
    m_beginDate = begindate;
    m_endDate  = enddate;
    m_currentitem = nullptr;

    for (int i = 0 ; i < m_weekSchedule.size(); ++i) {
        m_weekSchedule[i]->clearItem();
        m_weekSchedule[i]->setData(m_data,i*7,7);
        m_weekSchedule[i]->setHeight(he,(m_height - m_topMagin - m_buttommagin) / 6.0-27);
        m_weekSchedule[i]->updateSchedule();
        QVector<QVector<MScheduleDateRangeInfo> > mSchedule = m_weekSchedule[i]->getMScheduleInfo();
        updateDateShow(mSchedule,m_weekSchedule[i]->getScheduleShowItem());
    }
    return;
}

void CMonthSchceduleView::updateHigh()
{
    for (int j = 0 ; j < m_weekSchedule.size(); ++j) {
        for (int i = 0; i < m_weekSchedule[j]->getScheduleShowItem().count(); i++) {
            m_weekSchedule[j]->getScheduleShowItem().at(i)->update();
        }
    }

}

QVector<DPushButton *> CMonthSchceduleView::getScheduleShowItem() const
{
    QVector<DPushButton *>   m_scheduleShowItem;
    for (int j = 0 ; j < m_weekSchedule.size(); ++j) {
        for (int i = 0; i < m_weekSchedule[j]->getScheduleShowItem().count(); i++) {
            m_scheduleShowItem.append(m_weekSchedule[j]->getScheduleShowItem().at(i));
        }
    }
    return m_scheduleShowItem;
}
void CMonthSchceduleView::updateDateShow(QVector<QVector<MScheduleDateRangeInfo> > &vCMDaySchedule, QVector<DPushButton *> &schudeleShowItem)
{
    for (int i = 0; i < vCMDaySchedule.count(); i++) {
        for (int j = 0; j < vCMDaySchedule[i].count(); j++) {
            if (vCMDaySchedule[i].at(j).state) {
                createScheduleNumWidget(vCMDaySchedule[i].at(j), i + 1,schudeleShowItem);
            } else {
                createScheduleItemWidget(vCMDaySchedule[i].at(j), i + 1,schudeleShowItem);
            }
        }
    }
}

void CMonthSchceduleView::splitSchedule(MScheduleDateRangeInfo &old, QVector<MScheduleDateRangeInfo> &newData)
{
    int brow = (m_beginDate.daysTo(old.bdate) + 1) / 7;
    int erow = (m_beginDate.daysTo(old.edate) + 1) / 7;
    int bcol = (m_beginDate.daysTo(old.bdate) + 1) % 7;
    int ecol = (m_beginDate.daysTo(old.edate) + 1) % 7;
    if (bcol != 0) brow = brow + 1;
    if (ecol != 0) erow = erow + 1;
    if (brow > erow) {
        return;
    }
    if (brow == erow) {
        newData.append(old);
    } else {
        bcol =  bcol ? bcol : 7;
        ecol =  ecol ? ecol : 7;
        //处理开始
        MScheduleDateRangeInfo bfirst = old;
        bfirst.edate = bfirst.bdate.addDays(7 - bcol);
        newData.append(bfirst);
        //处理中间数据
        for (int i = brow + 1; i < erow; i++) {
            MScheduleDateRangeInfo info = old;
            info.bdate = m_beginDate.addDays((i - 1) * 7);
            info.edate = m_beginDate.addDays((i - 1) * 7 + 6);
            newData.append(info);
        }

        //处理结束
        MScheduleDateRangeInfo second = old;
        second.bdate = second.edate.addDays(-ecol + 1);
        newData.append(second);
    }
}

void CMonthSchceduleView::createScheduleItemWidget(MScheduleDateRangeInfo info, int cnum,QVector<DPushButton *> &schudeleShowItem)
{
    ScheduleDtailInfo gd = info.tData;
    CSchedulesColor gdcolor = CScheduleDataManage::getScheduleDataManage()->getScheduleColorByType(gd.type.ID);
    CMonthSchceduleWidgetItem *gwi = new CMonthSchceduleWidgetItem(m_parernt);
    QPoint pos;
    int fw;
    int fh;
    computePos(cnum, info.bdate, info.edate, pos, fw, fh);
    gwi->setColor(gdcolor.gradientFromC, gdcolor.gradientToC, true);
    QFont font("PingFangSC-Light");
//    font = DFontSizeManager::instance()->get(DFontSizeManager::T8,font);
    gwi->setSizeType(DFontSizeManager::T8);
//    font.setPixelSize(12);

    gwi->setData(gd);
    gwi->setFixedSize(fw, fh);
    gwi->setText(gdcolor.textColor, font, QPoint(13, 3));
    gwi->move(pos);
    gwi->setRect(pos.x(),pos.y(),fw,fh);
    //if (m_currentMonth != info.bdate.month() && m_currentMonth != info.edate.month()) {
    // QColor TransparentC = "#000000";
    //TransparentC.setAlphaF(0.05);
    //gwi->setTransparentB(true, TransparentC);
    //}
    QColor TransparentC = "#000000";
    TransparentC.setAlphaF(0.05);
    gwi->setTransparentB(false, TransparentC);
    gwi->setWindowFlags(gwi->windowFlags() | Qt::WindowStaysOnTopHint);
    gwi->show();
    connect(gwi, &CMonthSchceduleWidgetItem::signalsDelete, this, &CMonthSchceduleView::slotdeleteitem);
    connect(gwi, &CMonthSchceduleWidgetItem::signalsEdit, this, &CMonthSchceduleView::slotedititem);
    connect(gwi, &CMonthSchceduleWidgetItem::signalsPress, this, &CMonthSchceduleView::slotupdateItem);
    connect(gwi, &CMonthSchceduleWidgetItem::signalViewtransparentFrame, this, &CMonthSchceduleView::signalViewtransparentFrame);
    connect(gwi, &CMonthSchceduleWidgetItem::signalUpdateUI, this, &CMonthSchceduleView::signalUpdateUI);
    connect(gwi, &CMonthSchceduleWidgetItem::signalPressScheduleShow, this, &CMonthSchceduleView::signalPressScheduleShow);
    connect(gwi,&CMonthSchceduleWidgetItem::signalupdatehe,this,&CMonthSchceduleView::slotUpdatehe);

    schudeleShowItem.append(gwi);
}

void CMonthSchceduleView::createScheduleNumWidget(MScheduleDateRangeInfo info, int cnum,QVector<DPushButton *> &schudeleShowItem)
{
    int type = CScheduleDataManage::getScheduleDataManage()->getTheme();
    CMonthSchceduleNumButton *gwi = new CMonthSchceduleNumButton(m_parernt);
    QPoint pos;
    int fw;
    int fh;
    computePos(cnum, info.bdate, info.edate, pos, fw, fh);
    QColor gradientFromC = "#000000";
    gradientFromC.setAlphaF(0.00);
    gwi->setColor(gradientFromC, gradientFromC, true);
    QFont font;
//    font = DFontSizeManager::instance()->get(DFontSizeManager::T8,font);
    gwi->setSizeType(DFontSizeManager::T8);
//    font.setPixelSize(12);
    if (type == 0 || type == 1) {
        QColor tc("#5E5E5E");
        tc.setAlphaF(0.9);
        gwi->setText(tc, font, QPoint(13, 3));
    } else {
        QColor tc("#798190");
        tc.setAlphaF(1);
        gwi->setText(tc, font, QPoint(13, 3));
    }
    gwi->setFixedSize(fw, fh);
    gwi->setData(info.num);
    gwi->setDate(info.bdate);
    if (m_currentMonth != info.bdate.month() && m_currentMonth != info.edate.month()) {
        QColor TransparentC = "#000000";
        TransparentC.setAlphaF(0.05);
        gwi->setTransparentB(true, TransparentC);
    }
    connect(gwi, &CMonthSchceduleNumButton::signalsCurrentScheduleDate, this, &CMonthSchceduleView::signalsCurrentScheduleDate);
    connect(gwi, &CMonthSchceduleNumButton::signalPressScheduleShow, this, &CMonthSchceduleView::signalPressScheduleShow);

    gwi->move(pos);
    gwi->setWindowFlags(gwi->windowFlags() | Qt::WindowStaysOnTopHint);
    gwi->show();
    schudeleShowItem.append(gwi);
}

void CMonthSchceduleView::computePos(int cnum, QDate bgeindate, QDate enddate, QPoint &pos, int &fw, int &fh)
{
    int brow = (m_beginDate.daysTo(bgeindate)) / 7;

    int bcol = (m_beginDate.daysTo(bgeindate) ) % 7;
    int ecol = (m_beginDate.daysTo(enddate) ) % 7;

    fw = (ecol - bcol + 1) * ((m_width - m_leftMagin ) / 7.0) - 11;
    fh = 22;
    int x = m_leftMagin + bcol * ((m_width - m_leftMagin )  / 7.0) + 5;
    int y = m_topMagin + ((m_height - m_topMagin - m_buttommagin) * brow / 6.0 + 0.5)  + 27 + (cnum - 1) * fh + 2.9;
    pos = QPoint(x, y);
}



CWeekScheduleView::CWeekScheduleView(QObject *parent)
    :QObject (parent),
     m_ScheduleHeight(22),
     m_DayHeight(47)
{
    setMaxNum();
}

CWeekScheduleView::~CWeekScheduleView()
{

}

void CWeekScheduleView::setData(QVector<ScheduleDateRangeInfo> &data, const int position, const int count)
{
    int endPos = position+count;
    Q_ASSERT(!(endPos>data.size()));
    m_ScheduleInfo.clear();
    for (int i = position; i<endPos; ++i) {
        for (int j = 0 ; j < data.at(i).vData.size(); ++j) {
            if (!m_ScheduleInfo.contains(data.at(i).vData.at(j))) {
                m_ScheduleInfo.append(data.at(i).vData.at(j));
            }
        }
    }
    beginDate  = data.at(position).date;
    endDate = data.at(position+count -1).date;
    m_colum = count;
    updateSchedule();
}

void CWeekScheduleView::setHeight(const int ScheduleHeight, const int DayHeigth)
{
    m_ScheduleHeight = ScheduleHeight;
    m_DayHeight = DayHeigth;
    setMaxNum();
}

void CWeekScheduleView::updateSchedule()
{
    QDate   tbegindate,tenddate;
    QVector<MScheduleDateRangeInfo> vMDaySchedule;
    m_ColumnScheduleCount.clear();
    m_ColumnScheduleCount.fill(0,m_colum);
    for (int i = 0 ; i < m_ScheduleInfo.size(); ++i) {
        //日程时间重新标定
        tbegindate = m_ScheduleInfo.at(i).beginDateTime.date();
        tenddate = m_ScheduleInfo.at(i).endDateTime.date();
        if (tbegindate <  beginDate) tbegindate = beginDate;
        if (tenddate > endDate) tenddate = endDate;
        //日程信息
        MScheduleDateRangeInfo info;
        info.bdate = tbegindate;
        info.edate = tenddate;
        info.tData = m_ScheduleInfo.at(i);
        info.state = false;
        vMDaySchedule.append(info);
        qint64 pos = beginDate.daysTo(info.bdate);
        qint64 count = info.bdate.daysTo(info.edate);
        int j = pos;
        for (; j < (pos+ count+1); ++j) {
            ++m_ColumnScheduleCount[j];
        }
    }
    qSort(vMDaySchedule.begin(),vMDaySchedule.end());
    sortAndFilter(vMDaySchedule);
}

void CWeekScheduleView::clearItem()
{
    for (int i = 0; i < m_scheduleShowItem.count(); i++) {

        m_scheduleShowItem[i]->deleteLater();
    }
    m_scheduleShowItem.clear();
}

void CWeekScheduleView::setMaxNum()
{
    m_MaxNum = m_DayHeight/(m_ScheduleHeight+1);
}

void CWeekScheduleView::mScheduleClear()
{
    for (int i = 0; i < m_MScheduleInfo.size(); ++i) {
        m_MScheduleInfo[i].clear();
    }
    m_MScheduleInfo.clear();
}

void CWeekScheduleView::sortAndFilter(QVector<MScheduleDateRangeInfo> &vMDaySchedule)
{
    QVector<QVector<bool> > scheduleFill;
    QVector<bool> scheduf;
    scheduf.fill(false,m_colum);
    scheduleFill.fill(scheduf,m_MaxNum);
    int postion = 0;
    int end = 0;
    mScheduleClear();

    for (int i = 0 ; i < vMDaySchedule.size(); ++i) {
        postion = beginDate.daysTo(vMDaySchedule.at(i).bdate);
        end     = beginDate.daysTo(vMDaySchedule.at(i).edate);
        int row = 0;
        int pos = postion;
        int count = 0;
        int scheduleRow = row;
        for (; postion<end+1; ++postion) {
            if (row == m_MaxNum) {
                if (m_ColumnScheduleCount[postion] >m_MaxNum) {
                    continue;
                }
                row =0;
                pos = postion;
            }
            while (row<m_MaxNum) {
                if (m_MScheduleInfo.size()<(row+1)) {
                    RowScheduleInfo ms;
                    m_MScheduleInfo.append(ms);
                }
                if (!scheduleFill[row][postion]) {
                    if ((m_ColumnScheduleCount[postion]>m_MaxNum) &&(row>=m_MaxNum-1)) {
                        scheduleFill[row][postion] = true;
                        if (pos !=postion) {
                            MScheduleDateRangeInfo scheduleInfo;
                            scheduleInfo.bdate = beginDate.addDays(pos);
                            scheduleInfo.edate = beginDate.addDays(postion -1);
                            scheduleInfo.state = false;
                            scheduleInfo.tData = vMDaySchedule.at(i).tData;
                            m_MScheduleInfo[row].append(scheduleInfo);
                        }

                        MScheduleDateRangeInfo info;
                        info.bdate = beginDate.addDays(postion);
                        info.edate = info.bdate;
                        info.num = m_ColumnScheduleCount[postion] -m_MaxNum +1;
                        info.state = true;
                        m_MScheduleInfo[row].append(info);

                        pos = postion +1;
                        if (pos<7 && pos <end +1) {
                            if (m_ColumnScheduleCount[pos]<row+1) {
                                row =m_ColumnScheduleCount[pos]-1;
                            }
                        } else {
                            row = 0;
                        }
                        count = 0;
                    } else {
                        scheduleFill[row][postion] = true;
                        ++count;
                        scheduleRow = row;
                    }
                    break;
                } else {
                    ++row;
                }
            }
        }
        if (pos>6||count==0) {

        } else {
            MScheduleDateRangeInfo scheduleInfo;
            scheduleInfo.bdate = beginDate.addDays(pos);
            scheduleInfo.edate = beginDate.addDays(postion -1);
            scheduleInfo.state = false;
            scheduleInfo.tData = vMDaySchedule.at(i).tData;
            m_MScheduleInfo[scheduleRow].append(scheduleInfo);
        }
    }
}
