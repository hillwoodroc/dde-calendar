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
#ifndef SEMANTICANALYSISTASK_H
#define SEMANTICANALYSISTASK_H

#include <QObject>
#include <QDateTime>
#include "../data/jsondata.h"

class QJsonObject;

class semanticAnalysisTask : public QObject
{
    Q_OBJECT
public:
    explicit semanticAnalysisTask(QObject *parent = nullptr);
    ~semanticAnalysisTask();
    bool resolveTaskJson(const QString &semantic);
    QString Intent() const;
    void setIntent(const QString &Intent);
    JsonData *getJsonData() const;
    void setJsonData(JsonData *JsonData);

private:
    void deleteJsonData();
    JsonData *createJsonDataFactory(const QString &Intent);
    void setShouldEndSession(bool isEnd);

signals:

public slots:
private:
    QString m_Intent;
    JsonData *m_JsonData {nullptr};
};

#endif // SEMANTICANALYSISTASK_H
