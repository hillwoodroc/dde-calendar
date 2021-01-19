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
#include "schceduledlg.h"
#include <DHiDPIHelper>
#include <DMessageBox>
#include <DPalette>
#include <QCalendarWidget>
#include <QHBoxLayout>
#include <QIcon>
#include <QIntValidator>
#include <QMessageBox>
#include <QShortcut>
#include <QTextBlock>
#include <QToolButton>
#include <QVBoxLayout>
#include "schcedulectrldlg.h"
#include "scheduledatamanage.h"
#include "timeedit.h"
#include "timeeditctrl.h"
DGUI_USE_NAMESPACE
CSchceduleDlg::CSchceduleDlg(int type, QWidget *parent, const bool isAllDay)
    : DDialog(parent), m_createAllDay(isAllDay)
{
    setContentsMargins(0, 0, 0, 0);
    m_type = type;
    initUI();
//    if (m_type == 0 && isAllDay) {
//        m_allDayCheckbox->setChecked(true);
//        m_beginTimeEdit->setVisible(false);
//        m_endTimeEdit->setVisible(false);
//    }
    initConnection();
    if (type == 1) {
        m_titleLabel->setText(tr("New Event"));
        m_beginDateEdit->setDate(QDate::currentDate());
        int hours = QTime::currentTime().hour();
        int minnutes = QTime::currentTime().minute() % 15;
        if (minnutes != 0) {
            minnutes = QTime::currentTime().minute() / 15 * 15 + 15;
        }
        m_beginTimeEdit->setTime(QTime(hours, minnutes));
        m_endDateEdit->setDate(QDate::currentDate());
        m_endTimeEdit->setTime(QTime(hours, minnutes).addSecs(3600));
        // m_endRepeatDate->setMinimumDate(QDate::currentDate());
    } else {
        m_titleLabel->setText(tr("Edit Event"));
    }
    setFocusPolicy(Qt::WheelFocus);
    setFixedSize(438, 460);
}

CSchceduleDlg::~CSchceduleDlg()
{
    emit signalViewtransparentFrame(0);
}

void CSchceduleDlg::setData(const ScheduleDtailInfo &info)
{
    m_scheduleDtailInfo = info;
    m_typeComBox->setCurrentIndex(info.type.ID - 1);
    m_textEdit->setText(info.titleName);
    m_beginDateEdit->setDate(info.beginDateTime.date());
    m_beginTimeEdit->setTime(info.beginDateTime.time());
    m_endDateEdit->setDate(info.endDateTime.date());
    m_endTimeEdit->setTime(info.endDateTime.time());
    m_allDayCheckbox->setChecked(info.allday);
    m_endRepeatDate->setMinimumDate(info.beginDateTime.date());
//    if (m_type == 0 && m_createAllDay) {

//    } else {
    slotallDayStateChanged(info.allday);
//    }

    initRmindRpeatUI();
    // m_textEdit->setTextCursor()
}

void CSchceduleDlg::setDate(const QDateTime &date)
{
    m_currentDate = date;
    int hours = date.time().hour();
    int minnutes = date.time().minute() % 15;
    if (minnutes != 0) {
        minnutes = date.time().minute() / 15 * 15;
    }
    if (minnutes == 60) {
        if (hours + 1 == 24) {
            m_currentDate.setTime(QTime(0, 0));
            m_currentDate = m_currentDate.addDays(1);
        } else {
            m_currentDate.setTime(QTime(hours + 1, 0));
        }
    } else {
        m_currentDate.setTime(QTime(hours, minnutes));
    }

    m_beginDateEdit->setDate(m_currentDate.date());
    m_beginTimeEdit->setTime(m_currentDate.time());
    QDateTime datetime = m_currentDate.addSecs(3600);
    m_endDateEdit->setDate(datetime.date());
    m_endTimeEdit->setTime(datetime.time());
    m_endRepeatDate->setMinimumDate(date.date());
}

ScheduleDtailInfo CSchceduleDlg::getScheduleData()
{
    return m_scheduleDtailInfo;
}

void CSchceduleDlg::setAllDay(bool flag)
{
    m_allDayCheckbox->setChecked(flag);
}

void CSchceduleDlg::slotCancelBt()
{
    reject();
}

