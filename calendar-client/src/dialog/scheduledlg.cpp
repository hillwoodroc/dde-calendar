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
#include "scheduledlg.h"
#include "schedulectrldlg.h"
#include "scheduledatamanage.h"
#include "timeedit.h"
#include "constants.h"
#include "cscheduleoperation.h"
#include "tabletconfig.h"

#include <DHiDPIHelper>
#include <DFontSizeManager>
#include <DApplication>

#include <QDesktopWidget>
#include <QTimer>
#include <QHBoxLayout>
#include <QIcon>
#include <QShortcut>
#include <QVBoxLayout>
#include <QKeyEvent>

DGUI_USE_NAMESPACE

bool CScheduleDlg::DialogIsClose = true;

CScheduleDlg::CScheduleDlg(int type, QWidget *parent, const bool isAllDay)
    : DCalendarDDialog(parent)
    , m_createAllDay(isAllDay)
{
    setContentsMargins(0, 0, 0, 0);
    m_type = type;
    initUI();
    initConnection();
    setTabFouseOrder();

    if (type == 1) {
        m_titleLabel->setText(tr("New Event"));
        m_beginDateEdit->setDate(QDate::currentDate());
        int hours = QTime::currentTime().hour();
        int minnutes = QTime::currentTime().minute() % DDECalendar::QuarterOfAnhourWithMinute;

        if (minnutes != 0) {
            minnutes = QTime::currentTime().minute() / DDECalendar::QuarterOfAnhourWithMinute * DDECalendar::QuarterOfAnhourWithMinute + DDECalendar::QuarterOfAnhourWithMinute;
        }
        m_beginTimeEdit->setTime(QTime(hours, minnutes));
        m_endDateEdit->setDate(QDate::currentDate());
        m_endTimeEdit->setTime(QTime(hours, minnutes).addSecs(3600));
    } else {
        m_titleLabel->setText(tr("Edit Event"));
    }

    setFixedSize(438, 470);
    //焦点设置到输入框
    m_textEdit->setFocus();
    CScheduleDlg::DialogIsClose = false;
}

CScheduleDlg::~CScheduleDlg()
{
    CScheduleDlg::DialogIsClose = true;
}

void CScheduleDlg::setData(const ScheduleDataInfo &info)
{
    m_ScheduleDataInfo = info;
    m_typeComBox->setCurrentIndex(info.getType() - 1);
    if (m_type == 1) {
        //如果为新建则设置为提示信息
        m_textEdit->setPlaceholderText(info.getTitleName());
    } else {
        //如果为编辑则显示
        m_textEdit->setPlainText(info.getTitleName());
        //光标移动到文末
        m_textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    }

    m_beginDateEdit->setDate(info.getBeginDateTime().date());
    m_beginTimeEdit->setTime(info.getBeginDateTime().time());
    m_endDateEdit->setDate(info.getEndDateTime().date());
    m_endTimeEdit->setTime(info.getEndDateTime().time());
    m_allDayCheckbox->setChecked(info.getAllDay());
    m_endRepeatDate->setMinimumDate(info.getBeginDateTime().date());

    m_currentDate = info.getBeginDateTime();
    m_EndDate = info.getEndDateTime();

    slotallDayStateChanged(info.getAllDay());
    initRmindRpeatUI();
}

void CScheduleDlg::setDate(const QDateTime &date)
{
    m_currentDate = date;
    int hours = date.time().hour();
    int minnutes = 0;
    //取15的整数倍
    minnutes = date.time().minute() / DDECalendar::QuarterOfAnhourWithMinute * DDECalendar::QuarterOfAnhourWithMinute;
    //如果有余数则添加15分钟
    minnutes = minnutes + (date.time().minute() % DDECalendar::QuarterOfAnhourWithMinute == 0 ? 0 : 15);
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
    m_EndDate = datetime;
    m_endDateEdit->setDate(datetime.date());
    m_endTimeEdit->setTime(datetime.time());
    m_endRepeatDate->setMinimumDate(date.date());
}

void CScheduleDlg::setAllDay(bool flag)
{
    m_allDayCheckbox->setChecked(flag);
}

/**
 * @brief CScheduleDlg::clickOkBtn      对话框确定按钮处理
 * @return              返回true表示隐藏对话框  false表示不隐藏对话框
 */
