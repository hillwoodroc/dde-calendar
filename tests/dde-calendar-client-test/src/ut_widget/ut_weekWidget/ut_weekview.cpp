/*
* Copyright (C) 2019 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhengxiaokang <zhengxiaokang@uniontech.com>
*
* Maintainer: zhengxiaokang <zhengxiaokang@uniontech.com>
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
#include "ut_weekview.h"

int getWeekNumOfYear(const QDate &date)
{
    Q_UNUSED(date);
    return 4;
}

ut_weekview::ut_weekview()
{
    mWeekView = new CWeekView(&getWeekNumOfYear);
}

ut_weekview::~ut_weekview()
{
    delete mWeekView;
    mWeekView = nullptr;
}

//void CWeekView::setSelectDate(const QDate date)
TEST_F(ut_weekview, setSelectDate)
{
    mWeekView->setSelectDate(QDate::currentDate());
    ASSERT_EQ(mWeekView->m_weekNumWidget->m_selectDate, QDate::currentDate());
}

//void CWeekView::setCurrent(const QDateTime &dateTime)
TEST_F(ut_weekview, setCurrent)
{
    mWeekView->setCurrent(QDateTime::currentDateTime());
    ASSERT_EQ(mWeekView->m_weekNumWidget->m_currentDate, QDateTime::currentDateTime());
}

//void CWeekView::setTheMe(int type)
TEST_F(ut_weekview, setTheMe)
{
    mWeekView->setTheMe(1);
    ASSERT_EQ(mWeekView->m_weekNumWidget->m_currentDayTextColor, Qt::white);
    mWeekView->setTheMe(2);
    ASSERT_EQ(QString::number(mWeekView->m_weekNumWidget->m_backgrounddefaultColor.alphaF(), 'f', 2), "0.05");
}

TEST_F(ut_weekview, getPixmap)
{
    mWeekView->setSelectDate(QDate::currentDate());
    mWeekView->setFixedSize(600, 50);
    QPixmap pixmap(mWeekView->size());
    mWeekView->render(&pixmap);
    ASSERT_EQ(pixmap.size(), mWeekView->size());
}