void CSchceduleDlg::slotOkBt()
{
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    ScheduleDtailInfo scheduleDtailInfo = m_scheduleDtailInfo;
    QDateTime beginDateTime, endDateTime;
    beginDateTime.setDate(m_beginDateEdit->date());
    beginDateTime.setTime(m_beginTimeEdit->getTime());
    endDateTime.setDate(m_endDateEdit->date());
    endDateTime.setTime(m_endTimeEdit->getTime());
    if (m_textEdit->toPlainText().isEmpty()) {
        scheduleDtailInfo.titleName = m_textEdit->placeholderText();
    } else {
        scheduleDtailInfo.titleName = m_textEdit->toPlainText();
    }

    if (scheduleDtailInfo.titleName.isEmpty()) {
        // QMessageBox::warning(this, tr("error"), tr("The event is empty!"));
        return;
    }
    if (beginDateTime > endDateTime) {
        //        QMessageBox::information(this, ("    "), tr("End time must be greater than start
        //        time"));
        DDialog *prompt = new DDialog(this);
        prompt->setIcon(QIcon(":/resources/icon/warning.svg"));
        prompt->setMessage(tr("End time must be greater than start time"));
        prompt->setWindowFlags(prompt->windowFlags() | Qt::WindowStaysOnTopHint);
        prompt->addButton(tr("OK"), true, DDialog::ButtonNormal);
        prompt->show();
        return;
    }
    if (m_type == 1)
        scheduleDtailInfo.id = 0;
    scheduleDtailInfo.allday = m_allDayCheckbox->isChecked();
    if (m_rmindCombox->currentIndex() == 0)
        scheduleDtailInfo.remind = false;
    else {
        scheduleDtailInfo.remind = true;
    }
    if (scheduleDtailInfo.allday) {
        if (scheduleDtailInfo.remind) {
            scheduleDtailInfo.remindData.time = QTime(9, 0);
            if (m_rmindCombox->currentIndex() == 1) {
                scheduleDtailInfo.remindData.n = 0;
            } else if (m_rmindCombox->currentIndex() == 2) {
                scheduleDtailInfo.remindData.n = 1;
            } else if (m_rmindCombox->currentIndex() == 3) {
                scheduleDtailInfo.remindData.n = 2;
            } else if (m_rmindCombox->currentIndex() == 4) {
                scheduleDtailInfo.remindData.n = 7;
            }
        }
    } else {
        if (scheduleDtailInfo.remind) {
            if (m_rmindCombox->currentIndex() == 1) {
                scheduleDtailInfo.remindData.n = 0;
            } else if (m_rmindCombox->currentIndex() == 2) {
                scheduleDtailInfo.remindData.n = 15;
            } else if (m_rmindCombox->currentIndex() == 3) {
                scheduleDtailInfo.remindData.n = 30;
            } else if (m_rmindCombox->currentIndex() == 4) {
                scheduleDtailInfo.remindData.n = 60;
            } else if (m_rmindCombox->currentIndex() == 5) {
                scheduleDtailInfo.remindData.n = 1440;
            } else if (m_rmindCombox->currentIndex() == 6) {
                scheduleDtailInfo.remindData.n = 2880;
            } else if (m_rmindCombox->currentIndex() == 7) {
                scheduleDtailInfo.remindData.n = 10080;
            }
        }
    }
    scheduleDtailInfo.rpeat = m_beginrepeatCombox->currentIndex();
    if (scheduleDtailInfo.rpeat != 0) {
        scheduleDtailInfo.enddata.type = m_endrepeatCombox->currentIndex();
        if (m_endrepeatCombox->currentIndex() == 1) {
            if (m_endrepeattimes->text().isEmpty()) {
                // QMessageBox::warning(this, tr("error"), tr("The end repeat times is null!"));
                return;
            }
            scheduleDtailInfo.enddata.tcount = m_endrepeattimes->text().toInt();
        } else if (m_endrepeatCombox->currentIndex() == 2) {
            QDateTime endrpeattime = beginDateTime;
            endrpeattime.setDate(m_endRepeatDate->date());
            if (beginDateTime > endrpeattime) {
                // QMessageBox::warning(this, tr("error"), tr("The end repeat time less than begin
                // time!"));
                return;
            }
            scheduleDtailInfo.enddata.date = endrpeattime;
        }
    }

    CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->GetType(
        m_typeComBox->currentIndex() + 1, scheduleDtailInfo.type);
    scheduleDtailInfo.beginDateTime = beginDateTime;
    scheduleDtailInfo.endDateTime = endDateTime;

    if (m_type == 1) {
        CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->addSchedule(
            scheduleDtailInfo);
    } else if (m_type == 0) {
        if (m_scheduleDtailInfo.rpeat == 0 &&
                m_scheduleDtailInfo.rpeat == scheduleDtailInfo.rpeat) {
            CScheduleDataManage::getScheduleDataManage()->getscheduleDataCtrl()->updateScheduleInfo(
                scheduleDtailInfo);

        } else {
            if (m_scheduleDtailInfo.allday != scheduleDtailInfo.allday) {
                CSchceduleCtrlDlg msgBox(this);
                // msgBox.setWindowFlags(Qt::FramelessWindowHint);
                // msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34,
                // 34) * devicePixelRatioF()));
                msgBox.setText(
                    tr("All occurrences of a repeating event must have the same all-day status."));
                //                if (scheduleDtailInfo.allday) {
                //                    msgBox.setText(tr("All occurrences of a repeating event must
                //                    have the same all-day status."));

                //                } else {
                //                    msgBox.setText(tr("Repetitive event of all must have the same
                //                    not throughout the state."));
                //                }
                msgBox.setInformativeText(tr("Do you want to change all occurrences?"));
                //设置按钮文字
                msgBox.addPushButton(tr("Cancel"), true);
                msgBox.addWaringButton(tr("Change All"), true);
                msgBox.exec();
                //按钮对应的功能
                if (msgBox.clickButton() == 0) {
                    //取消
                    return;
                } else if (msgBox.clickButton() == 1) {
                    //修改所有
                    ScheduleDtailInfo updatescheduleData;
                    CScheduleDataManage::getScheduleDataManage()
                    ->getscheduleDataCtrl()
                    ->getScheduleInfoById(scheduleDtailInfo.id, updatescheduleData);
                    scheduleDtailInfo.beginDateTime = updatescheduleData.beginDateTime;
                    scheduleDtailInfo.endDateTime = updatescheduleData.endDateTime;
                    scheduleDtailInfo.RecurID = updatescheduleData.RecurID;
                    CScheduleDataManage::getScheduleDataManage()
                    ->getscheduleDataCtrl()
                    ->updateScheduleInfo(scheduleDtailInfo);
                }

            } else if (m_scheduleDtailInfo.rpeat != scheduleDtailInfo.rpeat) {
                CSchceduleCtrlDlg msgBox(this);
                // msgBox.setWindowFlags(Qt::FramelessWindowHint);
                // msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34,
                // 34) * devicePixelRatioF()));
                msgBox.setText(tr("You are changing the repeating rule of this event."));
                msgBox.setInformativeText(tr("Do you want to change all occurrences?"));
                //设置按钮文字
                msgBox.addPushButton(tr("Cancel"), true);
                msgBox.addWaringButton(tr("Change All"), true);
                msgBox.exec();
                //按钮对应的功能
                if (msgBox.clickButton() == 0) {
                    //取消
                    return;
                } else if (msgBox.clickButton() == 1) {
                    //修改所有
                    CScheduleDataManage::getScheduleDataManage()
                    ->getscheduleDataCtrl()
                    ->updateScheduleInfo(scheduleDtailInfo);
                }
            } else {
                if (m_scheduleDtailInfo.RecurID == 0) {
                    CSchceduleCtrlDlg msgBox(this);
                    // msgBox.setWindowFlags(Qt::FramelessWindowHint);
                    // msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34,
                    // 34) * devicePixelRatioF()));
                    msgBox.setText(tr("You are changing a repeating event."));
                    msgBox.setInformativeText(
                        tr("Do you want to change only this occurrence of the event, or all "
                           "occurrences?"));
                    //添加按钮设置文字
                    msgBox.addPushButton(tr("Cancel"));
                    msgBox.addPushButton(tr("All"));
                    msgBox.addsuggestButton(tr("Only This Event"));
                    msgBox.exec();
                    //按钮对应功能

                    if (msgBox.clickButton() == 0) {
                        return;
                    } else if (msgBox.clickButton() == 1) {
                        scheduleDtailInfo.ignore.clear();
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->updateScheduleInfo(scheduleDtailInfo);
                    } else if (msgBox.clickButton() == 2) {
                        ScheduleDtailInfo newschedule = scheduleDtailInfo;
                        newschedule.rpeat = 0;
                        newschedule.RecurID = 0;
                        newschedule.id = 0;
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->addSchedule(newschedule);
                        ScheduleDtailInfo updatescheduleData;
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->getScheduleInfoById(m_scheduleDtailInfo.id, updatescheduleData);
                        updatescheduleData.ignore.append(m_scheduleDtailInfo.beginDateTime);

                        // m_scheduleDtailInfo.ignore.append(m_scheduleDtailInfo.beginDateTime);
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->updateScheduleInfo(updatescheduleData);
                    }
                } else {
                    CSchceduleCtrlDlg msgBox(this);
                    // msgBox.setWindowFlags(Qt::FramelessWindowHint);
                    // msgBox.setIconPixmap(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-logo.svg").scaled(QSize(34,
                    // 34) * devicePixelRatioF()));

                    msgBox.setText(tr("You are changing a repeating event."));
                    msgBox.setInformativeText(
                        tr("Do you want to change only this occurrence of the event, or this and "
                           "all future occurrences?"));
                    //设置按钮文字
                    msgBox.addPushButton(tr("Cancel"));
                    msgBox.addPushButton(tr("All Future Events"));
                    msgBox.addsuggestButton(tr("Only This Event"));
                    msgBox.exec();
                    //按钮对应功能
                    if (msgBox.clickButton() == 0) {
                        return;
                    } else if (msgBox.clickButton() == 1) {
                        ScheduleDtailInfo newschedule = scheduleDtailInfo;
                        newschedule.RecurID = 0;
                        newschedule.id = 0;
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->addSchedule(newschedule);
                        ScheduleDtailInfo updatescheduleData;
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->getScheduleInfoById(m_scheduleDtailInfo.id, updatescheduleData);
                        updatescheduleData.enddata.type = 2;
                        updatescheduleData.enddata.date =
                            m_scheduleDtailInfo.beginDateTime.addDays(-1);
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->updateScheduleInfo(updatescheduleData);
                    } else if (msgBox.clickButton() == 2) {
                        ScheduleDtailInfo newschedule = scheduleDtailInfo;
                        newschedule.rpeat = 0;
                        newschedule.RecurID = 0;
                        newschedule.id = 0;
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->addSchedule(newschedule);
                        ScheduleDtailInfo updatescheduleData;
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->getScheduleInfoById(m_scheduleDtailInfo.id, updatescheduleData);
                        updatescheduleData.ignore.append(m_scheduleDtailInfo.beginDateTime);
                        CScheduleDataManage::getScheduleDataManage()
                        ->getscheduleDataCtrl()
                        ->updateScheduleInfo(updatescheduleData);
                    }
                }
            }
        }
    }
    accept();
    emit  signalScheduleUpdate();
}

