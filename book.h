#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QVBoxLayout>
#include "tabdata.h"
#include "sheet.h"

class Window;

class Book : public QWidget
{
    Q_OBJECT

public:
    Book(Window *window);
    Window* window() const;
    void addSheet(const QString &label = QString());
    void removeSheet(int index, bool doDelete = true);
    int count() const;
    bool isEmpty() const;
    SheetList sheets() const;
    void setCurrentIndex(int value);
    int currentIndex() const;
    Sheet* currentSheet() const;
    int indexOf(const QUuid &tabUid) const;
    bool contains(const QUuid &tabUid) const;
    Sheet* sheet(int index) const;
    Sheet* sheet(const QUuid &tabUid) const;
    TabData tabData() const;
    bool moveSheet(Book *fromBook, const QUuid &tabUid, int toIndex);

signals:
    void changed();

private:
    Window* m_window;
    SheetList m_sheets;
    QVBoxLayout *m_layout;
    void innerSetCurrentIndex(int index, bool &wasChanged);
    QString newTabLabel() const;
};

#endif // BOOK_H
