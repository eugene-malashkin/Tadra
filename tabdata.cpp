#include "tabdata.h"

//******************************************************************************************************
/*!
 *\struct TabInfo
 *\brief Данные по вкладке.
*/
//******************************************************************************************************

TabInfo::TabInfo()
    : uid()
    , label()
{

}

TabInfo::TabInfo(const QUuid &anUid, const QString &aLabel)
    : uid(anUid)
    , label(aLabel)
{

}

TabInfo::TabInfo(const TabInfo &another)
    : uid(another.uid)
    , label(another.label)
{

}

bool TabInfo::operator == (const TabInfo &another) const
{
    return (uid == another.uid) && (label == another.label);
}

bool TabInfo::operator != (const TabInfo &another) const
{
    return !(operator==(another));
}

bool TabInfo::isValid() const
{
    return !uid.isNull();
}


//******************************************************************************************************
/*!
 *\struct TabData
 *\brief Данные по вкладкам.
*/
//******************************************************************************************************

TabData::TabData()
    : items()
    , currentIndex(-1)
{

}

TabData::TabData(const TabData &another)
    : items(another.items)
    , currentIndex(another.currentIndex)
{

}

bool TabData::operator == (const TabData &another) const
{
    return
        (items == another.items) &&
        (currentIndex == another.currentIndex);
}

bool TabData::operator != (const TabData &another) const
{
    return !(operator==(another));
}

bool TabData::isValid() const
{
    bool result = true;
    foreach (const TabInfo &item, items)
    {
        if (!item.isValid())
        {
            result = false;
            break;
        }
    }
    return result;
}
