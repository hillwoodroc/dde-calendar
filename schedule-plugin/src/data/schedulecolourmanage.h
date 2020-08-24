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
#ifndef SCHEDULECOLOURMANAGE_H
#define SCHEDULECOLOURMANAGE_H

#include <QColor>
struct CSchedulesColor {
    int type;
    QColor gradientFromC;
    QColor gradientToC;
    QColor dotColor;
    QColor Purecolor;
    QColor shadowcolor;
    QColor textColor;
    QColor timeColor;
    QColor hightlightgradientFromC;
    QColor hightlightgradientToC;
    QColor hightlightPurecolor;
    QColor hovergradientFromC;
    QColor hovergradientToC;
    QColor hoverPurecolor;
    QColor splitColor;
    QColor pressgradientFromC;
    QColor pressgradientToC;
    QColor pressPurecolor;
};

class ScheduleColourManage
{
public:
    ScheduleColourManage();
    ~ScheduleColourManage();

public:
    void setTheMe(int type = 0);
    CSchedulesColor getColorByTypeId(const int &id);

private:
    CSchedulesColor LifeScheduleColour() const;

    CSchedulesColor WorkScheduleColour() const;

    CSchedulesColor OtherScheduleColour() const;

    CSchedulesColor SolarScheduleColour() const;

private:
    void setLifeScheduleTheme(int type = 0);
    void setWorkScheduleTheme(int type = 0);
    void setOtherScheduleTheme(int type = 0);
    void setSolarScheduleTheme(int type = 0);

private:
    CSchedulesColor *m_LifeScheduleColour {nullptr};
    CSchedulesColor *m_WorkScheduleColour {nullptr};
    CSchedulesColor *m_OtherScheduleColour {nullptr};
    CSchedulesColor *m_SolarScheduleColour {nullptr};
};

#endif // SCHEDULECOLOURMANAGE_H
