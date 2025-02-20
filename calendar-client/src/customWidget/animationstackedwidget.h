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
#ifndef AnimationStackedWidget_H
#define AnimationStackedWidget_H

#include <QStackedWidget>
#include <QStackedLayout>
#include <QPropertyAnimation>

/**
 * @brief The AnimationStackedWidget class
 * 动画切换界面
 */
class AnimationStackedWidget : public QStackedWidget
{
    Q_OBJECT
    Q_PROPERTY(double offset READ offset WRITE setOffset)
public:
    enum MoveOrientation {LeftToRight, RightToLeft, TopToBottom, BottomToTop};
    enum AnimationOri {LR, TB}; //LR:Left Right  ,TB:Top  Bottom
public:
    explicit AnimationStackedWidget(const AnimationOri ori = LR, QWidget *parent = nullptr);
    ~AnimationStackedWidget() override;
    //设置动画持续时间
    void setDuration(int duration = 500);
    /**
     * @brief IsRunning 动画是否正在运行
     * @return  动画师傅正在运行
     */
    bool IsRunning()const
    {
        return m_IsAnimation;
    }
    //获取移动偏移量
    double offset() const;
    //设置移动偏移量
    void setOffset(double offset);
protected:
    //窗口绘制事件
    void paintEvent(QPaintEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
signals:
    /**
     * @brief signalIsFinished     动画结束信号
     */
    void signalIsFinished();
public slots:
    //动画切换完成
    void animationFinished();
    //设置当前窗口
    void setCurrent(int index);
    //setPre设置上一个索引的窗口
    void setPre();
    //设置下一个索引的窗口
    void setNext();
private:
    //根据索引设置当前窗口，开启动画绘制
    void setCurrentWidget(int &index, int beginWidth);
    //获取动画起始值
    int getBeginValue();
    //根据当前索引和下一个索引获取动画方向
    MoveOrientation getMoveOrientation(const int currIndex, const int nextIndex);
    //获取当前窗口的图片动态绘制
    void paintCurrentWidget(QPainter &paint, int currentIndex);
    //获取下一个窗口的图片动态绘制
    void paintNextWidget(QPainter &paint, int nextIndex);
private:
    /**
     * @brief m_animationOri 动画方向--左右还是上下
     */
    AnimationOri m_animationOri{LR};
    /**
     * @brief m_Animation 动画对象
     */
    QPropertyAnimation *m_Animation = nullptr;
    /**
     * @brief m_Duration 动画时间(ms)
     */
    int m_Duration{0};
    /**
     * @brief m_IsAnimation 是否为动画状态
     */
    bool m_IsAnimation = false;
    /**
     * @brief m_offset 移动偏移量
     */
    double m_offset = 0;
    /**
     * @brief paintNext
     */
    int m_WidgetCount{0};
    /**
     * @brief m_NextIndex 下一个索引
     */
    int m_NextIndex{0};
    /**
     * @brief m_moveOri 移动方向
     */
    MoveOrientation m_moveOri = MoveOrientation::LeftToRight;
    int m_nextGotoIndex = -1; //在动画过程中，下次需要跳转的页面
};

#endif // AnimationStackedWidget_H
