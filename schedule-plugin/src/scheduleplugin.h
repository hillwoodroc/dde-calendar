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
#ifndef SCHEDULEPLUGIN_H
#define SCHEDULEPLUGIN_H

#include "interface/systemsemanticplugin.h"
#include "interface/reply.h"

#include <QMutex>

class Scheduleplugin : public QObject
    , public IServicePlugin
{
    Q_OBJECT
    Q_INTERFACES(IServicePlugin)
    Q_PLUGIN_METADATA(IID PluginInterface_iid)
public:
    explicit Scheduleplugin(QObject *parent = nullptr);
    ~Scheduleplugin() override;

    int init() override;
    void uninit() override;

    QStringList getSupportService() override;
    IService *createService(const QString &service) override;
    void releaseService(IService *service) override;
    bool needRunInDifferentThread() override;
    bool isAutoDetectSession() override;
public slots:
    void slotSendMessage(Reply reply);

private:
    QSet<IService *> m_ServiceSet;
    QMutex m_ServiceLock;
};

#endif // SCHEDULEPLUGIN_H
