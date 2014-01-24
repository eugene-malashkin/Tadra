#ifndef BASE_H
#define BASE_H

#include <QList>
#include <QVector>
#include <QHash>
#include <QPointer>
#include <QUuid>

typedef QVector<int> IntVector;
typedef QVector<double> DoubleVector;
typedef QList<IntVector> IntVectorList;

template <class T> inline uint qHash(const QPointer<T> key, uint seed) Q_DECL_NOTHROW
{
    return qHash(key.data(), seed);
}

inline uint qHash(const QUuid &key, uint seed) Q_DECL_NOTHROW
{
    return qHash(key.toRfc4122(), seed);
}

#endif // BASE_H
