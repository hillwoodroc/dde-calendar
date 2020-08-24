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
#include "icondframe.h"
#include <QPainter>
#include <QDate>
#include <QSvgRenderer>
#include <QVBoxLayout>

#include <DApplicationHelper>

#include "../globaldef.h"
#include "../interface/reply.h"

IconDFrame::IconDFrame(QWidget *parent)
    : DFrame(parent)
    , m_Dayrenderer(new QSvgRenderer())
    , m_Weekrenderer(new QSvgRenderer())
    , m_Monthrenderer(new QSvgRenderer())
    , m_backgroundrenderer(new QSvgRenderer())
{
}

IconDFrame::~IconDFrame()
{
    delete m_Dayrenderer;
    m_Dayrenderer = nullptr;

    delete m_Weekrenderer;
    m_Weekrenderer = nullptr;

    delete m_Monthrenderer;
    m_Monthrenderer = nullptr;

    delete m_backgroundrenderer;
    m_backgroundrenderer = nullptr;
}

void IconDFrame::setCenterLayout(QLayout *layout)
{
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addSpacing(25);
    vlayout->addLayout(layout);
    this->setLayout(vlayout);
}

void IconDFrame::paintEvent(QPaintEvent *event)
{
    setTheMe(DApplicationHelper::instance()->themeType());
    DFrame::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 反锯齿;
    paintPixmap(painter);
    paintTitle(painter);
}

void IconDFrame::setTheMe(const int type)
{
    QColor titleColor;
    if (type == 0 || type == 1) {
        titleColor.setRgb(0, 0, 0);
        titleColor.setAlphaF(0.4);
    } else {
        titleColor.setRgb(255, 255, 255);
        titleColor.setAlphaF(0.4);
    }
    setTitleColor(titleColor);
}

void IconDFrame::paintPixmap(QPainter &painter)
{
    QString dayfile = QString(":/resources/DynamicIcon/day%1.svg").arg(QDate::currentDate().day());

    m_Dayrenderer->load(dayfile);

    QString weekfile = QString(":/resources/DynamicIcon/week%1.svg").arg(QDate::currentDate().dayOfWeek());
    m_Weekrenderer->load(weekfile);

    QString monthfile = QString(":/resources/DynamicIcon/month%1.svg").arg(QDate::currentDate().month());
    m_Monthrenderer->load(monthfile);

    QString backgroundfile(":/resources/DynamicIcon/calendar_bg.svg");
    m_backgroundrenderer->load(backgroundfile);

    painter.save();
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    //draw background
    QRectF backgroundRect = QRectF(11,
                                   9,
                                   16,
                                   16);
    m_backgroundrenderer->render(&painter, backgroundRect);

    //draw month
    qreal month_x = 13;
    qreal month_y = 10;
    qreal month_w = 11;
    qreal month_h = 4;
    m_Monthrenderer->render(&painter, QRectF(month_x, month_y, month_w, month_h));

    //draw day
    qreal day_x = 14;
    qreal day_y = 14;
    qreal day_w = 9;
    qreal day_h = 7;
    m_Dayrenderer->render(&painter, QRectF(day_x, day_y, day_w, day_h));
    painter.restore();
}

void IconDFrame::paintTitle(QPainter &painter)
{
    QRect rect(34, 7, 24, 17);
    painter.save();
    painter.setPen(TitleColor());
    painter.setFont(TitleFont());
    painter.drawText(rect, Qt::AlignCenter, tr(PLUGIN_TITLE_NAME));
    painter.restore();
}

void IconDFrame::slotReceivce(QVariant data, Reply *reply)
{
    Q_UNUSED(data);
    Q_UNUSED(reply);
}

QColor IconDFrame::TitleColor() const
{
    return m_TitleColor;
}

void IconDFrame::setTitleColor(const QColor &TitleColor)
{
    m_TitleColor = TitleColor;
}

QFont IconDFrame::TitleFont()
{
    m_TitleFont.setPixelSize(12);
    return m_TitleFont;
}
