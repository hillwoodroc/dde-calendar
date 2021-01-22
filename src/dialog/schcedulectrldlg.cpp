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
#include "schcedulectrldlg.h"
#include "schceduledlg.h"
#include "scheduledatamanage.h"
#include "cdynamicicon.h"

#include <DMessageBox>
#include <DPushButton>
#include <DHiDPIHelper>
#include <DPalette>
#include <DFontSizeManager>

#include <QVBoxLayout>

DGUI_USE_NAMESPACE
CSchceduleCtrlDlg::CSchceduleCtrlDlg(QWidget *parent)
    : DDialog(parent)
{
    setContentsMargins(0, 0, 0, 0);
    initUI();
    initConnection();
    resize(380, 260);
    setFocusPolicy(Qt::ClickFocus);
}

void CSchceduleCtrlDlg::initUI()
{
    //在点击任何对话框上的按钮后不关闭对话框，保证关闭子窗口时不被一起关掉
    setOnButtonClickedClose(false);
    QIcon t_icon(CDynamicIcon::getInstance()->getPixmap());// = QIcon::fromTheme("dde-calendar");
    setIcon(t_icon);
    m_Title = new QLabel(this);
    m_Title->setFixedHeight(48);
    m_Title->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);

    QFont labelTitle;
    labelTitle.setWeight(QFont::Bold);
    labelTitle.setPixelSize(17);
    m_Title->setFont(labelTitle);

    m_mainBoxLayout = new QVBoxLayout();
    m_mainBoxLayout->setMargin(0);
    m_mainBoxLayout->setSpacing(0);

    m_firstLabel = new QLabel(this);
    m_firstLabel->setAlignment(Qt::AlignCenter);
    m_firstLabel->setFixedWidth(350);
    m_firstLabel->setWordWrap(true);
    labelF.setWeight(QFont::Medium);
    DFontSizeManager::instance()->bind(m_firstLabel, DFontSizeManager::T6);
    m_firstLabel->setForegroundRole(DPalette::WindowText);
    m_firstLabel->setFont(labelF);
    m_mainBoxLayout->addWidget(m_firstLabel);

    m_seconLabel = new QLabel(this);
    m_seconLabel->setFixedWidth(350);
    m_seconLabel->setAlignment(Qt::AlignCenter);
    labelTitle.setWeight(QFont::Bold);
    DFontSizeManager::instance()->bind(m_seconLabel, DFontSizeManager::T6);
    m_seconLabel->setForegroundRole(DPalette::WindowText);
    //设置字重
    labelT.setWeight(QFont::Normal);
    m_seconLabel->setFont(labelT);
    m_seconLabel->setWordWrap(true);
    m_mainBoxLayout->addSpacing(3);
    m_mainBoxLayout->addWidget(m_seconLabel);
    DFrame *gwi = new DFrame(this);
    gwi->setContentsMargins(0, 0, 0, 0);
    gwi->setLayout(m_mainBoxLayout);
    DPalette anipa = gwi->palette();
    QColor color = "#F8F8F8";
    color.setAlphaF(0.0);
    anipa.setColor(DPalette::Background, color);
    gwi->setAutoFillBackground(true);
    gwi->setPalette(anipa);
    gwi->setBackgroundRole(DPalette::Background);
    addContent(gwi, Qt::AlignCenter);
    setFocus();
}

void CSchceduleCtrlDlg::initConnection()
{
    //关联主题信号
    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
                     this,
                     &CSchceduleCtrlDlg::setTheMe);
    connect(this, &DDialog::buttonClicked, this, &CSchceduleCtrlDlg::buttonJudge); //连接信号和槽
}

void CSchceduleCtrlDlg::setTheMe(const int type)
{
    //标题文字颜色
    QColor titleColor;
    //提示内容文字颜色
    QColor contentColor;
    if (type == 2) {
        titleColor = "#FFFFFF";
        contentColor = "#FFFFFF";
        contentColor.setAlphaF(0.7);
    } else {
        titleColor = "#000000";
        titleColor.setAlphaF(0.9);
        contentColor = "#000000";
        contentColor.setAlphaF(0.7);
    }
    setPaletteTextColor(m_firstLabel, titleColor);
    setPaletteTextColor(m_seconLabel, contentColor);
}

