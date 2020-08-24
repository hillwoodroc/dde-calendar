#ifndef BUTTONWIDGET_H
#define BUTTONWIDGET_H
#include <DWidget>
#include <QAbstractButton>
#include <QHBoxLayout>
#include <QObject>

DWIDGET_USE_NAMESPACE

namespace BTWidget {
const int BUTTON_LAYOUT_TOP_MARGIN = 0;
const int BUTTON_LAYOUT_BOTTOM_MARGIN = 0;
const int BUTTON_LAYOUT_LEFT_MARGIN = 0;
const int BUTTON_LAYOUT_RIGHT_MARGIN = 0;
} // namespace BTWidget

class buttonwidget : public DWidget
{
    Q_OBJECT
public:
    explicit buttonwidget(QWidget *parent = nullptr);

    enum ButtonType {
        ButtonNormal,
        ButtonWarning,
        ButtonRecommend
    };

    int addbutton(const QString &text, bool isDefault = false, ButtonType type = ButtonNormal);
    void insertButton(int index, const QString &text, bool isDefault = false, ButtonType type = ButtonNormal);
    void insertButton(int index, QAbstractButton *button, bool isDefault = false);

    int buttonCount() const;

signals:
    void buttonClicked(int index, const QString &text);
public slots:
    void onButtonClicked(bool clicked);

private:
    QList<QAbstractButton *> buttonList;
    QSpacerItem *spacer = nullptr;

    QHBoxLayout *buttonLayout;
    int clickedButtonIndex;
};

#endif // BUTTONWIDGET_H
