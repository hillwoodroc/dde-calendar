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

#include "monthdayview.h"
#include <QHBoxLayout>
#include <QPainterPath>
#include "constants.h"
#include <QPainter>
#include <QBrush>
#include <QEvent>
#include <DPalette>
#include <DHiDPIHelper>
DGUI_USE_NAMESPACE
CMonthDayView::CMonthDayView(QWidget *parent) : DFrame(parent)
{
    m_dayNumFont.setFamily("Avenir-Light");
    m_dayNumFont.setPixelSize(16);
    m_dayNumFont.setWeight(QFont::Light);


    // cells grid
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setMargin(0);
    hboxLayout->setSpacing(0);
    hboxLayout->setContentsMargins(10, 0, 10, 0);
    for (int c = 0; c != 12; ++c) {
        QWidget *cell = new QWidget;
        cell->setFixedSize(DDEMonthCalendar::MDayCellWidth, DDEMonthCalendar::MDayCellHeight);
        cell->installEventFilter(this);
        cell->setFocusPolicy(Qt::ClickFocus);
        hboxLayout->addWidget(cell);
        m_cellList.append(cell);
    }
    setLayout(hboxLayout);
    setFrameRounded(true);
    setLineWidth(0);
    setWindowFlags(Qt::FramelessWindowHint);
}

CMonthDayView::~CMonthDayView()
{
    for (int i = 0; i < 12; i++) {
        m_cellList.at(i)->removeEventFilter(this);
        delete m_cellList.at(i);
    }
    m_cellList.clear();
}

void CMonthDayView::setCurrentDate(const QDate date)
{
#if 0
    m_selectDate = date;
    for (int i = 0; i < 12; i++) {
        if (QDate(date.year(), i + 1, date.day()).isValid()) {
            m_days[i] = QDate(date.year(), i + 1, date.day());
        } else {
            m_days[i] = QDate(date.year(), i + 1, 1);
        }
    }
    setSelectedCell(m_selectDate.month() - 1);
    update();
    emit signalsCurrentDate(date);
#endif
    m_selectDate = date;
    m_days[5] = m_selectDate;
    for (int i(4); i >= 0; i--) {
        m_days[4 - i] = m_selectDate.addMonths(-i - 1);
    }
    for (int i(6); i != 12; ++i) {
        m_days[i] = m_selectDate.addMonths(i - 5);
    }
    setSelectedCell(5);
    update();
    emit signalsCurrentDate(date);
}

void CMonthDayView::setRCurrentDate(const QDate date)
{
    if (m_selectDate == date) return;
#if 0
    m_selectDate = date;
    for (int i = 0; i < 12; i++) {
        if (QDate(date.year(), i + 1, date.day()).isValid()) {
            m_days[i] = QDate(date.year(), i + 1, date.day());
        } else {
            m_days[i] = QDate(date.year(), i + 1, 1);
        }
    }
    setSelectedCell(m_selectDate.month() - 1, 1);
    update();
#endif
    m_selectDate = date;
    m_days[5] = m_selectDate;
    for (int i(4); i >= 0; i--) {
        m_days[4 - i] = m_selectDate.addMonths(-i - 1);
    }
    for (int i(6); i != 12; ++i) {
        m_days[i] = m_selectDate.addMonths(i - 5);
    }
    setSelectedCell(5);
    update();
}

void CMonthDayView::setTheMe(int type)
{
    m_themetype = type;
    QColor frameclor;
    if (type == 0 || type == 1) {

        m_defaultTextColor = Qt::black;
        m_backgrounddefaultColor = Qt::white;
        m_currentDayTextColor = Qt::white;
        m_backgroundcurrentDayColor = "#0081FF";
        m_fillColor = "#FFFFFF";
        frameclor = m_fillColor;
        m_fillColor.setAlphaF(0);

    } else if (type == 2) {
        m_defaultTextColor = "#C0C6D4";
        QColor framecolor = Qt::black;
        framecolor.setAlphaF(0.5);
        m_backgrounddefaultColor = framecolor;
        m_currentDayTextColor = "#C0C6D4";
        m_backgroundcurrentDayColor = "#0059D2";
        m_fillColor = "#FFFFFF";
        m_fillColor.setAlphaF(0.05);
        frameclor = m_fillColor;
        m_fillColor.setAlphaF(0);
    }

    DPalette anipa = palette();
    anipa.setColor(DPalette::Background, frameclor);
    setPalette(anipa);
    setBackgroundRole(DPalette::Background);
}

