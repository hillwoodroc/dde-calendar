/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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
#include "timeverticalscroll.h"
//#include "ui_CTimeVerticalScroll.h"
#include <QMouseEvent>
#include <QDebug>
#include <DPalette>
#include <QPainterPath>
#include <QWheelEvent>
#include "scheduledatamanage.h"
DGUI_USE_NAMESPACE
CTimeVerticalScroll::CTimeVerticalScroll(QWidget *parent): DWidget(parent)
{
    //DPalette palette(this->palette());

    // palette.setColor(DPalette::Window, Qt::white);

    // this->setPalette(palette);
//     ui->setupUi(this);
//    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint/* | Qt::WindowStaysOnTopHint*/);
    setContentsMargins(0, 0, 0, 0);
    m_minRange = 0;    //最小值默认为0
    m_maxRange = 59;   //最大值默认59
    m_currentValue = 15; //当前值默认0
    isDragging = false;
    m_bClicked = false;
    m_deviation = 0;     //默认偏移量为0
    m_numSize = 15;
    interval = 1;     //间隔默认1
    devide = 7;           //默认分成9格

    homingAni = new QPropertyAnimation(this, "deviation");
    homingAni->setDuration(300);
    homingAni->setEasingCurve(QEasingCurve::OutQuad);

    //resize(50, 240);
    m_currentY = this->height() / 2;
    m_Devideheight = this->height() / devide;
}

/*
 * 设置范围
 * double min 最小值
 * double max 最大值
*/
void CTimeVerticalScroll::setRange(double min, double max)
{
    m_minRange = min;
    m_maxRange = max;
    if (m_currentValue < min) {
        m_currentValue = min;
    }
    if (m_currentValue > max) {
        m_currentValue = max;
    }
    repaint();
}
//获取当前值
int CTimeVerticalScroll::readValue()
{
    return m_currentValue;
}
//设置当前值
void CTimeVerticalScroll::setCurrValue(int value)
{
    m_currentValue = value;
    homing();
    int w = width();
    repaint();
}

void CTimeVerticalScroll::setmZeropadd(bool flag)
{
    m_Zeropadd = flag;
}

void CTimeVerticalScroll::setPadingStr(QString str)
{
    m_PadingStr = str;
}

void CTimeVerticalScroll::mousePressEvent(QMouseEvent *e)
{
//    qDebug()<<"mouse pressed on vertical scroll";

    homingAni->stop();
    //isDragging = true;
    m_mouseSrcPos = e->pos().y();
    QWidget::mousePressEvent(e);
}

void CTimeVerticalScroll::mouseMoveEvent(QMouseEvent *e)
{
    if (isDragging) {
        if ((m_currentValue == m_minRange && e->pos().y() >= m_mouseSrcPos) ||
                ( m_currentValue == m_maxRange && e->pos().y() <= m_mouseSrcPos )) {
//            m_currentY += e->pos().y() - m_mouseSrcPos;
            return;
        }

        m_deviation = e->pos().y() - m_mouseSrcPos;
        //若移动速度过快，则进行限制
        if (m_deviation > (height() - 1) / devide) {
            m_deviation = (height() - 1) / devide;
        } else if (m_deviation < -(height() - 1) / devide) {
            m_deviation = -( height() - 1) / devide;
        }

        repaint();
    }
}

void CTimeVerticalScroll::mouseReleaseEvent(QMouseEvent *e)
{
    if (isDragging) {
        isDragging = false;
        if (e->pos().y() == m_mouseSrcPos) {
            m_bClicked = true;
            m_deviation = m_currentY - e->pos().y();
        }
        homing();
        emit currentValueChangedClose(m_currentValue);
        repaint();
    }
    if (e->pos().y() == m_mouseSrcPos) {
        m_bClicked = true;
        m_deviation = m_currentY - e->pos().y();
        homing();
        emit currentValueChangedClose(m_currentValue);
        repaint();
    }

    m_bClicked = false;
}

void CTimeVerticalScroll::wheelEvent(QWheelEvent *e)
{
    if (e->delta() > 0) {
        m_deviation = (this->height() - 1) / devide;
    } else {
        m_deviation = -(this->height() - 1) / devide;
    }
    homing();
    repaint();
    emit currentValueChanged(m_currentValue);
}

void CTimeVerticalScroll::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    int Height = height() - 1;

    if ( m_deviation >= Height / devide && m_currentValue > m_minRange ) {
        m_mouseSrcPos += Height / devide;
        m_deviation -= Height / devide;
        m_currentValue -= interval;
    }

    if ( m_deviation <= -Height / devide && m_currentValue < m_maxRange ) {
        m_mouseSrcPos -= Height / devide;
        m_deviation += Height / devide;
        m_currentValue += interval;
    }
    //emit currentValueChanged(m_currentValue);
