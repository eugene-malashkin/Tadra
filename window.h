#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QPointer>
#include "tabdata.h"
#include "singletont.h"

class HeadBar;
class Book;

class Window;
typedef QList<Window*> WindowList;

class WindowsManager : public QObject, SingletonT<WindowsManager>
{
    Q_OBJECT

public:
    WindowsManager();
    static void addWindow();
    static WindowList windowList();
    static void receiveTabPlace(const QPointF &globalPos, Window* &window, int &index);
    static Window* findTabSite(const QUuid &tabUid);
};

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(bool isPreviewMode);
    bool isPreviewMode() const;
    HeadBar* headBar() const;
    Book* book() const;

public slots:
    void addWindow();
    void addTab();
    void addDocument();
    void closeWindow();
    void closeTab();
    void closeDocument();

private slots:
    void onTabToBeActivated(int index);
    void onTabToBeAdded();
    void onTabToBeRemoved(int index);
    void onTabToBeginDragging(QUuid uid);
    void onTabToContinueDragging(QUuid uid, QPointF globalPos);
    void onTabToBeDropped(QUuid uid, QPointF globalPos);
    void onBookChanged();
    void toggleFullScreen();
    void nextTab();
    void previousTab();
    void renameTab();

private:
    bool m_isPreviewMode;
    HeadBar *m_headBar;
    Book *m_book;
    QPointer<Window> m_previewWindow;
    QAction *m_toggleFullScreenAction;
    void initializeCentralWidget();
    void initializeMenu();
};

#endif // WINDOW_H
