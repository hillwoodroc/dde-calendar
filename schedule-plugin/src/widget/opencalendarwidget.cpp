/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     uniontech  <uniontech@uniontech.com>
*
* Maintainer: uniontech  <chenhaifeng@uniontech.com>
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
#include "opencalendarwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QProcess>

#include "../globaldef.h"

#include <DApplicationHelper>

OpenCalendarWidget::OpenCalendarWidget(QWidget *parent)
    : DWidget(parent)
{
    setFixedHeight(OPENCALENDAR_WDIGET_HEIGHT);
}

void OpenCalendarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    setTheMe(DApplicationHelper::instance()->themeType());
    QPainter painter(this);
    painter.setPen(TitleColor());
    painter.setFont(TitleFont());
    painter.drawText(this->rect(),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     QString(OPENCALENDAR_WIDGET_TITLE).arg(m_ScheduleCount));
}

void OpenCalendarWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QProcess proc;
        proc.startDetached(PROCESS_OPEN_CALENDAR);
    }
}

QColor OpenCalendarWidget::TitleColor() const
{
    return m_TitleColor;
}

void OpenCalendarWidget::setTitleColor(const QColor &TitleColor)
{
    m_TitleColor = TitleColor;
}

QFont OpenCalendarWidget::TitleFont()
{
    m_TitleFont.setPixelSize(12);
    return m_TitleFont;
}

void OpenCalendarWidget::setTheMe(const int type)
{
    QColor titleColor;
    if (type == 0 || type == 1) {
        titleColor.setRgb(0, 0, 0);
        titleColor.setAlphaF(0.4);
    } else {
        titleColor.setRgb(255, 255, 255);
        titleColor.setAlphaF(0.4);
    }
    setTitleColor(titleColor);
}

void OpenCalendarWidget::setScheduleCount(int ScheduleCount)
{
    m_ScheduleCount = ScheduleCount;
    update();
}