void CMonthDayView::setwindowFixw(int w, int rw)
{
    m_fixwidth = w;
    m_realwidth = rw;
    int w2 = m_fixwidth * 0.08333 + 0.5;
    int h = height();
    for (int c = 0; c != 12; ++c) {
        m_cellList[c]->setFixedSize(w2, h);
        m_cellList[c]->update();
    }
    if ((m_realwidth   < m_fixwidth) && m_searchfalg) {
        int t_num = qRound((m_fixwidth - m_realwidth ) / w2 / 2.0);
        QVector<bool> vindex;
        vindex.resize(12);
        vindex.fill(true);
        for (int i = 0; i < t_num; i++) {
            vindex[i] = false;
            vindex[11 - i] = false;
        }
        for (int i = 0; i < 12; i++) {
            m_cellList[i]->setVisible(vindex[i]);
            m_cellList[i]->update();
        }
    } else {
        for (int i = 0; i < 12; i++) {
            m_cellList[i]->setVisible(true);
            m_cellList[i]->update();
        }
    }
}

void CMonthDayView::setsearchfalg(bool flag)
{
    m_searchfalg = flag;
}

void CMonthDayView::paintCell(QWidget *cell)
{
    const QRect rect(0, 0, cell->width(), cell->height());

    const int pos = m_cellList.indexOf(cell);
    const bool isCurrentDay = (m_days[pos].month() == QDate::currentDate().month() && m_days[pos].year() == QDate::currentDate().year());

    const bool isSelectDay = m_days[pos].month() == m_selectDate.month();

    if (m_days[pos].year() < 1900) return;

    QPainter painter(cell);

    int labelwidth = cell->width();
    int labelheight = cell->height();
    if (1) {
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
        painter.setBrush(QBrush(m_fillColor));
        painter.setPen(Qt::NoPen);
        QPainterPath painterPath;
        painterPath.moveTo(m_radius, 0);
        if (m_roundangle[pos] == -1) {
            painterPath.arcTo(QRect(0, 0, m_radius * 2, m_radius * 2), 90, 90);
        } else {
            painterPath.lineTo(0, 0);
            painterPath.lineTo(0, m_radius);
        }
        painterPath.lineTo(0, labelheight - m_radius);
        if (m_roundangle[pos] == -1) {
            painterPath.arcTo(QRect(0, labelheight - m_radius * 2, m_radius * 2, m_radius * 2), 180, 90);
        } else {
            painterPath.lineTo(0, labelheight);
            painterPath.lineTo(m_radius, labelheight);
        }
        painterPath.lineTo(labelwidth - m_radius, labelheight);
        if (m_roundangle[pos] == 1) {
            painterPath.arcTo(QRect(labelwidth - m_radius * 2, labelheight - m_radius * 2, m_radius * 2, m_radius * 2), 270, 90);
        } else {
            painterPath.lineTo(labelwidth, labelheight);
            painterPath.lineTo(labelwidth, labelheight - m_radius);
        }
        painterPath.lineTo(labelwidth, m_radius);
        //painterPath.moveTo(labelwidth, m_radius);
        if (m_roundangle[pos] == 1) {

            painterPath.arcTo(QRect(labelwidth - m_radius * 2, 0, m_radius * 2, m_radius * 2), 0, 90);

        } else {
            painterPath.lineTo(labelwidth, 0);
            painterPath.lineTo(labelwidth - m_radius, 0);
        }
        painterPath.lineTo(m_radius, 0);
        painterPath.closeSubpath();
        painter.drawPath(painterPath);
        painter.restore();
    }

    painter.setPen(Qt::SolidLine);

    const QString dayNum = QString::number(m_days[pos].month());

    if (isSelectDay) {
#if 0
        QRect fillRect(3, 3, cell->width() - 6, cell->height() - 6);
        int hh = 0;

        if (cell->width() > cell->height()) {
            hh = cell->height();
            fillRect = QRect((cell->width() - hh) / 2.0 + 0.5, 4, hh, hh);
        } else {
            hh = cell->width();
            fillRect = QRect(0, (cell->height() - hh) / 2.0  + 4, hh, hh);
        }

        QPixmap pixmap;
        if (m_themetype == 2)
            pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/darkchoose30X30_checked .svg").scaled(hh, hh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        else {
            pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/choose30X30_checked .svg").scaled(hh, hh, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
        pixmap.setDevicePixelRatio(devicePixelRatioF());
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawPixmap(fillRect, pixmap);
        painter.restore();
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setPen(m_currentDayTextColor);
        painter.drawText(QRect(0, 0, cell->width(), cell->height()), Qt::AlignCenter, dayNum);
#else
        QRect fillRect((cell->width() - 36) / 2, (cell->height() - 36) / 2 + 4, 36, 36);
        QPixmap pixmap;
        if (m_themetype == 2)
            pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/darkchoose30X30_checked .svg");
        else {
            pixmap = DHiDPIHelper::loadNxPixmap(":/resources/icon/choose30X30_checked .svg");
        }
        pixmap.setDevicePixelRatio(devicePixelRatioF());
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawPixmap(fillRect, pixmap);
        painter.restore();
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setPen(m_currentDayTextColor);
        painter.setFont(m_dayNumFont);
        painter.drawText(QRect(0, 0, cell->width(), cell->height()), Qt::AlignCenter, dayNum);
#endif
    } else {
        if (isCurrentDay) {
            painter.setPen(m_backgroundcurrentDayColor);
        } else {
            painter.setPen(m_defaultTextColor);
        }
        painter.setFont(m_dayNumFont);
        painter.drawText(QRect(0, 0, cell->width(), cell->height()), Qt::AlignCenter, dayNum);
    }
    painter.end();
}

