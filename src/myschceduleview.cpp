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
#include "myschceduleview.h"
#include <QVBoxLayout>
#include "schceduledlg.h"
#include "scheduledatamanage.h"
#include <DMessageBox>
#include <DPushButton>
#include <DHiDPIHelper>
#include <QPainterPath>
#include <DPalette>
#include "schcedulectrldlg.h"
#include <QShortcut>
#include <DFontSizeManager>
#include <QPainter>
#include <QtMath>
DGUI_USE_NAMESPACE
CMySchceduleView::CMySchceduleView(const ScheduleDtailInfo &schduleInfo, QWidget *parent)
    : DDialog(parent)
//      m_scheduleInfo(ScheduleDtailInfo)
{
    setContentsMargins(0, 0, 0, 0);
    m_scheduleInfo = schduleInfo;
    initUI();
    initConnection();
    //setTitle(tr("My Schcedule"));
    setFixedSize(403, 160);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();
    if (themetype == 2) {
        DPalette anipa = palette();
        QColor color = "#191919";
        color.setAlphaF(0.8);
        anipa.setColor(DPalette::Background, color);
        setPalette(anipa);
    }

//    AutoFeed(m_scheduleInfo.titleName);
    //m_schceduleLabel->setText(info.titleName);
    if (m_scheduleInfo.type.ID == 4) {
        m_timeLabel->setText(m_scheduleInfo.beginDateTime.toString(("yyyy-MM-dd")));
    } else {
        m_timeLabel->setText(m_scheduleInfo.beginDateTime.toString("yyyy-MM-dd hh:mm") + " ~ " + m_scheduleInfo.endDateTime.toString("yyyy-MM-dd hh:mm"));
    }

    //setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34, 34) * devicePixelRatioF()));
}

CMySchceduleView::~CMySchceduleView()
{
    emit signalViewtransparentFrame(0);
}
void CMySchceduleView::AutoFeed(QString text)
{
    QString strText = text;
    QFont labelF;
    labelF.setFamily("SourceHanSansSC");
    labelF.setWeight(QFont::Medium);
    labelF.setPixelSize(14);
    QFontMetrics fm(labelF);
    int row = 0;

    QString str;
    if (!strText.isEmpty()) {
        QStringList sslist = strText.split("\n");
        QString result;
        for (int j = 0; j < sslist.count(); j++) {
            QString currentstr = sslist.at(j);
            for (int i = 0; i < currentstr.count(); i++) {
                str.append(currentstr.at(i));
                int widthT = fm.width(str) + 5;
                if (widthT >= 340) {
                    currentstr.insert(i - 1, "\n");
                    i--;
                    str.clear();
                    row++;
                } else {

                }
            }
            if (currentstr.isEmpty()) {
                result += "\n";
                row++;
            } else {
                if (j != sslist.count() - 1) {
                    result += currentstr + "\n";
                    row++;
                } else {
                    result += currentstr;
                }

            }
        }
        strText = result;
    }
//    m_schceduleLabel->setFixedHeight((row + 1) * 24);
    if (((row + 1) * 24) > 100) {
        area->setFixedHeight(100 - 20);
    } else {
        area->setFixedHeight((row + 1) * 24);
    }

    if ((row * 24 + 180) > 240) {
        setFixedHeight(240);
    } else {
        setFixedHeight(row * 24 + 180);
    }
//    m_schceduleLabel->setText(strText);
    m_schceduleLabel->setText(text);
    m_schceduleLabel->adjustSize();
}

/**
 * @brief CMySchceduleView::moveCentorShow      在顶层窗口居中显示
 */
void CMySchceduleView::moveCentorShow()
{
    //需要获取的顶层窗口
    QWidget *_parentWidget = this;
    //
    QWidget *tmpWidget = nullptr;
    do {
        //获取父类对象，如果为qwiget则赋值否则退出
        tmpWidget = qobject_cast<QWidget *>(_parentWidget->parent());
        if (tmpWidget == nullptr) {
            break;
        } else {
            _parentWidget = tmpWidget;
        }
    } while (_parentWidget != nullptr);
    //获取最顶层窗口的中心坐标
    const QPoint global = _parentWidget->mapToGlobal(_parentWidget->rect().center());
    //居中显示
    move(global.x() - width() / 2, global.y() - height() / 2);
}

void CMySchceduleView::showEvent(QShowEvent *event)
{
    DDialog::showEvent(event);
    emit signalViewtransparentFrame(1);
}

void CMySchceduleView::paintEvent(QPaintEvent *event)
{

}

