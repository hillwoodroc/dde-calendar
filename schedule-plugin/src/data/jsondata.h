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
#ifndef JSONDATA_H
#define JSONDATA_H

#include <QObject>
#include <QVector>
#include "timedatastruct.h"
class QJsonObject;
class JsonData
{
public:
    enum RepeatStatus { NONE,
                        EVED,
                        EVEW,
                        EVEM,
                        EVEY,
                        WORKD,
                        RESTD
    };
    enum DateTimeStatus { NODATE,
                          NOTIME,
                          ALLHAS
    };
    enum PropertyStatus { PRO_NONE,
                          NEXT,
                          LAST,
                          ALL,
                          PRO_THIS
    };

public:
    JsonData();
    virtual ~JsonData();
    void JosnResolve(const QJsonObject &jsObj);
    QString SuggestMsg() const;
    void setSuggestMsg(const QString &SuggestMsg);

    bool ShouldEndSession() const;
    void setShouldEndSession(bool ShouldEndSession);

    void setRepeatStatus(const RepeatStatus &RepeatStatus);

    QString TitleName() const;
    void setTitleName(const QString &TitleName);

    void setDateTimeStatus(const DateTimeStatus &DateTimeStatus);

    QVector<DateTimeInfo> DateTime() const;

    RepeatStatus getRepeatStatus() const;

    QVector<int> getRepeatNum() const;
    void setRepeatNum(const QVector<int> &RepeatNum);

    QVector<DateTimeInfo> getDateTime() const;
    void setDateTime(const QVector<DateTimeInfo> &DateTime);

    PropertyStatus getPropertyStatus() const;
    void setPropertyStatus(const PropertyStatus &propertyStatus);

    int offset() const;
    void setOffset(int offset);

    bool isVaild() const;

protected:
    void contentJsonResolve(const QJsonObject &jsobj);
    void repeatJsonResolve(const QJsonObject &jsobj);
    void datetimeJsonResolve(const QJsonObject &jsobj);
    void propertyJsonResolve(const QJsonObject &jsobj);
    void posRankOffsetResolve(const QJsonObject &jsobj);
    QVector<DateTimeInfo> suggestDatetimeResolve(const QJsonObject &jsobj);
    DateTimeInfo resolveNormValue(const QString &str);
    virtual void setDefaultValue();
    virtual void jsonObjResolve(const QJsonObject &jsobj);

private:
    QString m_SuggestMsg {""};
    bool m_ShouldEndSession {false};
    RepeatStatus m_RepeatStatus {NONE};
    QVector<int> m_RepeatNum {};
    DateTimeStatus m_DateTimeStatus {NODATE};
    PropertyStatus m_propertyStatus {PRO_NONE};
    QString m_TitleName {""};
    QVector<DateTimeInfo> m_DateTime {};
    int m_offset {-1};
};

#endif // JSONDATA_H
