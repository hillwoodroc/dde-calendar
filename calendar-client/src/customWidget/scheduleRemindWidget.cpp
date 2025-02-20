/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chenhaifeng  <chenhaifeng@uniontech.com>
*
* Maintainer: chenhaifeng  <chenhaifeng@uniontech.com>
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
#include "scheduleRemindWidget.h"
#include "constants.h"

#include <DGuiApplicationHelper>

#include <QPainter>
#include <QtMath>

DGUI_USE_NAMESPACE
ScheduleRemindWidget::ScheduleRemindWidget(QWidget *parent)
    : DArrowRectangle(DArrowRectangle::ArrowLeft, DArrowRectangle::FloatWindow, parent)
    , m_centerWidget(new CenterWidget(this))
{
    //如果dtk版本为5.3以上则使用新接口
#if (DTK_VERSION > DTK_VERSION_CHECK(5, 3, 0, 0))
    //设置显示圆角
    setRadiusArrowStyleEnable(true);
    //设置圆角
    setRadius(DARROWRECT::DRADIUS);
#endif
    m_centerWidget->setFixedWidth(207);
    m_centerWidget->setFixedHeight(57);
    setContent(m_centerWidget);
    this->resizeWithContent();
    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
                     m_centerWidget,
                     &CenterWidget::setTheMe);
    m_centerWidget->setTheMe(DGuiApplicationHelper::instance()->themeType());
}

ScheduleRemindWidget::~ScheduleRemindWidget()
{

}

void ScheduleRemindWidget::setData(const ScheduleDataInfo &vScheduleInfo, const CSchedulesColor &gcolor)
{
    m_centerWidget->setData(vScheduleInfo, gcolor);
    m_ScheduleInfo = vScheduleInfo;
    gdcolor = gcolor;
    this->setHeight(m_centerWidget->height() + 10);
}

/**
 * @brief ScheduleRemindWidget::setDirection       设置箭头方向
 * @param value
 */
void ScheduleRemindWidget::setDirection(DArrowRectangle::ArrowDirection value)
{
    //设置箭头方向
    this->setArrowDirection(value);
    //设置内容窗口
    this->setContent(m_centerWidget);
}

/**
 * @brief ScheduleRemindWidget::setTimeFormat 设置日期显示格式
 * @param timeformat 日期格式
 */
void ScheduleRemindWidget::setTimeFormat(QString timeformat)
{
    m_centerWidget->setTimeFormat(timeformat);
}

CenterWidget::CenterWidget(DWidget *parent)
    : DFrame(parent)
    , textwidth(0)
    , textheight(0)
{
    textfont.setWeight(QFont::Medium);
}

CenterWidget::~CenterWidget()
{

}

void CenterWidget::setData(const ScheduleDataInfo &vScheduleInfo, const CSchedulesColor &gcolor)
{
    m_ScheduleInfo = vScheduleInfo;
    gdcolor = gcolor;
    textfont.setPixelSize(DDECalendar::FontSizeTwelve);
    UpdateTextList();
    update();
}

void CenterWidget::setTheMe(const int type)
{
    if (type == 2) {
        timeColor = QColor("#C0C6D4");
        timeColor.setAlphaF(0.7);
        textColor = QColor("#C0C6D4");
        textColor.setAlphaF(1);
    } else {
        timeColor = QColor("#414D68");
        timeColor.setAlphaF(0.7);
        textColor = QColor("#414D68");
        textColor.setAlphaF(1);
    }
    update();
}

/**
 * @brief CenterWidget::setTimeFormat 设置日期显示格式
 * @param timeFormat 日期格式
 */
void CenterWidget::setTimeFormat(QString timeFormat)
{
    m_timeFormat = timeFormat;
    update();
}

void CenterWidget::UpdateTextList()
{
    testList.clear();
    QFontMetrics metrics(textfont);
    textwidth = metrics.width(m_ScheduleInfo.getTitleName());
    textheight = metrics.height();
    const int  h_count = qCeil(textwidth / textRectWidth);
    QString text;

    if (h_count < 1) {
        testList.append(m_ScheduleInfo.getTitleName());
    } else {
        const int text_Max_Height = 108;
        const int text_HeightMaxCount = qFloor(text_Max_Height / textheight);

        for (int i = 0; i < m_ScheduleInfo.getTitleName().count(); ++i) {
            text += m_ScheduleInfo.getTitleName().at(i);
            if (metrics.width(text) > textRectWidth) {
                text.remove(text.count() - 1, 1);
                testList.append(text);
                text = "";

                if (testList.count() == (text_HeightMaxCount - 1)) {
                    text = m_ScheduleInfo.getTitleName().right(m_ScheduleInfo.getTitleName().count() - i);
                    testList.append(metrics.elidedText(text, Qt::ElideRight, textRectWidth));
                    break;
                }
                --i;
            } else {
                if (i + 1 == m_ScheduleInfo.getTitleName().count()) {
                    testList.append(text);
                }
            }
        }
    }

    this->setFixedHeight(testList.count() * textheight + 30 + 8);
}

void CenterWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    int diam = 8;
    int x = 40 - 13;
    QFont timeFont;
    timeFont.setPixelSize(DDECalendar::FontSizeTwelve);
    QPainter painter(this);
    //draw time
    QPen pen;
    pen.setColor(timeColor);
    painter.setPen(pen);
    painter.setFont(timeFont);
    QString timestr;
    QLocale locale;
    timestr = m_ScheduleInfo.getBeginDateTime().time().toString("AP " + m_timeFormat);

    QFontMetrics metrics(timeFont);
    if (m_ScheduleInfo.getAllDay())
        timestr = tr("All Day");
    int timewidth = metrics.width(timestr);
    int timeheight = metrics.height();

    painter.drawText(QRect(x + 13, 7, timewidth, timeheight), Qt::AlignLeft | Qt::AlignTop, timestr);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(gdcolor.orginalColor));
    painter.drawEllipse(x, 7 + (timeheight - diam) / 2, diam, diam);
    pen.setColor(textColor);
    painter.setPen(pen);
    painter.setFont(textfont);

    for (int i = 0; i < testList.count(); i++) {
        painter.drawText(
            QRect(x, 30 + i * textheight, textRectWidth, textheight),
            Qt::AlignLeft, testList.at(i));
    }
}