bool CMySchceduleView::eventFilter(QObject *o, QEvent *e)
{
    QWidget *cell = qobject_cast<QWidget *>(o);
    if (cell == w && e->type() == QEvent::Paint) {
        paintLabel(cell);
    }
    return false;
}

void CMySchceduleView::paintLabel(QWidget *label)
{
    QPainter painter(label);
    QColor color;
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();
    painter.save();
    if (themetype == 0 || themetype == 1) {
        color = QColor("#2C4767");

    } else {
        color = QColor("#A8B7D1");
    }
    painter.setPen(color);

    QString strText = m_scheduleInfo.titleName;
    QFontMetrics fm = painter.fontMetrics();
    int titlewidth = fm.width(strText);
    QStringList strList;
    QString str;
    int h = fm.height();
    strList.clear();
    if (titlewidth < 340) {
        strList.append(strText);
        label->setFixedHeight(20);
    } else {
        for (int i = 0; i < strText.count(); i++) {
            str += strText.at(i);
            if (fm.width(str) > 340) {
                str.remove(str.count() - 1, 1);
                strList.append(str);
                str.clear();
                --i;
            }
        }
        strList.append(str);
        label->setFixedHeight(strList.count() * h + 8);
    }


    if (strList.count() * h > 100) {
        area->setFixedHeight(100 - 18);
    } else {
        area->setFixedHeight(strList.count() * h);
    }
    if (strList.count() * h + 180 > 240) {
        setFixedHeight(240);
    } else {
        setFixedHeight(strList.count() + 180);
    }

    for (int i = 0; i < strList.count(); i++) {
        painter.drawText(QRect(0, h * i, 340, h + 4), Qt::AlignHCenter, strList.at(i));
    }

    painter.restore();
    painter.end();
}

void CMySchceduleView::slotEditBt()
{
    CSchceduleDlg dlg(0, this);
    dlg.setData(m_scheduleInfo);
    if (dlg.exec() == DDialog::Accepted) {
        accept();
        emit signalsEditorDelete(1);
    }
}

void CMySchceduleView::slotDeleteBt()
{
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    if (m_scheduleInfo.rpeat == 0) {
        CSchceduleCtrlDlg msgBox;
        //msgBox.setWindowFlags(Qt::FramelessWindowHint);
        // msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34, 34) * devicePixelRatioF()));
        msgBox.setText(tr("You are deleting an event."));
        msgBox.setInformativeText(tr("Are you sure you want to delete this event?"));

        msgBox.addPushButton(tr("Cancel"), true);
        msgBox.addWaringButton(tr("Delete"), true);
        msgBox.exec();
        //是否删除
        if (msgBox.clickButton() == 0) {
            //取消
            return;
        } else if (msgBox.clickButton() == 1) {
            //删除
            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(m_scheduleInfo.id);
        } else {
            return;
        }
    } else {
        if (m_scheduleInfo.RecurID == 0) {
            CSchceduleCtrlDlg msgBox;
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
                //取消
                return;
            } else if (msgBox.clickButton() == 1) {
                //删除所有
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->deleteScheduleInfoById(m_scheduleInfo.id);
            } else if (msgBox.clickButton() == 2) {
                //仅删除这个日程
                ScheduleDtailInfo newschedule;
                //根据id查找点击日程
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_scheduleInfo.id, newschedule);
                //忽略需要删除的日程
                newschedule.ignore.append(m_scheduleInfo.beginDateTime);
                //更新日程信息
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            } else {
                return;
            }
        } else {
            CSchceduleCtrlDlg msgBox;
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
                //取消
                return;
            } else if (msgBox.clickButton() == 1) {
                //删除未来所有
                ScheduleDtailInfo newschedule;
                //根据id查找点击的日程
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_scheduleInfo.id, newschedule);
                //设置日程结束重复于的类型
                newschedule.enddata.type = 2;
                //设置重复日程结束日期
                newschedule.enddata.date = m_scheduleInfo.beginDateTime.addDays(-1);
                //更新日程信息
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            } else if (msgBox.clickButton() == 2) {
                //仅删除这个日程
                ScheduleDtailInfo newschedule;
                //根据id查找点击的日程
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->getScheduleInfoById(m_scheduleInfo.id, newschedule);
                //忽略这个日程
                newschedule.ignore.append(m_scheduleInfo.beginDateTime);
                //更新日程信息
                CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(newschedule);
            } else {
                return;
            }
        }
    }
    accept();
    emit signalsEditorDelete(1);
}

/**
 * @brief CMySchceduleView::exec        显示对话框
 * @return
 */
int CMySchceduleView::exec()
{
    //移动窗口
    moveCentorShow();
    return DDialog::exec();
}

/**
 * @brief CMyScheduleView::slotBtClick      按钮点击事件
 * @param buttonIndex
 * @param buttonName
 */