//    qDebug()<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<QString::number(m_currentValue);

    //middle number
    paintNum(painter, m_currentValue, m_deviation);

    int out = -1;
    //两侧数字1
    if (m_currentValue != m_minRange) {
        paintNum(painter, m_currentValue - interval, m_deviation - Height / devide);
    } else {
        paintNum(painter, m_maxRange, m_deviation - Height / devide);
        out = 0;
    }

    if (m_currentValue != m_maxRange) {
        paintNum(painter, m_currentValue + interval, m_deviation + Height / devide);
    } else {
        paintNum(painter, m_minRange, m_deviation + Height / devide);
        out = 0;
    }
    if (out > -1)
        out++;

    for (int i = 2; i <= devide / 2; ++i) {
        if (m_currentValue - interval * i >= m_minRange) {
            paintNum(painter, m_currentValue - interval * i, m_deviation - Height / devide * i);
        } else {
            if (out < 0)
                out = 0;
            paintNum(painter, m_maxRange - out * interval, m_deviation - Height / devide * i);
            out++;
        }

        if (m_currentValue + interval * i <= m_maxRange) {
            paintNum(painter, m_currentValue + interval * i, m_deviation + Height / devide * i);
        } else {
            if (out < 0)
                out = 0;
            paintNum(painter, m_minRange + interval * out, m_deviation + Height / devide * i);
            out++;
        }
    }
}
/*
 * 根据偏移量描绘数字
 * double num 需要显示的数字
 * int deviation 数字相对中间的偏移量
*/
void CTimeVerticalScroll::paintNum(QPainter &painter, int num, int deviation)
{
    int Width = width() - 1;
    int Height = height() - 1;
    int size = (Height /*- qAbs(deviation)*/) / m_numSize; //偏移量越大，数字越小
    int transparency = 255 - 255 * qAbs(deviation) / Height;
    int height = Height / devide;
    int y = Height / 2 + deviation - height / 2;

    QFont font;
    font.setPixelSize(size);
    if (num == m_currentValue)
        font.setBold(true);
    else
        font.setBold(false);
    painter.setFont(font);
    int type = CScheduleDataManage::getScheduleDataManage()->getTheme();
    if (type == 0 || type == 1) {
        painter.setPen(QColor(0, 0, 0, transparency));

    } else {
        painter.setPen(QColor("#C0C6D4"));

    }

    if ( y >= 0 && y + height < Height) {
        if (deviation == 0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor("#0887FF"));
            painter.drawRect(0, y, Width - 15, height);
            painter.setPen(QColor("#FFFFFF"));
            if (m_Zeropadd && num < 10) {
                painter.drawText(QRectF(0, y, Width - 20, height),
                                 Qt::AlignCenter,
                                 "0" + QString::number(num) + m_PadingStr);
            } else {
                painter.drawText(QRectF(0, y, Width - 20, height),
                                 Qt::AlignCenter,
                                 QString::number(num) + m_PadingStr);
            }

        } else {
            if (m_Zeropadd && num < 10) {
                painter.drawText(QRectF(0, y, Width - 20, height),
                                 Qt::AlignCenter,
                                 "0" + QString::number(num) + m_PadingStr);
            } else {
                painter.drawText(QRectF(0, y, Width - 20, height),
                                 Qt::AlignCenter,
                                 QString::number(num) + m_PadingStr);
            }

        }
    }
}

/*
 * 使选中的数字回到屏幕中间
*/
void CTimeVerticalScroll::homing()
{
    if (m_bClicked) {
        if ( m_deviation > 0.5 * m_Devideheight) {
            homingAni->setStartValue( ( height() - 1 ) / devide + 1);
            homingAni->setEndValue(0);
//            if(m_currentValue == m_minRange)
//                m_currentValue = m_maxRange;
//            else
            {
                int tmpInterval = m_deviation / m_Devideheight;
                if (m_Devideheight * tmpInterval < m_deviation) {
                    if (m_deviation - m_Devideheight * tmpInterval > 0.5 * m_Devideheight)
                        tmpInterval += 1;
                }
                m_currentValue -= tmpInterval;
                if (m_currentValue < m_minRange) {
                    m_currentValue += m_maxRange - m_minRange + 1;
                }
                m_deviation = 0;
            }
        } else if ( qAbs(m_deviation) <= 0.5 * m_Devideheight) {
            m_deviation = 0;
            return;
        } else if ( m_deviation < -0.5 * m_Devideheight ) {
            homingAni->setStartValue(-(height() - 1) / devide);
            homingAni->setEndValue(0);
//            if(m_currentValue == m_maxRange)
//                m_currentValue = m_minRange;
//            else
            {
                int tmpInterval = -m_deviation / m_Devideheight;
                if (m_Devideheight * tmpInterval < -m_deviation) {
                    if (-m_deviation - m_Devideheight * tmpInterval >= m_Devideheight * 0.5)
                        tmpInterval += 1;
                }
                m_currentValue += tmpInterval;
                if (m_currentValue > m_maxRange) {
                    m_currentValue -= m_maxRange - m_minRange + 1;
                }
                m_deviation = 0/*m_Devideheight-1*/;
//                qDebug()<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<QString::number(m_currentValue);
            }
        }
    } else {
        if ( m_deviation > height() / 10) {
            homingAni->setStartValue( ( height() - 1 ) / 8 - m_deviation);
            homingAni->setEndValue(0);
            if (m_currentValue == m_minRange)
                m_currentValue = m_maxRange;
            else
                m_currentValue -= interval;
//            qDebug()<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<QString::number(m_currentValue);
        } else if ( m_deviation > -height() / 10 ) {
            homingAni->setStartValue(m_deviation);
            homingAni->setEndValue(0);
        } else if ( m_deviation < -height() / 10 ) {
            homingAni->setStartValue(-(height() - 1) / 8 - m_deviation);
            homingAni->setEndValue(0);
            if (m_currentValue == m_maxRange)
                m_currentValue = m_minRange;
            else
                m_currentValue += interval;
//            qDebug()<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<QString::number(m_currentValue);
        }
    }

    homingAni->start();
}

int CTimeVerticalScroll::readDeviation()
{
    return m_deviation;
}

void CTimeVerticalScroll::setDeviation(int n)
{
    m_deviation = n;
    repaint();
}

void CTimeVerticalScroll::resizeEvent(QResizeEvent *e)
{
    m_currentY = this->height() / 2;
    m_Devideheight = this->height() / devide;
    QWidget::resizeEvent(e);
}
