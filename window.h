#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "tabdata.h"

class HeadBar;
class Book;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(bool isPreviewMode);
    bool isPreviewMode() const;
    void escapePreviewMode(const QPoint &globalPos);
    HeadBar* headBar() const;
    Book* book() const;

private slots:
    void onTabToBeActivated(int index);
    void onTabToBeAdded();
    void onTabToBeRemoved(int index);
    void onTabToBeginDragging(QUuid uid);
    void onTabToContinueDragging(QUuid uid, QPointF globalPos);
    void onTabToBeDropped(QUuid uid, QPointF globalPos);
    void addWindow();
    void onBookChanged();

    void test1();
    void test2();
    void test3();
    void test4();


private:
    bool m_isPreviewMode;
    HeadBar *m_headBar;
    Book *m_book;
    QPointer<Window> m_previewWindow;
    typedef QList<Window*> WindowList;
    static WindowList windowList();
    static void receiveTabPlace(const QPointF &globalPos, Window* &window, int &index);
    static Window* findTabSite(const QUuid &tabUid);
    void initializeCentralWidget();
    void initializeMenu();
};

#endif // WINDOW_H