void CSchceduleDlg::slotTextChange()
{
    QTextCursor cursor = m_textEdit->textCursor();
    QString textContent = m_textEdit->toPlainText();
    int length = textContent.count();
    QString tStitlename = textContent;
    if (tStitlename.contains("\n")) {
        tStitlename.replace("\n", "");
        m_textEdit->setText(tStitlename);
        cursor.movePosition(QTextCursor::End);
        m_textEdit->setTextCursor(cursor);
        return;
    }
    int maxLength = 256;  // 最大字符数
    if (length > maxLength) {
        // QMessageBox::information(this, tr("infomation"), tr("Max length is 256!"));
        m_textEdit->setText(m_context);
        cursor.movePosition(QTextCursor::End);
        m_textEdit->setTextCursor(cursor);
        /*textContent = textContent.mid(0, 256);
        m_textEdit->setText(textContent);
        cursor.movePosition(QTextCursor::End);
        //if (cursor.hasSelection()) {
        //  cursor.clearSelection();
        //}
        //cursor.deletePreviousChar();
        //设置当前的光标为更改后的光标
        m_textEdit->setTextCursor(cursor);*/
    }
    m_context = m_textEdit->toPlainText();
}

void CSchceduleDlg::slotBDateEidtInfo(const QDate &date)
{
    m_beginDateEdit->setDate(date);
    m_endRepeatDate->setMinimumDate(date);
    m_endDateEdit->setMinimumDate(date);

    QDateTime beginDateTime, endDateTime;
    beginDateTime.setDate(m_beginDateEdit->date());
    beginDateTime.setTime(m_beginTimeEdit->getTime());
    endDateTime.setDate(m_endDateEdit->date());
    endDateTime.setTime(m_endTimeEdit->getTime());

    if (endDateTime < beginDateTime) {
        m_endTimeEdit->setTime(m_beginTimeEdit->getTime().addSecs(3600));
    }
}

