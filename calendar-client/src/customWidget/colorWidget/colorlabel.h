/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
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
#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <DLabel>
#include <DWidget>

#include <QPaintEvent>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QCursor>

DWIDGET_USE_NAMESPACE

class ColorLabel : public DLabel
{
    Q_OBJECT
public:
    explicit ColorLabel(DWidget *parent = nullptr);

    //h∈(0, 360), s∈(0, 1), v∈(0, 1)
    QColor getColor(qreal h, qreal s, qreal v);
    void setHue(int hue);

    /**
     * @brief pickColor 获取鼠标位置颜色
     * @param pos　位置
     * @param picked　是否获取
     */
    void pickColor(QPoint pos, bool picked = false);

signals:
    /**
     * @brief clicked　鼠标点击信号
     */
    void clicked();
    /**
     * @brief pickedColor　获取颜色信号
     * @param color　颜色
     */
    void signalpickedColor(QColor color);
    /**
     * @brief signalPreViewColor　颜色预览信号
     * @param color　颜色
     */
    void signalPreViewColor(QColor color);

protected:
    void paintEvent(QPaintEvent *);
    /**
     * @brief mousePressEvent　鼠标点击事件
     * @param e　鼠标事件
     */
    void mousePressEvent(QMouseEvent *e);
    /**
     * @brief mouseMoveEvent　鼠标移动事件
     * @param e　鼠标事件
     */
    void mouseMoveEvent(QMouseEvent *e);
    /**
     * @brief mouseReleaseEvent　鼠标弹起事件
     * @param e　鼠标事件
     */
    void mouseReleaseEvent(QMouseEvent *e);
private:
    /**
     * @brief pickColorCursor
     * @return 返回自定义光标
     */
    QCursor pickColorCursor();

private:
    QCursor m_dotCursor; // 圆点光标
    int m_hue =  0;
    bool m_pressed = false; //鼠标按下状态，用于判断是否发送颜色值
    bool m_entered = false; //鼠标移入状态，用于判断光标显示类型
    QColor m_pickedColor;
};

#endif // COLORLABEL_H
