#include "hintwindow.h"
#include "ui_hintwindow.h"
#include <QDesktopWidget>

static const int hintWindowDeltaX = 4;
static const int hintWindowDeltaY = 16;

//******************************************************************************************************
/*!
 *\class HintWindow
 *\brief Окно для отображения всплывающей подсказки
*/
//******************************************************************************************************

HintWindow::HintWindow(QWidget *parent)
    :QWidget(parent, Qt::Window | Qt::FramelessWindowHint | Qt::ToolTip)
    ,ui(new Ui::HintWindow)
{
    ui->setupUi(this);
}

HintWindow::~HintWindow()
{
    delete ui;
}

void HintWindow::setText(const QString &value)
{
    ui->label->setText(value);
    adjustSize();
}

QString HintWindow::text() const
{
    return ui->label->text();
}

void HintWindow::showAt(const QPoint &cursorPosition)
{
    ui->label->adjustSize();
    QSize ls = ui->label->size();
    QSize ss = screenSize(this);
    QPoint windowPosition(cursorPosition.x() + hintWindowDeltaX, cursorPosition.y() + hintWindowDeltaY);
    if (windowPosition.x() + ls.width() > ss.width())
    {
        windowPosition.setX(windowPosition.x() - ls.width() - hintWindowDeltaX*2);
    }
    if (windowPosition.y() + ls.height() > ss.height())
    {
        windowPosition.setY(windowPosition.y() - ls.height() - hintWindowDeltaY*2);
    }
    move(windowPosition);
    show();
}

QSize HintWindow::screenSize(const QWidget *widget)
{
    QSize result;
    QDesktopWidget *desktop = qApp->desktop();
    int screen = desktop->screenNumber(widget);
    QRect screenRect = desktop->screenGeometry(screen);
    result = screenRect.size();
    return result;
}