bool CScheduleDlg::clickOkBtn()
{
    ScheduleDataInfo _newSchedule = m_ScheduleDataInfo;
    QDateTime beginDateTime, endDateTime;
    beginDateTime.setDate(m_beginDateEdit->date());
    beginDateTime.setTime(m_beginTimeEdit->getTime());
    endDateTime.setDate(m_endDateEdit->date());
    endDateTime.setTime(m_endTimeEdit->getTime());

    if (m_textEdit->toPlainText().isEmpty()) {
        _newSchedule.setTitleName(m_textEdit->placeholderText());
    } else {
        _newSchedule.setTitleName(m_textEdit->toPlainText());
    }

    if (_newSchedule.getTitleName().isEmpty()) {
        return false;
    }
    //如果类型选项不为负数则设置日程类型
    if (m_typeComBox->currentIndex() >= 0)
        _newSchedule.setType(m_typeComBox->currentIndex() + 1);

    if (beginDateTime > endDateTime) {
        DCalendarDDialog *prompt = new DCalendarDDialog(this);
        prompt->setIcon(QIcon(":/resources/icon/warning.svg"));
        prompt->setMessage(tr("End time must be greater than start time"));
        prompt->addButton(tr("OK"), true, DDialog::ButtonNormal);
        prompt->exec();
        return false;
    }
    if (m_type == 1)
        _newSchedule.setID(0) ;
    _newSchedule.setAllDay(m_allDayCheckbox->isChecked());

    RemindData _remindData;
    if (_newSchedule.getAllDay()) {
        _remindData.setRemindTime(QTime(9, 0));
        switch (m_rmindCombox->currentIndex()) {
        case 1:
            _remindData.setRemindNum(DDECalendar::OnStartDay);
            break;
        case 2:
            _remindData.setRemindNum(DDECalendar::OneDayBeforeWithDay);
            break;
        case 3:
            _remindData.setRemindNum(DDECalendar::TwoDayBeforeWithDay);
            break;
        case 4:
            _remindData.setRemindNum(DDECalendar::OneWeekBeforeWithDay);
            break;
        default:
            break;
        }
    } else {
        switch (m_rmindCombox->currentIndex()) {
        case 1:
            _remindData.setRemindNum(DDECalendar::AtTimeOfEvent);
            break;
        case 2:
            _remindData.setRemindNum(DDECalendar::FifteenMinutesBefore);
            break;
        case 3:
            _remindData.setRemindNum(DDECalendar::ThirtyMinutesBefore);
            break;
        case 4:
            _remindData.setRemindNum(DDECalendar::OneHourBefore);
            break;
        case 5:
            _remindData.setRemindNum(DDECalendar::OneDayBeforeWithMinutes);
            break;
        case 6:
            _remindData.setRemindNum(DDECalendar::TwoDayBeforeWithMinutes);
            break;
        case 7:
            _remindData.setRemindNum(DDECalendar::OneWeekBeforeWithMinutes);
            break;
        default:
            break;
        }
    }
    _newSchedule.setRemindData(_remindData);

    RepetitionRule _repetitionRule;
    _repetitionRule.setRuleId(static_cast<RepetitionRule::RRuleID>(m_beginrepeatCombox->currentIndex()));
    if (_repetitionRule.getRuleId() > 0) {
        _repetitionRule.setRuleType(static_cast<RepetitionRule::RRuleEndType>(m_endrepeatCombox->currentIndex()));
        if (m_endrepeatCombox->currentIndex() == 1) {
            if (m_endrepeattimes->text().isEmpty()) {
                return false;
            }
            _repetitionRule.setEndCount(m_endrepeattimes->text().toInt());
        } else if (m_endrepeatCombox->currentIndex() == 2) {
            QDateTime endrpeattime = beginDateTime;
            endrpeattime.setDate(m_endRepeatDate->date());

            if (beginDateTime > endrpeattime) {
                return false;
            }
            _repetitionRule.setEndDate(endrpeattime);
        }
    }
    _newSchedule.setRepetitionRule(_repetitionRule);
    _newSchedule.setBeginDateTime(beginDateTime);
    _newSchedule.setEndDateTime(endDateTime);
    CScheduleOperation _scheduleOperation(this);

    if (m_type == 1) {
        //创建日程
        _scheduleOperation.createSchedule(_newSchedule);
    } else if (m_type == 0) {
        //修改日程,根据返回的参数判断是否关闭对话框
        return _scheduleOperation.changeSchedule(_newSchedule, m_ScheduleDataInfo);
    }
    return true;
}

void CScheduleDlg::slotBtClick(int buttonIndex, const QString &buttonName)
{
    Q_UNUSED(buttonName)
    //是否隐藏对话框
    switch (buttonIndex) {
    case 0: {
        //取消
        close();
        break;
    }
    case 1: {
        //确定
        //自动化测试会出现短时间内按钮click2次的情况。添加第一次触发后将保存按钮置灰的设置。
        //若保存按钮不启用则不处理
        if (getButton(1)->isEnabled()) {
            m_setAccept = clickOkBtn();
            //若新建或编辑成功则将保存按钮置灰
            getButton(1)->setEnabled(!m_setAccept);
            //如果确定创建(修改)成功则关闭对话框
            if (m_setAccept) {
                close();
            } else {
                setFocusProxy(getButton(1));
            }
        }
        break;
    }
    default:
        break;
    }
}

void CScheduleDlg::slotTextChange()
{
    QString textContent = m_textEdit->toPlainText();
    int length = textContent.count();
    QString tStitlename = textContent;

    int maxLength = 256; // 最大字符数
    //去除回车字符
    if (tStitlename.contains("\n")) {
        //设置纯文本显示原始内容
        tStitlename.replace("\n", "");
        m_textEdit->setPlainText(tStitlename);
        //将焦点移动到最后
        m_textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        //因设置text会再次触发textchange信号,在此直接return
        return;
    }
    //如果长度大于最大长度则显示原来的字符
    if (length > maxLength) {
        m_textEdit->setPlainText(m_context);
        //将焦点移动到最后
        m_textEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        return;
    }

    m_context = m_textEdit->toPlainText();
}

