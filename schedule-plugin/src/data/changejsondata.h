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
#ifndef CHANGEJSONDATA_H
#define CHANGEJSONDATA_H

#include "jsondata.h"

class changejsondata : public JsonData
{
public:
    changejsondata();
    QVector<DateTimeInfo> fromDateTime() const;
    void setFromDateTime(const QVector<DateTimeInfo> &fromDateTime);

    QVector<DateTimeInfo> toDateTime() const;
    void setToDateTime(const QVector<DateTimeInfo> &toDateTime);

    QString toPlaceStr() const;
    void setToPlaceStr(const QString &toPlaceStr);

private:
    void jsonObjResolve(const QJsonObject &jsobj) override;
    void setDefaultValue() override;
    void fromDatetimeJsonResolve(const QJsonObject &jsobj);
    void toDatetimeJsonResolve(const QJsonObject &jsobj);
    void toPlaceJsonResolve(const QJsonObject &jsobj);

private:
    QVector<DateTimeInfo> m_fromDateTime {};
    QVector<DateTimeInfo> m_toDateTime {};
    QString m_toPlaceStr {};
};

#endif // CHANGEJSONDATA_H