void CMySchceduleView::slotBtClick(int buttonIndex, QString buttonName)
{
    Q_UNUSED(buttonName);
    if (buttonIndex == 0) {
        //删除日程
        slotDeleteBt();
        return;
    }
    if (buttonIndex == 1) {
        //编辑日程
        slotEditBt();
        return;
    }
}

void CMySchceduleView::initUI()
{
    //m_icon = new QLabel(this);
    //  m_icon->setFixedSize(30, 30);
    //m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg")
    //         .scaled(m_icon->size() * devicePixelRatioF()));
    //m_icon->move(11, 10);
    m_Title = new QLabel(this);
    m_Title->setFixedSize(108, 51);
    m_Title->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    DFontSizeManager::instance()->bind(m_Title, DFontSizeManager::T5);
    QIcon t_icon = QIcon::fromTheme("dde-calendar");
    setIcon(t_icon);
    QFont labelTitle;
    labelTitle.setFamily("SourceHanSansSC");
    labelTitle.setWeight(QFont::DemiBold);
//    labelTitle.setPixelSize(17);
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    DPalette titlepa = m_Title->palette();
    if (themetype == 0 || themetype == 1) {
        titlepa.setColor(DPalette::WindowText, QColor("#001A2E"));

    } else {
        titlepa.setColor(DPalette::WindowText, QColor("#C0C6D4"));
    }

    m_Title->setPalette(titlepa);
    m_Title->setFont(labelTitle);
    m_Title->setText(tr("My Event"));
    m_Title->move(148, 0);
    m_Title->setAlignment(Qt::AlignCenter);



    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    // mainLayout->setContentsMargins(10, 10, 10, 10);

    area = new QScrollArea(this);
    area->setFrameShape(QFrame::NoFrame);
    area->setFixedWidth(390);
    DPalette pa = area->palette();
    if (themetype == 0 || themetype == 1) {
        pa.setColor(DPalette::WindowText, QColor("#2C4767"));

    } else {
        pa.setColor(DPalette::WindowText, QColor("#A8B7D1"));
    }
    area->setBackgroundRole(QPalette::Background);
    area->setPalette(pa);
    area->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    area->setWidgetResizable(true);
    area->setAlignment(Qt::AlignCenter);

    w = new QWidget(this);
    w->setFixedSize(340, 24);
    w->installEventFilter(this);

    area->setWidget(w);
    mainLayout->addWidget(area);
    m_timeLabel = new QLabel(this);
    m_timeLabel->setFixedHeight(26);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_timeLabel, DFontSizeManager::T6);
    QFont labelT;
    labelT.setFamily("SourceHanSansSC");
    labelT.setWeight(QFont::Bold);
//    labelT.setPixelSize(14);
    DPalette tpa = m_timeLabel->palette();


    if (themetype == 0 || themetype == 1) {
        tpa.setColor(DPalette::WindowText, QColor("#6A829F"));
    } else {
        tpa.setColor(DPalette::WindowText, QColor("#6A829F"));
    }



    m_timeLabel->setPalette(tpa);
    m_timeLabel->setFont(labelT);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(m_timeLabel);

    if (m_scheduleInfo.type.ID == 4) {
        addButton(tr("OK"), false, DDialog::ButtonNormal);
        QAbstractButton *button_ok = getButton(0);
        button_ok->setFixedSize(360, 36);
    } else {
        addButton(tr("Delete"), false, DDialog::ButtonNormal);
        addButton(tr("Edit"), false, DDialog::ButtonRecommend);
        for (int i = 0; i < buttonCount(); i++) {
            QAbstractButton *button = getButton(i);
            button->setFixedSize(165, 36);
        }
    }
    DFrame *gwi = new DFrame(this);
    gwi->setContentsMargins(0, 0, 0, 0);
    gwi->setLayout(mainLayout);
    DPalette anipa = gwi->palette();
    QColor color = "#F8F8F8";
    color.setAlphaF(0.0);
    anipa.setColor(DPalette::Background, color);
    gwi->setAutoFillBackground(true);
    gwi->setPalette(anipa);
    gwi->setBackgroundRole(DPalette::Background);
    // gwi->setGeometry(0, 51, 380, 110);
    addContent(gwi, Qt::AlignCenter);
}

void CMySchceduleView::initConnection()
{
    if (m_scheduleInfo.type.ID == 4) {
        connect(this, &DDialog::buttonClicked, this, &CMySchceduleView::close);
    } else {
        connect(this, &DDialog::buttonClicked, this, &CMySchceduleView::slotBtClick);
    }

    QShortcut *shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(QLatin1String("ESC")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));
}