void CSchceduleDlg::slotEDateEidtInfo(const QDate &date)
{
    m_endDateEdit->setDate(date);
}

void CSchceduleDlg::slotallDayStateChanged(int state)
{
    m_rmindCombox->clear();
    if (!state) {
        m_rmindCombox->addItem(tr("Never"));
        m_rmindCombox->addItem(tr("At time of event"));
        m_rmindCombox->addItem(tr("15 minutes before"));
        m_rmindCombox->addItem(tr("30 minutes before"));
        m_rmindCombox->addItem(tr("1 hour before"));
        m_rmindCombox->addItem(tr("1 day before"));
        m_rmindCombox->addItem(tr("2 days before"));
        m_rmindCombox->addItem(tr("1 week before"));
        m_beginTimeEdit->setVisible(true);
        m_endTimeEdit->setVisible(true);
        if (m_type == 0) {
            m_beginDateEdit->setDate(m_scheduleDtailInfo.beginDateTime.date());
            m_beginTimeEdit->setTime(m_scheduleDtailInfo.beginDateTime.time());
            m_endDateEdit->setDate(m_scheduleDtailInfo.endDateTime.date());
            m_endTimeEdit->setTime(m_scheduleDtailInfo.endDateTime.time());
        } else {
            m_beginDateEdit->setDate(m_currentDate.date());
            m_beginTimeEdit->setTime(m_currentDate.time());
            m_endDateEdit->setDate(m_currentDate.date());
            m_endTimeEdit->setTime(m_currentDate.time().addSecs(3600));
        }
    } else {
        m_rmindCombox->addItem(tr("Never"));
        m_rmindCombox->addItem(tr("On start day (9:00 AM)"));
        m_rmindCombox->addItem(tr("1 day before"));
        m_rmindCombox->addItem(tr("2 days before"));
        m_rmindCombox->addItem(tr("1 week before"));
        m_beginTimeEdit->setVisible(false);
        m_endTimeEdit->setVisible(false);
        if (m_type == 0) {
            m_beginDateEdit->setDate(m_scheduleDtailInfo.beginDateTime.date());
            m_beginTimeEdit->setTime(QTime(0, 0));
            m_endDateEdit->setDate(m_scheduleDtailInfo.endDateTime.date());
            m_endTimeEdit->setTime(QTime(23, 59));
        } else {
            m_beginDateEdit->setDate(m_currentDate.date());
            m_endDateEdit->setDate(m_currentDate.date());
            m_beginTimeEdit->setTime(QTime(0, 0));
            m_endTimeEdit->setTime(QTime(23, 59));
            m_rmindCombox->setCurrentIndex(2);
        }
    }

    // m_rmindCombox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void CSchceduleDlg::slotbRpeatactivated(int index)
{
    if (index > 0) {
        m_endrepeatWidget->setVisible(true);
        setFixedSize(438, 506);
        // m_gwi->setGeometry(0, 68, 438, 458);
    } else {
        m_endrepeatWidget->setVisible(false);
        setFixedSize(438, 460);
        // m_gwi->setGeometry(0, 68, 438, 412);
    }
}

void CSchceduleDlg::sloteRpeatactivated(int index)
{
    if (index == 0) {
        m_endrepeattimesWidget->setVisible(false);
        m_endRepeatDate->setVisible(false);
    } else if (index == 1) {
        m_endrepeattimesWidget->setVisible(true);
        m_endRepeatDate->setVisible(false);
    } else {
        m_endrepeattimesWidget->setVisible(false);
        m_endRepeatDate->setVisible(true);
    }
}

void CSchceduleDlg::slotBtClick(int buttonIndex, QString buttonName)
{
    Q_UNUSED(buttonName)
    switch (buttonIndex) {
    case 0: {
        //取消
        slotCancelBt();
        break;
    }
    case 1: {
        //确定
        slotOkBt();
        break;
    }
    default:
        break;
    }
}

bool CSchceduleDlg::eventFilter(QObject *obj, QEvent *pEvent)
{
    if (obj == m_textEdit) {
        if (pEvent->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(pEvent);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                return true;
            }
            if (keyEvent->key() == Qt::Key_Tab)
                return true;
            /*QString textContent = m_textEdit->toPlainText();
            int length = textContent.count();
            int maxLength = 255; // 最大字符数
            if (length > maxLength) {

                QMessageBox::information(this, tr("infomation"), tr("Max length is 256!"));
                return true;
            }*/
        }
        if (m_type == 1) {
            if (pEvent->type() == QEvent::FocusIn) {
                //清空编辑框默认占位符
                m_textEdit->setPlaceholderText("");
            } else if (pEvent->type() == QEvent::FocusOut) {
                //设置编辑框默认占位符
                m_textEdit->setPlaceholderText(tr("New event"));
            }
        }
    }
    return QDialog::eventFilter(obj, pEvent);
}

void CSchceduleDlg::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    emit signalViewtransparentFrame(1);
}

