#ifndef CURRENCYINSTRUMENT_H
#define CURRENCYINSTRUMENT_H

#include <QString>

struct CurrencyInstrument
{
    QString id;
    QString name;
    CurrencyInstrument();
    CurrencyInstrument(const QString &anId, const QString &aName);
    bool operator == (const CurrencyInstrument &another);
    bool operator != (const CurrencyInstrument &another);
    bool isValid() const;
    QString toString() const;
};

#endif // CURRENCYINSTRUMENT_H
