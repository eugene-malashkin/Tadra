#include "sheet.h"
#include <QPainter>
#include "book.h"

#include <QDebug>

//******************************************************************************************************
/*!
 *\class Sheet
 *\brief Лист, содержащий документы. Соответствует одной вкладке.
*/
//******************************************************************************************************

Sheet::Sheet(Book *book)
    : QWidget()
    , m_book(book)
    , m_tabInfo()
    , m_isActive(false)
{

}

Book* Sheet::book() const
{
    return m_book;
}

void Sheet::setTabInfo(const TabInfo &value)
{
    if (m_tabInfo != value)
    {
        m_tabInfo = value;
        update();
    }
}

TabInfo Sheet::tabInfo() const
{
    return m_tabInfo;
}

void Sheet::setActive(bool value)
{
    if (m_isActive != value)
    {
        m_isActive = value;
        setVisible(m_isActive);
    }
}

bool Sheet::isActive() const
{
    return m_isActive;
}

void Sheet::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRect r(0, 0, width(), height());
    painter.fillRect(r, Qt::gray);
    painter.drawText(r, Qt::AlignCenter, tabInfo().label);
}
