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
#ifndef ICONDFRAME_H
#define ICONDFRAME_H

#include <DFrame>

class QSvgRenderer;
class Reply;
DWIDGET_USE_NAMESPACE
class IconDFrame : public DFrame
{
    Q_OBJECT
public:
    explicit IconDFrame(QWidget *parent = nullptr);
    ~IconDFrame() override;

    void setCenterLayout(QLayout *layout);

    QColor TitleColor() const;
    void setTitleColor(const QColor &TitleColor);

private:
    QFont TitleFont();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setTheMe(const int type);
    void paintPixmap(QPainter &painter);
    void paintTitle(QPainter &painter);
signals:
    void signaleSendMessage(QString text);

public slots:
    virtual void slotReceivce(QVariant data, Reply *reply);

private:
    QSvgRenderer *m_Dayrenderer;
    QSvgRenderer *m_Weekrenderer;
    QSvgRenderer *m_Monthrenderer;
    QSvgRenderer *m_backgroundrenderer;

    QColor m_TitleColor;
    QFont m_TitleFont;
};

#endif // ICONDFRAME_H
