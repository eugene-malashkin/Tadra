#ifndef SHEET_H
#define SHEET_H

#include <QScrollArea>
#include "tabdata.h"
#include "documentlayer.h"

class Book;

class Sheet : public QScrollArea
{
    Q_OBJECT

public:
    Sheet(Book *book);
    Book* book() const;
    DocumentLayer* layer() const;
    void setTabInfo(const TabInfo &value);
    TabInfo tabInfo() const;
    void setActive(bool value);
    bool isActive() const;

private:
    Book *m_book;
    DocumentLayer *m_layer;
    TabInfo m_tabInfo;
    bool m_isActive;
};

typedef QList<Sheet*> SheetList;

#endif // SHEET_H
