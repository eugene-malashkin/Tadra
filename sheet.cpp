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
    : QScrollArea()
    , m_book(book)
    , m_layer(NULL)
    , m_tabInfo()
    , m_isActive(false)
{
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    m_layer = new DocumentLayer;
    setWidget(m_layer);
}

Book* Sheet::book() const
{
    return m_book;
}

DocumentLayer* Sheet::layer() const
{
    return m_layer;
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
