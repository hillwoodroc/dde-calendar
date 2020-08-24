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
#ifndef OPENCALENDARWIDGET_H
#define OPENCALENDARWIDGET_H

#include <DWidget>
#include <QColor>
#include <QFont>

DWIDGET_USE_NAMESPACE
class OpenCalendarWidget : public DWidget
{
    Q_OBJECT
public:
    explicit OpenCalendarWidget(QWidget *parent = nullptr);

    void setScheduleCount(int ScheduleCount);

    QFont TitleFont();
    QColor TitleColor() const;
    void setTitleColor(const QColor &TitleColor);

private:
    void setTheMe(const int type);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
signals:

public slots:
private:
    int m_ScheduleCount;
    QColor m_TitleColor;
    QFont m_TitleFont;
};

#endif // OPENCALENDARWIDGET_H