void CSchceduleDlg::initUI()
{
    m_titleLabel = new QLabel(this);
    QFont titlelabelF;
    titlelabelF.setFamily("SourceHanSansSC");
    titlelabelF.setWeight(QFont::DemiBold);
    titlelabelF.setPixelSize(17);
    QColor btitleColor = "#000000";
    btitleColor.setAlphaF(0.01);
    DPalette titlepa = m_titleLabel->palette();
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();
    if (themetype == 0 || themetype == 1) {
        titlepa.setColor(DPalette::WindowText, QColor("#001A2E"));
        titlepa.setColor(DPalette::Window, btitleColor);
    } else {
        titlepa.setColor(DPalette::WindowText, QColor("#C0C6D4"));
        titlepa.setColor(DPalette::Window, btitleColor);
    }
    m_titleLabel->setPalette(titlepa);
    m_titleLabel->setFixedSize(148, 51);
    m_titleLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    // m_titleLabel->setGeometry(165, 0, 108, 51);
    m_titleLabel->move(145, 0);
    m_titleLabel->setFont(titlelabelF);

    setSpacing(0);
    QFont mlabelF;
    mlabelF.setFamily("SourceHanSansSC");
    mlabelF.setWeight(QFont::Medium);
    mlabelF.setPixelSize(14);
    DPalette pa = m_titleLabel->palette();
    if (themetype == 0 || themetype == 1) {
        pa.setColor(DPalette::WindowText, QColor("#414D68"));
    } else {
        pa.setColor(DPalette::WindowText, QColor("#C0C6D4"));
    }
    // pa.setColor(DPalette::WindowText, QColor("#414D68"));

    QVBoxLayout *maintlayout = new QVBoxLayout;
    maintlayout->setMargin(0);
    maintlayout->setSpacing(10);
    QHBoxLayout *typelayout = new QHBoxLayout;
    typelayout->setSpacing(0);
    typelayout->setMargin(0);
    // maintlayout->setContentsMargins(20, 0, 20, 10);
    m_typeLabel = new QLabel(tr("Type:"));
    m_typeLabel->setFont(mlabelF);
    m_typeLabel->setPalette(pa);
    m_typeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_typeLabel->setFixedSize(78, 36);
    m_typeComBox = new DComboBox();
    m_typeComBox->setFixedSize(319, 36);
    m_typeComBox->setIconSize(QSize(24, 24));
    m_typeComBox->insertItem(0,
                             QIcon(DHiDPIHelper::loadNxPixmap(":/resources/icon/icon_type_work.svg")
                                   .scaled(QSize(24, 24) * devicePixelRatioF())),
                             tr("Work"));
    m_typeComBox->insertItem(1,
                             QIcon(DHiDPIHelper::loadNxPixmap(":/resources/icon/icon_type_life.svg")
                                   .scaled(QSize(24, 24) * devicePixelRatioF())),
                             tr("Life"));
    m_typeComBox->insertItem(
        2,
        QIcon(DHiDPIHelper::loadNxPixmap(":/resources/icon/icon_type_other.svg")
              .scaled(QSize(24, 24) * devicePixelRatioF())),
        tr("Other"));
    typelayout->addWidget(m_typeLabel);
    typelayout->addWidget(m_typeComBox);
    maintlayout->addLayout(typelayout);

    QHBoxLayout *contentLabellayout = new QHBoxLayout;
    contentLabellayout->setSpacing(0);
    contentLabellayout->setMargin(0);
    QVBoxLayout *conttelabellayout = new QVBoxLayout;
    conttelabellayout->setSpacing(0);
    conttelabellayout->setMargin(0);
    m_contentLabel = new QLabel();
    QFontMetrics fontWidth_contentlabel(mlabelF);
    QString str_contentlabel = fontWidth_contentlabel.elidedText(tr("Description:"), Qt::ElideRight, 68);
    m_contentLabel->setText(str_contentlabel);
    m_contentLabel->setFont(mlabelF);
    m_contentLabel->setPalette(pa);
    m_contentLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_contentLabel->setFixedWidth(78);
    conttelabellayout->addWidget(m_contentLabel);
    conttelabellayout->addStretch();
    m_textEdit = new DTextEdit(this);
    m_textEdit->setFixedSize(319, 86);
    m_textEdit->setAcceptRichText(false);

#if 0
    m_textEdit->setTextBackgroundColor(Qt::white);
    DPalette tpa = m_textEdit->palette();
    if (themetype == 0 || themetype == 1) {
        QColor conttelc("#000000");
        conttelc.setAlphaF(0.08);
        tpa.setColor(DPalette::Background, conttelc);
    } else {
        QColor conttelc("#FFFFFF");
        conttelc.setAlphaF(0.15);
        tpa.setColor(DPalette::Background, conttelc);
        QTextCharFormat fmt;
        conttelc.setAlphaF(0.0);

        QTextCursor textcursor = m_textEdit->textCursor();
        fmt = textcursor.charFormat();
        fmt.setBackground(conttelc);
        textcursor.setCharFormat(fmt);
        m_textEdit->setTextCursor(textcursor);
    }

    m_textEdit->setPalette(tpa);
#endif

    // if (m_type == 1) {
    m_textEdit->setPlaceholderText(tr("New Event"));
    //设置关联控件，用于QTextEdit控件捕获MouseButtonPress等事件
    QWidget *mpContentWidget = m_textEdit->viewport();
    //设置事件过滤器
    m_textEdit->installEventFilter(this);
    mpContentWidget->installEventFilter(this);
    //}

    contentLabellayout->addLayout(conttelabellayout);
    contentLabellayout->addWidget(m_textEdit);
    maintlayout->addLayout(contentLabellayout);

    QHBoxLayout *alldayLabellayout = new QHBoxLayout;
    alldayLabellayout->setSpacing(0);
    alldayLabellayout->setMargin(0);
    m_adllDayLabel = new QLabel(tr("All Day:"));
    m_adllDayLabel->setFont(mlabelF);
    m_adllDayLabel->setPalette(pa);
    m_adllDayLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_adllDayLabel->setFixedWidth(78);
    m_allDayCheckbox = new DCheckBox(this);
    alldayLabellayout->addWidget(m_adllDayLabel);
    alldayLabellayout->addWidget(m_allDayCheckbox);
    maintlayout->addLayout(alldayLabellayout);

    QHBoxLayout *beginLabellayout = new QHBoxLayout;
    beginLabellayout->setSpacing(0);
    beginLabellayout->setMargin(0);
    m_beginTimeLabel = new QLabel(tr("Starts:"));
    m_beginTimeLabel->setFont(mlabelF);
    m_beginTimeLabel->setPalette(pa);
    m_beginTimeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_beginTimeLabel->setFixedSize(78, 36);
    m_beginDateEdit = new QDateEdit(this);
    m_beginDateEdit->setFixedSize(200, 36);

    m_beginTimeEdit = new CTimeEdit(this);
    m_beginTimeEdit->setFixedSize(109, 36);
    m_beginDateEdit->setCalendarPopup(true);
    m_beginDateEdit->setDisplayFormat("yyyy-MM-dd");
    // QHBoxLayout *begintimeelayout  = new QHBoxLayout;
    // begintimeelayout->addWidget(m_beginDateEdit, 1);
    // begintimeelayout->addWidget(m_beginTimeEdit, 1);
    // begintimeelayout->setMargin(0);
    // begintimeelayout->setSpacing(0);
    // begintimeelayout->addStretch();
    beginLabellayout->addWidget(m_beginTimeLabel);
    beginLabellayout->addWidget(m_beginDateEdit);
    beginLabellayout->addSpacing(10);
    beginLabellayout->addWidget(m_beginTimeEdit);
    beginLabellayout->addStretch();
    // beginLabellayout->addLayout(begintimeelayout);
    maintlayout->addLayout(beginLabellayout);

    QHBoxLayout *enQLabellayout = new QHBoxLayout;
    enQLabellayout->setSpacing(0);
    enQLabellayout->setMargin(0);
    m_endTimeLabel = new QLabel(tr("Ends:"));
    m_endTimeLabel->setFont(mlabelF);
    m_endTimeLabel->setPalette(pa);
    m_endTimeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_endTimeLabel->setFixedSize(78, 36);
    m_endDateEdit = new QDateEdit(this);
    m_endDateEdit->setFixedSize(200, 36);
    m_endTimeEdit = new CTimeEdit(this);
    m_endTimeEdit->setFixedSize(109, 36);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("yyyy-MM-dd");

    enQLabellayout->addWidget(m_endTimeLabel);
    enQLabellayout->addWidget(m_endDateEdit);
    enQLabellayout->addSpacing(10);

    enQLabellayout->addWidget(m_endTimeEdit);
    enQLabellayout->addStretch();
    // enQLabellayout->addLayout(endtimeelayout);
    maintlayout->addLayout(enQLabellayout);

    QHBoxLayout *rminQLabellayout = new QHBoxLayout;
    rminQLabellayout->setSpacing(0);
    rminQLabellayout->setMargin(0);
    m_remindSetLabel = new QLabel();
    QFontMetrics fontWidth_remindSetLabel(mlabelF);
    QString str_remindSetLabel = fontWidth_remindSetLabel.elidedText(tr("Remind Me:"), Qt::ElideRight, 68);
    m_remindSetLabel->setText(str_remindSetLabel);
    m_remindSetLabel->setFont(mlabelF);
    m_remindSetLabel->setPalette(pa);
    m_remindSetLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_remindSetLabel->setFixedWidth(78);

    m_rmindCombox = new DComboBox();
    m_rmindCombox->setFixedSize(200, 36);
    m_rmindCombox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    rminQLabellayout->addWidget(m_remindSetLabel);
    rminQLabellayout->addWidget(m_rmindCombox);
    rminQLabellayout->addStretch();
    maintlayout->addLayout(rminQLabellayout);

    QHBoxLayout *repeatLabellayout = new QHBoxLayout;
    repeatLabellayout->setSpacing(0);
    repeatLabellayout->setMargin(0);
    m_beginrepeatLabel = new QLabel(tr("Repeat:"));
    m_beginrepeatLabel->setFont(mlabelF);
    m_beginrepeatLabel->setPalette(pa);
    m_beginrepeatLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_beginrepeatLabel->setFixedWidth(78);
    m_beginrepeatCombox = new DComboBox();
    m_beginrepeatCombox->setFixedSize(200, 36);
    m_beginrepeatCombox->addItem(tr("Never"));
    m_beginrepeatCombox->addItem(tr("Daily"));
    m_beginrepeatCombox->addItem(tr("Weekdays"));
    m_beginrepeatCombox->addItem(tr("Weekly"));
    m_beginrepeatCombox->addItem(tr("Monthly"));
    m_beginrepeatCombox->addItem(tr("Yearly"));
    repeatLabellayout->addWidget(m_beginrepeatLabel);
    repeatLabellayout->addWidget(m_beginrepeatCombox);
    repeatLabellayout->addStretch();
    maintlayout->addLayout(repeatLabellayout);

    QHBoxLayout *endrepeatLabellayout = new QHBoxLayout;
    endrepeatLabellayout->setSpacing(0);
    endrepeatLabellayout->setMargin(0);
    m_endrepeatLabel = new QLabel();
    QFontMetrics fontWidth_endrepeatLabel(mlabelF);
    QString str_endrepeatLabel = fontWidth_endrepeatLabel.elidedText(tr("End Repeat:"), Qt::ElideRight, 68);
    m_endrepeatLabel->setText(str_endrepeatLabel);
    m_endrepeatLabel->setFont(mlabelF);
    m_endrepeatLabel->setPalette(pa);
    m_endrepeatLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_endrepeatLabel->setFixedWidth(78);
    m_endrepeatCombox = new DComboBox();
    m_endrepeatCombox->setFixedSize(200, 36);
    m_endrepeatCombox->addItem(tr("Never"));
    m_endrepeatCombox->addItem(tr("After"));
    m_endrepeatCombox->addItem(tr("On"));
    endrepeatLabellayout->addWidget(m_endrepeatLabel);
    endrepeatLabellayout->addWidget(m_endrepeatCombox);

    QHBoxLayout *endrepeattimeslayout = new QHBoxLayout;
    endrepeattimeslayout->setSpacing(0);
    endrepeattimeslayout->setMargin(0);
    endrepeattimeslayout->setContentsMargins(10, 0, 0, 0);
    m_endrepeattimes = new DLineEdit(this);
    m_endrepeattimes->setFixedSize(71, 36);
    m_endrepeattimes->setText(QString::number(10));
    m_endrepeattimes->setClearButtonEnabled(false);
    QRegExp rx("^[1-9]\\d{0,2}$");
    QValidator *validator = new QRegExpValidator(rx, this);
    m_endrepeattimes->lineEdit()->setValidator(validator);
    m_endrepeattimesLabel = new QLabel(tr("time(s)"));
    m_endrepeattimesLabel->setFont(mlabelF);
    m_endrepeattimesLabel->setPalette(pa);
    m_endrepeattimesLabel->setFixedHeight(36);
    endrepeattimeslayout->addWidget(m_endrepeattimes);
    endrepeattimeslayout->addWidget(m_endrepeattimesLabel);
    m_endrepeattimesWidget = new DWidget;
    m_endrepeattimesWidget->setLayout(endrepeattimeslayout);
    m_endrepeattimesWidget->setVisible(false);
    endrepeatLabellayout->addWidget(m_endrepeattimesWidget);

    m_endRepeatDate = new DDateEdit;
    m_endRepeatDate->setCalendarPopup(true);
    m_endRepeatDate->setFixedSize(120, 36);
    m_endRepeatDate->setDate(QDate::currentDate());
    m_endRepeatDate->setDisplayFormat("yyyy-MM-dd");
    m_endRepeatDate->setCurrentSectionIndex(2);
    QFont enddatefont;
    enddatefont.setFamily("SourceHanSansSC");
    enddatefont.setWeight(QFont::Medium);
    enddatefont.setPixelSize(11);
    m_endRepeatDate->setFont(enddatefont);
    // m_endRepeatDate->setMinimumWidth(150);
    // m_endRepeatDate->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    endrepeatLabellayout->addWidget(m_endRepeatDate);
    endrepeatLabellayout->addStretch();
    m_endRepeatDate->setVisible(false);
    m_endrepeatWidget = new DWidget;
    m_endrepeatWidget->setLayout(endrepeatLabellayout);
    maintlayout->addWidget(m_endrepeatWidget);
    m_endrepeatWidget->setVisible(false);
    //添加按钮
    addButton(tr("Cancel"));
    addButton(tr("Save"), false, DDialog::ButtonRecommend);

    for (int i = 0; i < buttonCount(); i++) {
        QAbstractButton *button = getButton(i);
        button->setFixedSize(189, 36);
    }
    m_gwi = new DFrame(this);
    m_gwi->setFrameShape(QFrame::NoFrame);
    m_gwi->setLayout(maintlayout);
    addContent(m_gwi, Qt::AlignCenter);
    initDateEdit();

    if (m_type == 1)
        slotallDayStateChanged(0);
    setFocus();
}

