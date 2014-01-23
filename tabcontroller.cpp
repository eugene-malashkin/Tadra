#include "tabcontroller.h"

//******************************************************************************************************
/*!
 *\class TabController
 *\brief Контроллер вкладок.
*/
//******************************************************************************************************

TabController::TabController(TabResponsibility *base, QObject *parent)
    : QObject(parent)
    , m_base(base)
{

}

TabResponsibility* TabController::base() const
{
    return m_base;
}

void TabController::setData(const TabData &value)
{
    base()->setData(value);
}

TabData TabController::data() const
{
    return base()->data();
}

int TabController::dropIndex(const QPointF &globalPos) const
{
    return base()->dropIndex(globalPos);
}

void TabController::moveCurrentTab(const QPointF &globalPos)
{
    base()->moveCurrentTab(globalPos);
}

void TabController::fixCurrentTab()
{
    base()->fixCurrentTab();
}

void TabController::ceaseMoving()
{
    base()->ceaseMoving();
}
