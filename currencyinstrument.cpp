#include "currencyinstrument.h"

//******************************************************************************************************
/*!
 *\struct CurrencyInstrument
 *\brief По какой валюте отображать график.
*/
//******************************************************************************************************

CurrencyInstrument::CurrencyInstrument()
    : id()
    , name()
{

}

CurrencyInstrument::CurrencyInstrument(const QString &anId, const QString &aName)
    : id(anId)
    , name(aName)
{

}

bool CurrencyInstrument::operator == (const CurrencyInstrument &another)
{
    return (id == another.id) && (name == another.name);
}

bool CurrencyInstrument::operator != (const CurrencyInstrument &another)
{
    return !(operator ==(another));
}

bool CurrencyInstrument::isValid() const
{
    return (!id.isEmpty()) && (!name.isEmpty());
}

QString CurrencyInstrument::toString() const
{
    return QString("%1 %2").arg(id).arg(name);
}
