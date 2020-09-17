#include "SchecduleRemindWidget.h"
#include <QPainter>
#include <QtMath>
#include <QPainterPath>
#include <DGuiApplicationHelper>
DGUI_USE_NAMESPACE
SchecduleRemindWidget::SchecduleRemindWidget(QWidget *parent)
    : DArrowRectangle(DArrowRectangle::ArrowLeft, DArrowRectangle::FloatWindow, parent)
    , m_centerWidget(new CenterWidget(this))
{
    m_centerWidget->setFixedWidth(207);
    m_centerWidget->setFixedHeight(57);
    setContent(m_centerWidget);
    this->resizeWithContent();
    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged,
                     m_centerWidget,
                     &CenterWidget::setTheMe);
    m_centerWidget->setTheMe(DGuiApplicationHelper::instance()->themeType());
}

SchecduleRemindWidget::~SchecduleRemindWidget()
{

}

void SchecduleRemindWidget::setData(const ScheduleDtailInfo &vScheduleInfo, const CSchedulesColor &gcolor)
{
    m_centerWidget->setData(vScheduleInfo, gcolor);
    m_ScheduleInfo = vScheduleInfo;
    gdcolor = gcolor;
    this->setHeight(m_centerWidget->height() + 10);

}

CenterWidget::CenterWidget(DWidget *parent)
    : DFrame(parent)
{
    textfont.setFamily("SourceHanSansSC-Medium");
    textfont.setWeight(QFont::Medium);
//    DFontSizeManager::instance()->bind(this,DFontSizeManager::T8);
}

CenterWidget::~CenterWidget()
{

}

void CenterWidget::setData(const ScheduleDtailInfo &vScheduleInfo, const CSchedulesColor &gcolor)
{
    m_ScheduleInfo = vScheduleInfo;
    gdcolor = gcolor;
    textfont.setPixelSize(12);
//    textfont = DFontSizeManager::instance()->get(DFontSizeManager::T8, textfont);
    UpdateTextList();
    update();
}

void CenterWidget::setTheMe(const int type)
{
    DPalette palette;
    textColor = palette.color(DPalette::Normal, DPalette::Text);
    if (type == 2) {
        timeColor = QColor("#C0C6D4");
        timeColor.setAlphaF(0.7);
    } else {
        timeColor = QColor("#414D68");
        timeColor.setAlphaF(0.7);
    }
    update();
}

void CenterWidget::UpdateTextList()
{
    testList.clear();
    QFontMetrics metrics(textfont);
    textwidth = metrics.width(m_ScheduleInfo.titleName);
    textheight = metrics.height();
    const int  h_count = qCeil(textwidth / textRectWidth);

    QString text;
    if (h_count < 1) {
        testList.append(m_ScheduleInfo.titleName);
    } else {
        const int text_Max_Height = 108;
        const int text_HeightMaxCount = qFloor(text_Max_Height / textheight);
        for (int i = 0; i < m_ScheduleInfo.titleName.count(); ++i) {
            text += m_ScheduleInfo.titleName.at(i);
            if (metrics.width(text) > textRectWidth) {
                text.remove(text.count() - 1, 1);
                testList.append(text);
                text = "";
                if (testList.count() == (text_HeightMaxCount - 1)) {
                    text = m_ScheduleInfo.titleName.right( m_ScheduleInfo.titleName.count() - i );
                    testList.append(metrics.elidedText(text, Qt::ElideRight, textRectWidth));
                    break;
                }
                --i;
            } else {
                if (i + 1 == m_ScheduleInfo.titleName.count()) {
                    testList.append(text);
                }
            }
        }
    }
    this->setFixedHeight(testList.count() * textheight + 30 + 8);
}

void CenterWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    int diam = 8;
    int x = 40 - 13;
//    int y = 17 - 5;
    QFont timeFont;
    timeFont.setFamily("SourceHanSansSC-Medium");
    timeFont.setPixelSize(12);
//    timeFont = DFontSizeManager::instance()->get(DFontSizeManager::T8, timeFont);
    QPainter painter(this);

    //draw time
    QPen pen;
    pen.setColor(timeColor);
    painter.setPen(pen);
    painter.setFont(timeFont);
    QString timestr;

    QLocale locale;
    timestr = m_ScheduleInfo.beginDateTime.time().toString("AP h:mm");
    QFontMetrics metrics(timeFont);
    if (m_ScheduleInfo.allday)
        timestr = tr("All Day");
    int timewidth = metrics.width(timestr);
    int timeheight = metrics.height();

    painter.drawText(QRect( x + 13, 7, timewidth, timeheight), Qt::AlignLeft | Qt::AlignTop, timestr);

    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(gdcolor.dotColor));
    painter.drawEllipse(x, 7 + (timeheight - diam) / 2, diam, diam);

    pen.setColor(textColor);
    painter.setPen(pen);
    painter.setFont(textfont);
    for (int i = 0; i < testList.count(); i++) {
        painter.drawText(
            QRect(x, 30 + i * textheight, textRectWidth, textheight),
            Qt::AlignLeft, testList.at(i));
    }
    painter.end();
}
