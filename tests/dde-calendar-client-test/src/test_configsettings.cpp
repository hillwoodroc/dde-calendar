/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     hejinghai <hejinghai@uniontech.com>
*
* Maintainer: hejinghai <hejinghai@uniontech.com>
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
#include "test_configsettings.h"

test_configsettings::test_configsettings()
{
}

test_configsettings::~test_configsettings()
{
}

void test_configsettings::SetUp()
{
    conf = CConfigSettings::getInstance();
}

void test_configsettings::TearDown()
{

}

void stub_Handle()
{

}

void setValue_stub(void *obj, const QString &key, const QVariant &value)
{
    Q_UNUSED(obj)
        Q_UNUSED(key)
            Q_UNUSED(value)}

//void CConfigSettings::sync()
TEST_F(test_configsettings, sync)
{
    Stub stub;
    stub.set(ADDR(CConfigSettings, sync), stub_Handle);
    conf->sync();
}

TEST_F(test_configsettings, setOption)
{
    Stub stub;
    stub.set(ADDR(QSettings, setValue), setValue_stub);
    conf->setOption("base.state", 0);
}
