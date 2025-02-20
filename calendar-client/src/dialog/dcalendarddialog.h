/*
   * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     chenhaifeng <chenhaifeng@uniontech.com>
   *
   * Maintainer: chenhaifeng <chenhaifeng@uniontech.com>
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
#ifndef DCALENDARDDIALOG_H
#define DCALENDARDDIALOG_H

#include <DDialog>

DWIDGET_USE_NAMESPACE

class DCalendarDDialog : public DDialog
{
    Q_OBJECT
public:
    explicit DCalendarDDialog(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;
    virtual void updateDateTimeFormat();
signals:

public slots:
    void setTimeFormat(int value);
    void setDateFormat(int value);
protected:
    QString m_timeFormat = "hh:mm";
    QString m_dateFormat = "yyyy-MM-dd";
};

#endif // DCALENDARDDIALOG_H
