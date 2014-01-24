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
    static WindowList windowList(bool doIncludeAll);
    static Window* findWindow(const QUuid &uid);
    static void receiveTabPlace(const QPointF &globalPos, Window* &window, int &index);
    static Window* findTabSite(const QUuid &tabUid);
    static void updateWindowsEnumMenu();
    static void windowIsClosing(Window *window);
    static void gotoWindow(const QUuid &windowUid);
};

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(bool isPreviewMode);
    bool isPreviewMode() const;
    QUuid uid() const;
    HeadBar* headBar() const;
    Book* book() const;

public slots:
    void addWindow();
    void addTab();
    void addDocument();
    void closeWindow();
    void closeTab();
    void closeDocument();
    void updateWindowsEnumMenu();

protected:
     void closeEvent(QCloseEvent *event) override;

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
    void gotoWindow();

private:
    bool m_isPreviewMode;
    QUuid m_uid;
    HeadBar *m_headBar;
    Book *m_book;
    QPointer<Window> m_previewWindow;
    QAction *m_toggleFullScreenAction;
    QMenu *m_windowsEnumMenu;
    void initializeCentralWidget();
    void initializeMenu();
    void updateWindowTitle();
    static bool windowTitleLessThan(Window *w1, Window *w2);
};

#endif // WINDOW_H