void CSchceduleCtrlDlg::setPaletteTextColor(QWidget *widget, QColor textColor)
{
    //如果为空指针则退出
    if (nullptr == widget)
        return;
    DPalette palette = widget->palette();
    //设置文字显示颜色
    palette.setColor(DPalette::WindowText, textColor);
    widget->setPalette(palette);
}

void CSchceduleCtrlDlg::changeEvent(QEvent *event)
{
    Q_UNUSED(event)
    QFont font;
    QFontMetrics font_button(font);
    QFontMetrics font_firstLabel(font);
    QFontMetrics font_seconLabel(font);
    int height_firstLabel = (font_firstLabel.width(m_firstLabel->text()) / 300 + 1) * font_firstLabel.height();
    int height_seconLabel = (font_seconLabel.width(m_seconLabel->text()) / 300 + 1) * font_seconLabel.height();

    for (int i = 0; i < buttonCount(); i++) {
        QAbstractButton *button = getButton(i);
        QString str = str_btName.at(i);
        QString text_button = font_button.elidedText(str, Qt::ElideRight, 112);

        if (str.count() == 2) {
            button->setText(QString().append(str.at(0)).append(QChar::Nbsp).append(str.at(1)));
        } else {
            button->setText(text_button);
        }
    }
    setFixedHeight(36 + 48 + height_firstLabel + height_seconLabel + 30);
}

bool CSchceduleCtrlDlg::eventFilter(QObject *obj, QEvent *pEvent)
{
    return QDialog::eventFilter(obj, pEvent);
}

void CSchceduleCtrlDlg::buttonJudge(int id)
{
    m_id = id;
    accept();
}

QAbstractButton *CSchceduleCtrlDlg::addPushButton(QString btName, bool type)
{
    addButton(btName, false, DDialog::ButtonNormal);
    int button_index = getButtonIndexByText(btName);
    QAbstractButton *button = getButton(button_index);

    if (type) {
        button->setFixedHeight(36);
        button->setFixedWidth(165);
    } else {
        button->setFixedHeight(36);
        button->setFixedWidth(129);
    }

    button->setToolTip(btName);
    str_btName.append(btName);

    return button;
}

QAbstractButton *CSchceduleCtrlDlg::addsuggestButton(QString btName, bool type)
{
    addButton(btName, false, DDialog::ButtonRecommend);
    int button_index = getButtonIndexByText(btName);
    QAbstractButton *suggestButton = getButton(button_index);

    if (type) {
        suggestButton->setFixedHeight(36);
        suggestButton->setFixedWidth(165);
    } else {
        suggestButton->setFixedHeight(36);
        suggestButton->setFixedWidth(129);
    }

    suggestButton->setToolTip(btName);
    str_btName.append(btName);

    return suggestButton;
}

QAbstractButton *CSchceduleCtrlDlg::addWaringButton(QString btName, bool type)
{
    addButton(btName, false, DDialog::ButtonWarning);
    int button_index = getButtonIndexByText(btName);
    QAbstractButton *suggestButton = getButton(button_index);

    if (type) {
        suggestButton->setFixedHeight(36);
        suggestButton->setFixedWidth(165);
    } else {
        suggestButton->setFixedHeight(36);
        suggestButton->setFixedWidth(129);
    }

    suggestButton->setToolTip(btName);
    str_btName.append(btName);

    return suggestButton;
}

void CSchceduleCtrlDlg::setText(QString str)
{
    m_firstLabel->setText(str);
    m_firstLabel->setToolTip(str);
}

void CSchceduleCtrlDlg::setInformativeText(QString str)
{
    m_seconLabel->setText(str);
    m_seconLabel->setToolTip(str);
}

int CSchceduleCtrlDlg::clickButton()
{
    if (m_id < 0 || m_id > buttonCount() - 1) return buttonCount();
    return  m_id;
}