void CScheduleDlg::slotendrepeatTextchange()
{
    QAbstractButton *m_OkBt = getButton(1);

    if (m_endrepeattimes->text().isEmpty())
        m_OkBt->setEnabled(false);
    else
        m_OkBt->setEnabled(true);
}

void CScheduleDlg::slotBDateEidtInfo(const QDate &date)
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

void CScheduleDlg::slotallDayStateChanged(int state)
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
            m_beginDateEdit->setDate(m_ScheduleDataInfo.getBeginDateTime().date());
            m_beginTimeEdit->setTime(m_ScheduleDataInfo.getBeginDateTime().time());
            m_endDateEdit->setDate(m_ScheduleDataInfo.getEndDateTime().date());
            m_endTimeEdit->setTime(m_ScheduleDataInfo.getEndDateTime().time());
        } else {
            m_beginDateEdit->setDate(m_currentDate.date());
            m_beginTimeEdit->setTime(m_currentDate.time());
            m_endDateEdit->setDate(m_EndDate.date());
            m_endTimeEdit->setTime(m_EndDate.time());
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
            m_beginDateEdit->setDate(m_ScheduleDataInfo.getBeginDateTime().date());
            m_beginTimeEdit->setTime(QTime(0, 0));
            m_endDateEdit->setDate(m_ScheduleDataInfo.getEndDateTime().date());
            m_endTimeEdit->setTime(QTime(23, 59));
        } else {
            m_beginDateEdit->setDate(m_currentDate.date());
            m_endDateEdit->setDate(m_EndDate.date());
            m_beginTimeEdit->setTime(QTime(0, 0));
            m_endTimeEdit->setTime(QTime(23, 59));
            m_rmindCombox->setCurrentIndex(2);
        }
    }
}

void CScheduleDlg::slotbRpeatactivated(int index)
{
    if (index > 0) {
        m_endrepeatWidget->setVisible(true);
        setFixedSize(438, 506);
        if (m_endrepeatCombox->currentIndex() == 1) {
            //如果结束重复于次数，判断次数是否为空
            slotendrepeatTextchange();
        }
    } else {
        m_endrepeatWidget->setVisible(false);
        setFixedSize(438, 470);
        //重复类型为“从不”时，使能保存按钮
        QAbstractButton *m_OkBt = getButton(1);
        m_OkBt->setEnabled(true);
    }
}

void CScheduleDlg::sloteRpeatactivated(int index)
{
    if (index == 0) {
        m_endrepeattimesWidget->setVisible(false);
        m_endRepeatDate->setVisible(false);
    } else if (index == 1) {
        m_endrepeattimesWidget->setVisible(true);
        m_endRepeatDate->setVisible(false);
        //重复结束于次数，判断次数是否为空
        slotendrepeatTextchange();
    } else {
        m_endrepeattimesWidget->setVisible(false);
        m_endRepeatDate->setVisible(true);
    }
    if (index != 1) {
        //只要不是结束重复于次数，使能保存按钮
        QAbstractButton *m_OkBt = getButton(1);
        m_OkBt->setEnabled(true);
    }
}

/**
 * @brief CScheduleDlg::slotVisibleChange
 * 获取虚拟键盘显示状态
 */
void CScheduleDlg::slotVisibleChange()
{
    //如果虚拟键盘显示
    if (TabletConfig::isShowVirtualKeyboard()) {
        //如果虚拟键盘会遮挡对话框则调整Y坐标
        if (getMoveYOffset() > 0) {
            move(this->x(), this->y() - getMoveYOffset());
            //设置未居中显示
            m_isCenter = false;
        }
    } else {
        //如果没有居中显示则移动到中央
        if (!m_isCenter) {
            //隐藏虚拟键盘，界面调整到屏幕中央,延迟300毫秒后隐藏
            QTimer::singleShot(300, [=] {
                //如果窗口没有被关闭则调整界面
                if (!CScheduleDlg::DialogIsClose) {
                    moveToCenter();
                }
            });
        }
    }
}

bool CScheduleDlg::eventFilter(QObject *obj, QEvent *pEvent)
{
    if (obj == m_textEdit) {
        if (pEvent->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(pEvent);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                return true;
            }
            if (keyEvent->key() == Qt::Key_Tab) {
                //如果为tab则切换到全天checkbox
                m_allDayCheckbox->setFocus(Qt::TabFocusReason);
                return true;
            }
        }
        if (pEvent->type() == QEvent::FocusIn) {
            setVirtualKeyboard(true);
        }
        if (pEvent->type() == QEvent::FocusOut) {
            setVirtualKeyboard(false);
        }
    }

    if (m_endrepeattimes != nullptr && obj == m_endrepeattimes->lineEdit()) {
        if (pEvent->type() == QEvent::FocusIn) {
            setVirtualKeyboard(true);
        }
        if (pEvent->type() == QEvent::FocusOut) {
            setVirtualKeyboard(false);
        }
    }
    return DCalendarDDialog::eventFilter(obj, pEvent);
}