bool CMonthDayView::eventFilter(QObject *o, QEvent *e)
{
    QWidget *cell = qobject_cast<QWidget *>(o);

    if (cell && m_cellList.contains(cell)) {
        if (e->type() == QEvent::Paint) {
            paintCell(cell);
        } else if (e->type() == QEvent::MouseButtonPress) {
            cellClicked(cell);
        }
    }

    return false;
}
void CMonthDayView::cellClicked(QWidget *cell)
{
    if (!m_cellSelectable)
        return;

    const int pos = m_cellList.indexOf(cell);
    if (pos == -1)
        return;

    setSelectedCell(pos);
    update();
}

void CMonthDayView::setSelectedCell(int index, int type)
{
    if (m_selectedCell == index)
        return;

    const int prevPos = m_selectedCell;
    m_selectedCell = index;

    m_cellList.at(prevPos)->update();
    m_cellList.at(index)->update();
    m_selectDate = m_days[index];
    if (type == 0 && m_days[index].year() > 1899)
        emit signalsSelectDate(m_days[index]);
}

void CMonthDayView::resizeEvent(QResizeEvent *event)
{
#if 0
    int w = m_fixwidth * 0.08333 + 0.5;
    int h = height();
    for (int c = 0; c != 12; ++c) {
        m_cellList[c]->setFixedSize(w, h);
        m_cellList[c]->update();
    }
    if ((m_realwidth   < m_fixwidth) && m_searchfalg) {
        int t_num = qRound((m_fixwidth - m_realwidth ) / w / 2.0);
        QVector<bool> vindex;
        vindex.resize(12);
        vindex.fill(true);
        for (int i = 0; i < t_num; i++) {
            vindex[i] = false;
            vindex[11 - i] = false;
        }
        for (int i = 0; i < 12; i++) {
            m_cellList[i]->setVisible(vindex[i]);
            m_cellList[i]->update();
        }
    } else {
        for (int i = 0; i < 12; i++) {
            m_cellList[i]->setVisible(true);
            m_cellList[i]->update();
        }
    }
#endif
    int w = (width() - 20) / 12;
    int h = height();

    int ww = 36;
    if (w >= ww) {
        for (int c = 0; c != 12; ++c) {
            m_cellList[c]->setFixedSize(w, h);
            m_roundangle[c] = 0;
            //m_cellList[c]->setVisible(true);
            m_cellList[c]->update();
        }
        for (int i = 0; i < 12; i++) {
            m_cellList[i]->setVisible(true);
            m_cellList[i]->update();
        }
        m_roundangle[0] = -1;
        m_roundangle[11] = 1;
    } else {
        for (int c = 0; c != 12; ++c) {
            m_cellList[c]->setFixedSize(ww, h);
            //m_cellList[c]->setVisible(true);
            m_roundangle[c] = 0;
            m_cellList[c]->update();
        }
        int t_num = qRound((ww * 12 - width() ) / ww / 2.0);
        QVector<bool> vindex;
        vindex.resize(12);
        vindex.fill(true);
        for (int i = 0; i < t_num; i++) {
            vindex[i] = false;
            vindex[11 - i] = false;
        }
        for (int i = 0; i < 12; i++) {
            m_cellList[i]->setVisible(vindex[i]);
            m_cellList[i]->update();
        }
        //m_roundangle[t_num] = -1;
        //m_roundangle[11 - t_num] = 1;
    }
}

