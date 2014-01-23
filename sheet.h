#ifndef SHEET_H
#define SHEET_H

#include <QWidget>
#include "tabdata.h"

class Book;

class Sheet : public QWidget
{
    Q_OBJECT

public:
    Sheet(Book *book);
    Book* book() const;
    void setTabInfo(const TabInfo &value);
    TabInfo tabInfo() const;
    void setActive(bool value);
    bool isActive() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Book *m_book;
    TabInfo m_tabInfo;
    bool m_isActive;
};

typedef QList<Sheet*> SheetList;

#endif // SHEET_H