void CScheduleDlg::closeEvent(QCloseEvent *event)
{
    DDialog::closeEvent(event);
    //如果为true 这设置返回值为Accepted 否则设置为Rejected
    if (m_setAccept) {
        accept();
    } else {
        reject();
    }
}

void CScheduleDlg::changeEvent(QEvent *event)
{
    Q_UNUSED(event);
    QFont mlabelF;
    mlabelF.setWeight(QFont::Medium);

    QFontMetrics fontWidth_typeLabel(mlabelF);
    QString str_typelabel = fontWidth_typeLabel.elidedText(tr("Type:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_typeLabel->setText(str_typelabel);

    QFontMetrics fontWidth_contentlabel(mlabelF);
    QString str_contentlabel = fontWidth_contentlabel.elidedText(tr("Description:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_contentLabel->setText(str_contentlabel);

    QFontMetrics fontWidth_allDayLabel(mlabelF);
    QString str_allDayLabel = fontWidth_allDayLabel.elidedText(tr("All Day:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_adllDayLabel->setText(str_allDayLabel);

    QFontMetrics fontWidth_beginTimeLabel(mlabelF);
    QString str_beginTimeLabel = fontWidth_beginTimeLabel.elidedText(tr("Starts:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_beginTimeLabel->setText(str_beginTimeLabel);

    QFontMetrics fontWidth_endTimeLabel(mlabelF);
    QString str_endTimeLabel = fontWidth_endTimeLabel.elidedText(tr("Ends:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_endTimeLabel->setText(str_endTimeLabel);

    QFontMetrics fontWidth_remindSetLabel(mlabelF);
    QString str_remindSetLabel = fontWidth_remindSetLabel.elidedText(tr("Remind Me:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_remindSetLabel->setText(str_remindSetLabel);

    QFontMetrics fontWidth_beginRepeatLabel(mlabelF);
    QString str_beginRepeatLabel = fontWidth_beginRepeatLabel.elidedText(tr("Repeat:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_beginrepeatLabel->setText(str_beginRepeatLabel);

    QFontMetrics fontWidth_endrepeatLabel(mlabelF);
    QString str_endrepeatLabel = fontWidth_endrepeatLabel.elidedText(tr("End Repeat:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_endrepeatLabel->setText(str_endrepeatLabel);
}

/**
 * @brief CScheduleDlg::updateDateTimeFormat 更新日期显示格式
 */
void CScheduleDlg::updateDateTimeFormat()
{
    m_beginDateEdit->setDisplayFormat(m_dateFormat);
    m_endDateEdit->setDisplayFormat(m_dateFormat);
    m_endRepeatDate->setDisplayFormat(m_dateFormat);
}

/**
 * @brief CScheduleDlg::getMoveYOffset  根据虚拟键盘获取对话框Y坐标偏移量
 * @return
 */
int CScheduleDlg::getMoveYOffset()
{
    QRectF keyboardRectF = TabletConfig::getKeyboardRectangle();
    int keyboardY = qRound(QApplication::desktop()->availableGeometry().height() - keyboardRectF.height());
    return this->geometry().bottom() - keyboardY;
}

void CScheduleDlg::setVirtualKeyboard(bool isShow)
{
    if (!isShow) {
        //如果当前焦点为下拉选择框则立即移动窗口
        QWidget *currentFocus = focusWidget();
        QList<QWidget *> cbbList {m_typeComBox, m_rmindCombox, m_beginrepeatCombox, m_endrepeatCombox, m_beginDateEdit, m_beginTimeEdit, m_endDateEdit, m_endTimeEdit, m_endRepeatDate};
        if (cbbList.contains(currentFocus)) {
            moveToCenter();
            m_isCenter = true;
        }
    }
    //是否显示虚拟键盘
    TabletConfig::setVirtualKeyboard(isShow);
}

void CScheduleDlg::initUI()
{
    //设置对象名称和辅助显示名称
    this->setObjectName("ScheduleEditDialog");
    this->setAccessibleName("ScheduleEditDialog");
    //在点击任何对话框上的按钮后不关闭对话框，保证关闭子窗口时不被一起关掉
    setOnButtonClickedClose(false);

    m_titleLabel = new QLabel(this);
    QFont titlelabelF;
    titlelabelF.setWeight(QFont::DemiBold);
    titlelabelF.setPixelSize(17);
    m_titleLabel->setFixedSize(148, 51);
    m_titleLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    m_titleLabel->move(145, 0);
    m_titleLabel->setFont(titlelabelF);

    setSpacing(0);
    QFont mlabelF;
    mlabelF.setWeight(QFont::Medium);

    QVBoxLayout *maintlayout = new QVBoxLayout;
    maintlayout->setMargin(0);
    maintlayout->setSpacing(10);
    QHBoxLayout *typelayout = new QHBoxLayout;
    typelayout->setSpacing(0);
    typelayout->setMargin(0);
    m_typeLabel = new QLabel();
    m_typeLabel->setToolTip(tr("Type"));
    DFontSizeManager::instance()->bind(m_typeLabel, DFontSizeManager::T6);
    QFontMetrics fontWidth_typeLabel(mlabelF);
    QString str_typelabel = fontWidth_typeLabel.elidedText(tr("Type:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_typeLabel->setText(str_typelabel);
    m_typeLabel->setFont(mlabelF);
    m_typeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_typeLabel->setFixedSize(78, 36);
    m_typeComBox = new DComboBox(this);
    //设置对象名称和辅助显示名称
    m_typeComBox->setObjectName("ScheduleTypeCombobox");
    m_typeComBox->setAccessibleName("ScheduleTypeCombobox");
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
    m_contentLabel = new QLabel(this);
    DFontSizeManager::instance()->bind(m_contentLabel, DFontSizeManager::T6);
    QFontMetrics fontWidth_contentlabel(mlabelF);
    QString str_contentlabel = fontWidth_contentlabel.elidedText(tr("Description:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_contentLabel->setText(str_contentlabel);
    m_contentLabel->setFont(mlabelF);
    m_contentLabel->setToolTip(tr("Description"));
    m_contentLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_contentLabel->setFixedWidth(78);
    conttelabellayout->addWidget(m_contentLabel);
    conttelabellayout->addStretch();
    m_textEdit = new DTextEdit(this);
    //设置对象名称和辅助显示名称
    m_textEdit->setObjectName("ScheduleTitleEdit");
    m_textEdit->setAccessibleName("ScheduleTitleEdit");
    m_textEdit->setFixedSize(319, 86);
    m_textEdit->setAcceptRichText(false);

    m_textEdit->setPlaceholderText(tr("New Event"));
    //设置关联控件，用于QTextEdit控件捕获MouseButtonPress等事件
    QWidget *mpContentWidget = m_textEdit->viewport();
    //设置事件过滤器
    m_textEdit->installEventFilter(this);
    mpContentWidget->installEventFilter(this);

    contentLabellayout->addLayout(conttelabellayout);
    contentLabellayout->addWidget(m_textEdit);
    maintlayout->addLayout(contentLabellayout);

    QHBoxLayout *alldayLabellayout = new QHBoxLayout;
    alldayLabellayout->setSpacing(0);
    alldayLabellayout->setMargin(0);
    m_adllDayLabel = new QLabel(this);
    m_adllDayLabel->setToolTip(tr("All Day"));
    DFontSizeManager::instance()->bind(m_adllDayLabel, DFontSizeManager::T6);
    QFontMetrics fontWidth_allDayLabel(mlabelF);
    QString str_allDayLabel = fontWidth_allDayLabel.elidedText(tr("All Day:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_adllDayLabel->setText(str_allDayLabel);
    m_adllDayLabel->setFont(mlabelF);
    m_adllDayLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_adllDayLabel->setFixedWidth(78);
    m_allDayCheckbox = new DCheckBox(this);
    //设置对象名称和辅助显示名称
    m_allDayCheckbox->setObjectName("AllDayCheckBox");
    m_allDayCheckbox->setAccessibleName("AllDayCheckBox");
    alldayLabellayout->addWidget(m_adllDayLabel);
    alldayLabellayout->addWidget(m_allDayCheckbox);
    maintlayout->addLayout(alldayLabellayout);

    QHBoxLayout *beginLabellayout = new QHBoxLayout;
    beginLabellayout->setSpacing(0);
    beginLabellayout->setMargin(0);
    m_beginTimeLabel = new QLabel(this);
    m_beginTimeLabel->setToolTip(tr("Starts"));
    DFontSizeManager::instance()->bind(m_beginTimeLabel, DFontSizeManager::T6);
    QFontMetrics fontWidth_beginTimeLabel(mlabelF);
    QString str_beginTimeLabel = fontWidth_beginTimeLabel.elidedText(tr("Starts:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_beginTimeLabel->setText(str_beginTimeLabel);
    m_beginTimeLabel->setFont(mlabelF);
    m_beginTimeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_beginTimeLabel->setFixedSize(78, 36);
    m_beginDateEdit = new CDateEdit(this);
    //设置对象名称和辅助显示名称
    m_beginDateEdit->setObjectName("ScheduleBeginDateEdit");
    m_beginDateEdit->setAccessibleName("ScheduleBeginDateEdit");
    m_beginDateEdit->setFixedSize(175, 36);

    m_beginTimeEdit = new CTimeEdit(this);
    //设置对象名称和辅助显示名称
    m_beginTimeEdit->setObjectName("ScheduleBeginTimeEdit");
    m_beginTimeEdit->setAccessibleName("ScheduleBeginTimeEdit");
    m_beginTimeEdit->setFixedSize(141, 36);
    m_beginDateEdit->setCalendarPopup(true);
    m_beginDateEdit->setDisplayFormat(m_dateFormat);
    beginLabellayout->addWidget(m_beginTimeLabel);
    beginLabellayout->addWidget(m_beginDateEdit);
    beginLabellayout->addSpacing(8);
    beginLabellayout->addWidget(m_beginTimeEdit);
    beginLabellayout->addStretch();
    maintlayout->addLayout(beginLabellayout);

    QHBoxLayout *enQLabellayout = new QHBoxLayout;
    enQLabellayout->setSpacing(0);
    enQLabellayout->setMargin(0);
    m_endTimeLabel = new QLabel(this);
    m_endTimeLabel->setToolTip(tr("Ends"));
    DFontSizeManager::instance()->bind(m_endTimeLabel, DFontSizeManager::T6);
    QFontMetrics fontWidth_endTimeLabel(mlabelF);
    QString str_endTimeLabel = fontWidth_endTimeLabel.elidedText(tr("Ends:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_endTimeLabel->setText(str_endTimeLabel);
    m_endTimeLabel->setFont(mlabelF);
    m_endTimeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_endTimeLabel->setFixedSize(78, 36);
    m_endDateEdit = new CDateEdit(this);
    //设置对象名称和辅助显示名称
    m_endDateEdit->setObjectName("ScheduleEndDateEdit");
    m_endDateEdit->setAccessibleName("ScheduleEndDateEdit");
    m_endDateEdit->setFixedSize(175, 36);
    m_endTimeEdit = new CTimeEdit(this);
    //设置对象名称和辅助显示名称
    m_endTimeEdit->setObjectName("ScheduleEndTimeEdit");
    m_endTimeEdit->setAccessibleName("ScheduleEndTimeEdit");
    m_endTimeEdit->setFixedSize(141, 36);
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat(m_dateFormat);

    enQLabellayout->addWidget(m_endTimeLabel);
    enQLabellayout->addWidget(m_endDateEdit);
    enQLabellayout->addSpacing(8);

    enQLabellayout->addWidget(m_endTimeEdit);
    enQLabellayout->addStretch();
    maintlayout->addLayout(enQLabellayout);

    QHBoxLayout *rminQLabellayout = new QHBoxLayout;
    rminQLabellayout->setSpacing(0);
    rminQLabellayout->setMargin(0);
    m_remindSetLabel = new QLabel();
    DFontSizeManager::instance()->bind(m_remindSetLabel, DFontSizeManager::T6);
    QFontMetrics fontWidth_remindSetLabel(mlabelF);
    QString str_remindSetLabel = fontWidth_remindSetLabel.elidedText(tr("Remind Me:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_remindSetLabel->setToolTip(tr("Remind Me"));
    m_remindSetLabel->setText(str_remindSetLabel);
    m_remindSetLabel->setFont(mlabelF);
    m_remindSetLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_remindSetLabel->setFixedWidth(78);

    m_rmindCombox = new DComboBox(this);
    //设置对象名称和辅助显示名称
    m_rmindCombox->setObjectName("RmindComboBox");
    m_rmindCombox->setAccessibleName("RmindComboBox");
    m_rmindCombox->setFixedSize(175, 36);
    m_rmindCombox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    rminQLabellayout->addWidget(m_remindSetLabel);
    rminQLabellayout->addWidget(m_rmindCombox);
    rminQLabellayout->addStretch();
    maintlayout->addLayout(rminQLabellayout);

    QHBoxLayout *repeatLabellayout = new QHBoxLayout;
    repeatLabellayout->setSpacing(0);
    repeatLabellayout->setMargin(0);
    m_beginrepeatLabel = new QLabel();
    m_beginrepeatLabel->setToolTip(tr("Repeat"));
    DFontSizeManager::instance()->bind(m_beginrepeatLabel, DFontSizeManager::T6);
    QFontMetrics fontWidth_beginRepeatLabel(mlabelF);
    QString str_beginRepeatLabel = fontWidth_beginRepeatLabel.elidedText(tr("Repeat:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_beginrepeatLabel->setText(str_beginRepeatLabel);
    m_beginrepeatLabel->setFont(mlabelF);
    m_beginrepeatLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_beginrepeatLabel->setFixedWidth(78);
    m_beginrepeatCombox = new DComboBox(this);
    //设置对象名称和辅助显示名称
    m_beginrepeatCombox->setObjectName("BeginRepeatComboBox");
    m_beginrepeatCombox->setAccessibleName("BeginRepeatComboBox");
    m_beginrepeatCombox->setFixedSize(175, 36);
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
    DFontSizeManager::instance()->bind(m_endrepeatLabel, DFontSizeManager::T6);
    QFontMetrics fontWidth_endrepeatLabel(mlabelF);
    QString str_endrepeatLabel = fontWidth_endrepeatLabel.elidedText(tr("End Repeat:"), Qt::ElideRight, DDECalendar::NewScheduleLabelWidth);
    m_endrepeatLabel->setToolTip(tr("End Repeat"));
    m_endrepeatLabel->setText(str_endrepeatLabel);
    m_endrepeatLabel->setFont(mlabelF);
    m_endrepeatLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_endrepeatLabel->setFixedWidth(78);
    m_endrepeatCombox = new DComboBox(this);
    //设置对象名称和辅助显示名称
    m_endrepeatCombox->setObjectName("EndRepeatComboBox");
    m_endrepeatCombox->setAccessibleName("EndRepeatComboBox");
    m_endrepeatCombox->setFixedSize(175, 36);
    m_endrepeatCombox->addItem(tr("Never"));
    m_endrepeatCombox->addItem(tr("After"));
    m_endrepeatCombox->addItem(tr("On"));
    endrepeatLabellayout->addWidget(m_endrepeatLabel);
    endrepeatLabellayout->addWidget(m_endrepeatCombox);
    endrepeatLabellayout->addSpacing(8);

    QHBoxLayout *endrepeattimeslayout = new QHBoxLayout;
    endrepeattimeslayout->setSpacing(0);
    endrepeattimeslayout->setMargin(0);
    endrepeattimeslayout->setContentsMargins(10, 0, 0, 0);
    m_endrepeattimes = new DLineEdit(this);
    //设置对象名称和辅助显示名称
    m_endrepeattimes->setObjectName("EndRepeatTimeEidt");
    m_endrepeattimes->setAccessibleName("EndRepeatTimeEidt");
    m_endrepeattimes->setFixedSize(71, 36);
    m_endrepeattimes->setText(QString::number(10));
    m_endrepeattimes->setClearButtonEnabled(false);
    m_endrepeattimes->lineEdit()->installEventFilter(this);
    QRegExp rx("^[1-9]\\d{0,2}$");
    QValidator *validator = new QRegExpValidator(rx, this);
    m_endrepeattimes->lineEdit()->setValidator(validator);
    m_endrepeattimesLabel = new QLabel(tr("time(s)"));
    m_endrepeattimesLabel->setFont(mlabelF);
    m_endrepeattimesLabel->setFixedHeight(36);
    endrepeattimeslayout->addWidget(m_endrepeattimes);
    endrepeattimeslayout->addWidget(m_endrepeattimesLabel);
    m_endrepeattimesWidget = new DWidget;
    //设置对象名称和辅助显示名称
    m_endrepeattimesWidget->setObjectName("EndRepeatTimeWidget");
    m_endrepeattimesWidget->setAccessibleName("EndRepeatTimeWidget");
    m_endrepeattimesWidget->setLayout(endrepeattimeslayout);
    m_endrepeattimesWidget->setVisible(false);
    endrepeatLabellayout->addWidget(m_endrepeattimesWidget);

    m_endRepeatDate = new CDateEdit;
    //设置对象名称和辅助显示名称
    m_endRepeatDate->setObjectName("EndRepeatDateEdit");
    m_endRepeatDate->setAccessibleName("EndRepeatDateEdit");
    m_endRepeatDate->setCalendarPopup(true);
    m_endRepeatDate->setFixedSize(141, 36);
    m_endRepeatDate->setDate(QDate::currentDate());
    m_endRepeatDate->setDisplayFormat(m_dateFormat);
    m_endRepeatDate->setCurrentSectionIndex(2);
    QFont enddatefont;
    enddatefont.setWeight(QFont::Medium);
    m_endRepeatDate->setFont(enddatefont);
    endrepeatLabellayout->addWidget(m_endRepeatDate);
    endrepeatLabellayout->addStretch();
    m_endRepeatDate->setVisible(false);
    m_endrepeatWidget = new DWidget;
    //设置对象名称和辅助显示名称
    m_endrepeatWidget->setObjectName("EndRepeatDateWidget");
    m_endrepeatWidget->setAccessibleName("EndRepeatDateWidget");
    m_endrepeatWidget->setLayout(endrepeatLabellayout);
    maintlayout->addWidget(m_endrepeatWidget);
    m_endrepeatWidget->setVisible(false);

    for (int i = 0; i < buttonCount(); i++) {
        QAbstractButton *button = getButton(i);
        button->setFixedSize(189, 36);
    }
    m_gwi = new DFrame(this);
    m_gwi->setFrameShape(QFrame::NoFrame);
    m_gwi->setLayout(maintlayout);
    addContent(m_gwi, Qt::AlignCenter);

    //取消DDialog对虚拟键盘遮挡处理
    qApp->ignoreVirtualKeyboard(m_gwi->parentWidget());

    initDateEdit();
    if (m_type == 1)
        slotallDayStateChanged(0);
    //添加按钮
    addButton(tr("Cancel"));
    addButton(tr("Save"), false, DDialog::ButtonRecommend);
}

void CScheduleDlg::initConnection()
{
    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
                     this,
                     &CScheduleDlg::setTheMe);
    connect(this, &DDialog::buttonClicked, this, &CScheduleDlg::slotBtClick);
    connect(m_textEdit, &DTextEdit::textChanged, this, &CScheduleDlg::slotTextChange);
    connect(m_endrepeattimes, &DLineEdit::textChanged, this, &CScheduleDlg::slotendrepeatTextchange);

    connect(m_allDayCheckbox, &DCheckBox::stateChanged, this,
            &CScheduleDlg::slotallDayStateChanged);
    connect(m_beginrepeatCombox, QOverload<int>::of(&QComboBox::activated), this,
            &CScheduleDlg::slotbRpeatactivated);
    connect(m_endrepeatCombox, QOverload<int>::of(&QComboBox::activated), this,
            &CScheduleDlg::sloteRpeatactivated);
    connect(m_beginDateEdit, &DDateEdit::userDateChanged, this, &CScheduleDlg::slotBDateEidtInfo);
    QShortcut *shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(QLatin1String("ESC")));
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));
    //关联虚拟键盘显示改变信号
    connect(DApplication::inputMethod(), &QInputMethod::visibleChanged, this, &CScheduleDlg::slotVisibleChange);
}

void CScheduleDlg::initDateEdit()
{
    m_beginDateEdit->setMinimumDate(QDate(DDECalendar::QueryEarliestYear, 1, 1)); // 0天
    m_beginDateEdit->setMaximumDate(QDate(DDECalendar::QueryLatestYear, 12, 31));
    m_endDateEdit->setMinimumDate(QDate(DDECalendar::QueryEarliestYear, 1, 1)); // 0天
    m_endDateEdit->setMaximumDate(QDate(DDECalendar::QueryLatestYear, 12, 31));
    return;
}

void CScheduleDlg::initRmindRpeatUI()
{
    if (m_ScheduleDataInfo.getAllDay()) {
        if (m_ScheduleDataInfo.getRemindData().getRemindNum() > -1) {
            if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::OnStartDay) {
                m_rmindCombox->setCurrentIndex(1);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::OneDayBeforeWithDay) {
                m_rmindCombox->setCurrentIndex(2);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::TwoDayBeforeWithDay) {
                m_rmindCombox->setCurrentIndex(3);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::OneWeekBeforeWithDay) {
                m_rmindCombox->setCurrentIndex(4);
            }
        } else {
            m_rmindCombox->setCurrentIndex(0);
        }
    } else {
        if (m_ScheduleDataInfo.getRemindData().getRemindNum() > -1) {
            if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::AtTimeOfEvent) {
                m_rmindCombox->setCurrentIndex(1);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::FifteenMinutesBefore) {
                m_rmindCombox->setCurrentIndex(2);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::ThirtyMinutesBefore) {
                m_rmindCombox->setCurrentIndex(3);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::OneHourBefore) {
                m_rmindCombox->setCurrentIndex(4);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::OneDayBeforeWithMinutes) {
                m_rmindCombox->setCurrentIndex(5);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::TwoDayBeforeWithMinutes) {
                m_rmindCombox->setCurrentIndex(6);
            } else if (m_ScheduleDataInfo.getRemindData().getRemindNum() == DDECalendar::OneWeekBeforeWithMinutes) {
                m_rmindCombox->setCurrentIndex(7);
            }
        } else {
            m_rmindCombox->setCurrentIndex(0);
        }
    }
    slotbRpeatactivated(m_ScheduleDataInfo.getRepetitionRule().getRuleId());
    m_beginrepeatCombox->setCurrentIndex(m_ScheduleDataInfo.getRepetitionRule().getRuleId());

    if (m_ScheduleDataInfo.getRepetitionRule().getRuleId() != 0) {
        if (m_ScheduleDataInfo.getRepetitionRule().getRuleType() == 0) {
            m_endrepeatCombox->setCurrentIndex(0);
        } else if (m_ScheduleDataInfo.getRepetitionRule().getRuleType() == 1) {
            m_endrepeatCombox->setCurrentIndex(1);
            m_endrepeattimes->setText(QString::number(m_ScheduleDataInfo.getRepetitionRule().getEndCount()));
        } else if (m_ScheduleDataInfo.getRepetitionRule().getRuleType() == 2) {
            m_endrepeatCombox->setCurrentIndex(2);
            m_endRepeatDate->setDate(m_ScheduleDataInfo.getRepetitionRule().getEndDate().date());
        }
        m_endrepeatWidget->show();
        sloteRpeatactivated(m_ScheduleDataInfo.getRepetitionRule().getRuleType());
    } else {
        m_endrepeatWidget->hide();
    }
}

void CScheduleDlg::setTheMe(const int type)
{
    //日程标题编辑框文字颜色
    QColor titleColor;
    if (type == 2) {
        titleColor = "#C0C6D4";
    } else {
        titleColor = "#414D68";
    }
    DPalette pa = m_textEdit->palette();
    //设置颜色
    pa.setColor(DPalette::Text, titleColor);
    m_textEdit->setPalette(pa);
}

/**
 * @brief CScheduleDlg::setTabFouseOrder
 *      设置tab顺序
 */
void CScheduleDlg::setTabFouseOrder()
{
    setTabOrder(m_typeComBox, m_textEdit);
    setTabOrder(m_textEdit, m_allDayCheckbox);
    setTabOrder(m_allDayCheckbox, m_beginDateEdit);
    setTabOrder(m_beginDateEdit, m_beginTimeEdit);
//    setTabOrder(m_beginTimeEdit, m_endDateEdit);
    setTabOrder(m_endDateEdit, m_endTimeEdit);
//    setTabOrder(m_endTimeEdit, m_rmindCombox);
    setTabOrder(m_rmindCombox, m_beginrepeatCombox);
    setTabOrder(m_beginrepeatCombox, m_endrepeatCombox);
    //结束于次数，设置tab顺序
    if (m_ScheduleDataInfo.getRepetitionRule().getRuleType() == RepetitionRule::RRuleEndType::RRuleType_FREQ)
        setTabOrder(m_endrepeatCombox, m_endrepeattimes);
    //结束于日期，设置tab顺序
    if (m_ScheduleDataInfo.getRepetitionRule().getRuleType() == RepetitionRule::RRuleEndType::RRuleType_DATE)
        setTabOrder(m_endrepeatCombox, m_endRepeatDate);
}
