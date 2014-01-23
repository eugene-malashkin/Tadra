#ifndef TABDATA_H
#define TABDATA_H

#include <QUuid>
#include <QString>
#include <QList>
#include <QPoint>

struct TabInfo
{
    QUuid uid;
    QString label;
    TabInfo();
    TabInfo(const QUuid &anUid, const QString &aLabel);
    TabInfo(const TabInfo &another);
    bool operator == (const TabInfo &another) const;
    bool operator != (const TabInfo &another) const;
    bool isValid() const;
};
typedef QList<TabInfo> TabInfoList;

struct TabData
{
    TabInfoList items;
    int currentIndex;
    TabData();
    TabData(const TabData &another);
    bool operator == (const TabData &another) const;
    bool operator != (const TabData &another) const;
    bool isValid() const;
};


#endif // TABDATA_H