void CSchceduleDlg::initConnection()
{
    connect(this, &DDialog::buttonClicked, this, &CSchceduleDlg::slotBtClick);
    connect(m_textEdit, &DTextEdit::textChanged, this, &CSchceduleDlg::slotTextChange);

    connect(m_allDayCheckbox, &DCheckBox::stateChanged, this,
            &CSchceduleDlg::slotallDayStateChanged);
    connect(m_beginrepeatCombox, QOverload<int>::of(&QComboBox::activated), this,
            &CSchceduleDlg::slotbRpeatactivated);
    connect(m_endrepeatCombox, QOverload<int>::of(&QComboBox::activated), this,
            &CSchceduleDlg::sloteRpeatactivated);
    connect(m_beginDateEdit, &DDateEdit::userDateChanged, this, &CSchceduleDlg::slotBDateEidtInfo);

    QShortcut *shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(QLatin1String("ESC")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(slotCancelBt()));
}

void CSchceduleDlg::initDateEdit()
{
    // m_bCustomDateW = new CCustomCalendarWidget(0);
    // m_beginDateEdit->setCalendarWidget(m_bCustomDateW);
    m_beginDateEdit->setMinimumDate(QDate(1900, 1, 1));    // 0天
    m_beginDateEdit->setMaximumDate(QDate(9999, 12, 31));  //
    // m_beginDateEdit->setContextMenuPolicy(Qt::NoContextMenu);
    // m_bCustomDateW->disconnect(SIGNAL(selectionChanged()));
    // m_bCustomDateW->disconnect(SIGNAL(clicked(QDate)));

    // m_eCustomDateW = new CCustomCalendarWidget(0);
    // m_endDateEdit->setCalendarWidget(m_eCustomDateW);
    m_endDateEdit->setMinimumDate(QDate(1900, 1, 1));    // 0天
    m_endDateEdit->setMaximumDate(QDate(9999, 12, 31));  //
    // m_endDateEdit->setContextMenuPolicy(Qt::NoContextMenu);
    // m_eCustomDateW->disconnect(SIGNAL(selectionChanged()));
    // m_eCustomDateW->disconnect(SIGNAL(clicked(QDate)));
    return;
    int themetype = CScheduleDataManage::getScheduleDataManage()->getTheme();

    QWidget *view1 =
        m_beginDateEdit->calendarWidget()->findChild<QWidget *>("qt_calendar_navigationbar");
    if (view1) {
        DPalette anipa = view1->palette();
        if (themetype == 2) {
            QColor cc = "#FFFFFF";
            cc.setAlphaF(0.1);
            anipa.setColor(DPalette::Background, cc);
        } else {
            anipa.setColor(DPalette::Background, "#FFFFFF");
        }
        view1->setPalette(anipa);
        view1->setBackgroundRole(DPalette::Background);
    }
    QToolButton *monthbutton1 =
        m_beginDateEdit->calendarWidget()->findChild<QToolButton *>("qt_calendar_monthbutton");
    if (monthbutton1) {
        DPalette anipa = monthbutton1->palette();
        if (themetype == 2) {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        } else {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        }
        monthbutton1->setPalette(anipa);
        // monthbutton1->setBackgroundRole(DPalette::ButtonText);
    }
    QToolButton *yearbutton1 =
        m_beginDateEdit->calendarWidget()->findChild<QToolButton *>("qt_calendar_yearbutton");
    if (yearbutton1) {
        DPalette anipa = yearbutton1->palette();
        if (themetype == 2) {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        } else {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        }
        yearbutton1->setPalette(anipa);
    }

    QWidget *view2 =
        m_endDateEdit->calendarWidget()->findChild<QWidget *>("qt_calendar_navigationbar");
    if (view2) {
        DPalette anipa = view2->palette();
        if (themetype == 2) {
            anipa.setColor(DPalette::Background, "#252525");
        } else {
            anipa.setColor(DPalette::Background, "#FFFFFF");
        }
        view2->setPalette(anipa);
        view2->setBackgroundRole(DPalette::Background);
    }
    QToolButton *monthbutton2 =
        m_beginDateEdit->calendarWidget()->findChild<QToolButton *>("qt_calendar_monthbutton");
    if (monthbutton2) {
        DPalette anipa = monthbutton2->palette();
        if (themetype == 2) {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        } else {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        }
        monthbutton2->setPalette(anipa);
    }
    QToolButton *yearbutton2 =
        m_beginDateEdit->calendarWidget()->findChild<QToolButton *>("qt_calendar_yearbutton");
    if (yearbutton2) {
        DPalette anipa = yearbutton2->palette();
        if (themetype == 2) {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        } else {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        }
        yearbutton2->setPalette(anipa);
    }

    QWidget *view3 =
        m_endRepeatDate->calendarWidget()->findChild<QWidget *>("qt_calendar_navigationbar");
    if (view3) {
        DPalette anipa = view3->palette();
        if (themetype == 2) {
            anipa.setColor(DPalette::Background, "#252525");
        } else {
            anipa.setColor(DPalette::Background, "#FFFFFF");
        }
        view3->setPalette(anipa);
        view3->setBackgroundRole(DPalette::Background);
    }

    QToolButton *monthbutton3 =
        m_beginDateEdit->calendarWidget()->findChild<QToolButton *>("qt_calendar_monthbutton");
    if (monthbutton3) {
        DPalette anipa = monthbutton3->palette();
        if (themetype == 2) {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        } else {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        }
        monthbutton3->setPalette(anipa);
    }
    QToolButton *yearbutton3 =
        m_beginDateEdit->calendarWidget()->findChild<QToolButton *>("qt_calendar_yearbutton");
    if (yearbutton2) {
        DPalette anipa = yearbutton3->palette();
        if (themetype == 2) {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        } else {
            anipa.setColor(QPalette::ButtonText, QPalette::Highlight);
        }
        yearbutton3->setPalette(anipa);
    }
}

