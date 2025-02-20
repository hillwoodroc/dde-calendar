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
#include "cfocusitem.h"

#include "scheduledatamanage.h"

#include <QGraphicsScene>
#include <QPainter>

CFocusItem::CFocusItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_NextFocusItem(nullptr)
    , m_itemType(CITEM)
    , m_isFocus(false)
{
}

/**
 * @brief CFocusItem::setNextFocusItem  设置下一个FocusItem
 * @param nextFocusItem
 */
void CFocusItem::setNextFocusItem(CFocusItem *nextFocusItem)
{
    m_NextFocusItem = nextFocusItem;
}

/**
 * @brief CFocusItem::setItemFocus  设置item是否获取focus
 * @param isFocus
 */
void CFocusItem::setItemFocus(bool isFocus)
{
    m_isFocus = isFocus;
    this->scene()->update();
}

/**
 * @brief CFocusItem::getItemFocus      获取该item是否focus
 * @return
 */
bool CFocusItem::getItemFocus() const
{
    return m_isFocus;
}

/**
 * @brief CFocusItem::setItemType 设置item类型
 * @param itemType
 */
void CFocusItem::setItemType(CFocusItem::CItemType itemType)
{
    m_itemType = itemType;
}

/**
 * @brief CFocusItem::getItemType 获取item类型
 * @return
 */
CFocusItem::CItemType CFocusItem::getItemType() const
{
    return m_itemType;
}

/**
 * @brief CFocusItem::setNextItemFocusAndGetNextItem    设置下一个item focus状态并获取下一个Item
 * @return
 */
CFocusItem *CFocusItem::setNextItemFocusAndGetNextItem()
{
    if (m_NextFocusItem != nullptr) {
        m_isFocus = false;
        m_NextFocusItem->setItemFocus(true);
    }
    return m_NextFocusItem;
}

/**
 * @brief CFocusItem::getSystemActiveColor      获取系统活动色
 * @return
 */
QColor CFocusItem::getSystemActiveColor()
{
    return CScheduleDataManage::getScheduleDataManage()->getSystemActiveColor();
}

void CFocusItem::setDate(const QDate &date)
{
    m_Date = date;
}