void CSchceduleDlg::initRmindRpeatUI()
{
    if (m_scheduleDtailInfo.allday) {
        if (m_scheduleDtailInfo.remind) {
            if (m_scheduleDtailInfo.remindData.n == 0) {
                m_rmindCombox->setCurrentIndex(1);
            } else if (m_scheduleDtailInfo.remindData.n == 1) {
                m_rmindCombox->setCurrentIndex(2);
            } else if (m_scheduleDtailInfo.remindData.n == 2) {
                m_rmindCombox->setCurrentIndex(3);
            } else if (m_scheduleDtailInfo.remindData.n == 7) {
                m_rmindCombox->setCurrentIndex(4);
            }
        } else {
            m_rmindCombox->setCurrentIndex(0);
        }

    } else {
        if (m_scheduleDtailInfo.remind) {
            if (m_scheduleDtailInfo.remindData.n == 0) {
                m_rmindCombox->setCurrentIndex(1);
            } else if (m_scheduleDtailInfo.remindData.n == 15) {
                m_rmindCombox->setCurrentIndex(2);
            } else if (m_scheduleDtailInfo.remindData.n == 30) {
                m_rmindCombox->setCurrentIndex(3);
            } else if (m_scheduleDtailInfo.remindData.n == 60) {
                m_rmindCombox->setCurrentIndex(4);
            } else if (m_scheduleDtailInfo.remindData.n == 1440) {
                m_rmindCombox->setCurrentIndex(5);
            } else if (m_scheduleDtailInfo.remindData.n == 2880) {
                m_rmindCombox->setCurrentIndex(6);
            } else if (m_scheduleDtailInfo.remindData.n == 10080) {
                m_rmindCombox->setCurrentIndex(7);
            }
        } else {
            m_rmindCombox->setCurrentIndex(0);
        }
    }
    slotbRpeatactivated(m_scheduleDtailInfo.rpeat);
    m_beginrepeatCombox->setCurrentIndex(m_scheduleDtailInfo.rpeat);
    if (m_scheduleDtailInfo.rpeat != 0) {
        if (m_scheduleDtailInfo.enddata.type == 0) {
            m_endrepeatCombox->setCurrentIndex(0);
        } else if (m_scheduleDtailInfo.enddata.type == 1) {
            m_endrepeatCombox->setCurrentIndex(1);
            m_endrepeattimes->setText(QString::number(m_scheduleDtailInfo.enddata.tcount));
        } else if (m_scheduleDtailInfo.enddata.type == 2) {
            m_endrepeatCombox->setCurrentIndex(2);
            m_endRepeatDate->setDate(m_scheduleDtailInfo.enddata.date.date());
        }
        m_endrepeatWidget->show();
        sloteRpeatactivated(m_scheduleDtailInfo.enddata.type);
    } else {
        m_endrepeatWidget->hide();
    }
}
