#include "window.h"
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <QVBoxLayout>
#include "headbar.h"
#include "book.h"
#include "tabcontroller.h"

#include <QDebug>

//******************************************************************************************************
/*!
 *\class Windows
 *\brief Менеджер окон.
*/
//******************************************************************************************************

WindowsManager::WindowsManager()
    : QObject()
    , SingletonT<WindowsManager>()
{

}

void WindowsManager::addWindow()
{
    Window *next = new Window(false);
    next->show();
}

WindowList WindowsManager::windowList()
{
    WindowList result;
    QWidgetList widgets = QApplication::topLevelWidgets();
    foreach (QWidget *widget, widgets)
    {
        Window *window = qobject_cast<Window*>(widget);
        if (window != NULL)
        {
            result << window;
        }
    }
    return result;
}

void WindowsManager::receiveTabPlace(const QPointF &globalPos, Window* &window, int &index)
{
    window = NULL;
    index = -1;
    Window *candidate = qobject_cast<Window*>(QApplication::topLevelAt(globalPos.toPoint()));
    if ((candidate != NULL) && (!candidate->isPreviewMode()))
    {
        index = candidate->headBar()->tabController()->dropIndex(globalPos);
        if (index >= 0)
        {
            window = candidate;
        }
    }
}

Window* WindowsManager::findTabSite(const QUuid &tabUid)
{
    Window *result = NULL;
    WindowList windows = windowList();
    foreach (Window *window, windows)
    {
        if (window->book()->contains(tabUid))
        {
            result = window;
            break;
        }
    }
    return result;
}


//******************************************************************************************************
/*!
 *\class Window
 *\brief Окно (главное).
*/
//******************************************************************************************************

Window::Window(bool isPreviewMode)
    : QMainWindow(NULL, Qt::WindowFlags((isPreviewMode) ? Qt::FramelessWindowHint : 0))
    , m_isPreviewMode(isPreviewMode)
    , m_headBar(NULL)
    , m_book(NULL)
    , m_previewWindow()
{
    initializeCentralWidget();
    initializeMenu();
}

bool Window::isPreviewMode() const
{
    return m_isPreviewMode;
}

HeadBar* Window::headBar() const
{
    return m_headBar;
}

Book* Window::book() const
{
    return m_book;
}

void Window::addWindow()
{
    WindowsManager::addWindow();
}

void Window::addTab()
{
    book()->addSheet();
}

void Window::addDocument()
{

}

void Window::closeWindow()
{
    close();
}

void Window::closeTab()
{
    book()->removeSheet(book()->currentIndex());
}

void Window::closeDocument()
{

}

void Window::onTabToBeActivated(int index)
{
    book()->setCurrentIndex(index);
}

void Window::onTabToBeAdded()
{
    addTab();
}

void Window::onTabToBeRemoved(int index)
{
    book()->removeSheet(index);
}

void Window::onTabToBeginDragging(QUuid)
{

}

void Window::onTabToContinueDragging(QUuid tabUid, QPointF globalPos)
{
    Window *fromWindow = WindowsManager::findTabSite(tabUid);
    if (fromWindow != NULL)
    {
        Window *toWindow = NULL;
        int toIndex = -1;
        WindowsManager::receiveTabPlace(globalPos, toWindow, toIndex);
        if ((toWindow != NULL) && (toIndex >= 0))
        {
            // Перемещаем в существующее окно
            if (fromWindow == m_previewWindow)
            {
                m_previewWindow->hide();
            }
            if (fromWindow != toWindow)
            {
                fromWindow->headBar()->tabController()->fixCurrentTab();
            }
            toWindow->book()->moveSheet(fromWindow->book(), tabUid, toIndex);
            toWindow->headBar()->tabController()->moveCurrentTab(globalPos);
        }
        else
        {
            // Перемещаем в новое окно
            if (m_previewWindow.isNull())
            {
                m_previewWindow = new Window(true);
            }
            if (fromWindow != m_previewWindow)
            {
                fromWindow->headBar()->tabController()->fixCurrentTab();
            }
            if (!m_previewWindow.isNull())
            {
                m_previewWindow->book()->moveSheet(fromWindow->book(), tabUid, 0);
            }
            if (!m_previewWindow.isNull())
            {
                m_previewWindow->move(globalPos.toPoint() + QPoint(20, 20));
            }
            if (!m_previewWindow.isNull())
            {
                m_previewWindow->show();
            }
        }
    }
}

void Window::onTabToBeDropped(QUuid tabUid, QPointF globalPos)
{
    if (!m_previewWindow.isNull())
    {
        m_previewWindow->hide();
        Window *fromWindow = WindowsManager::findTabSite(tabUid);
        if (fromWindow == m_previewWindow)
        {
            Window *newWindow = new Window(false);
            newWindow->book()->moveSheet(fromWindow->book(), tabUid, 0);
            newWindow->move(globalPos.toPoint());
            newWindow->show();
        }
        m_previewWindow->close();
        m_previewWindow = NULL;
    }

    WindowList windows = WindowsManager::windowList();
    foreach (Window *window, windows)
    {
        window->headBar()->tabController()->ceaseMoving();
    }

    foreach (Window *window, windows)
    {
        if (window->book()->isEmpty())
        {
            window->close();
        }
    }
}

void Window::onBookChanged()
{
    headBar()->tabController()->setData(book()->tabData());
}

void Window::toggleFullScreen()
{
    if (!m_toggleFullScreenAction->isChecked())
    {
        showNormal();
    }
    else
    {
        showFullScreen();
    }
}

void Window::nextTab()
{
    book()->nextSheet();
}

void Window::previousTab()
{
    book()->previousSheet();
}

void Window::initializeCentralWidget()
{
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    m_headBar = new HeadBar;
    connect(m_headBar->tabController(), SIGNAL(tabToBeActivated(int)), this, SLOT(onTabToBeActivated(int)));
    connect(m_headBar->tabController(), SIGNAL(tabToBeAdded()), this, SLOT(onTabToBeAdded()));
    connect(m_headBar->tabController(), SIGNAL(tabToBeRemoved(int)), this, SLOT(onTabToBeRemoved(int)));
    connect(m_headBar->tabController(), SIGNAL(tabToBeginDragging(QUuid)), this, SLOT(onTabToBeginDragging(QUuid)));
    connect(m_headBar->tabController(), SIGNAL(tabToContinueDragging(QUuid,QPointF)), this, SLOT(onTabToContinueDragging(QUuid,QPointF)));
    connect(m_headBar->tabController(), SIGNAL(tabToBeDropped(QUuid,QPointF)), this, SLOT(onTabToBeDropped(QUuid,QPointF)));
    centralLayout->addWidget(m_headBar);

    m_book = new Book(this);
    connect(m_book, SIGNAL(changed()), this, SLOT(onBookChanged()));
    centralLayout->addWidget(m_book);
}

void Window::initializeMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("Файл");
    fileMenu->addAction("Новое окно", this, SLOT(addWindow()), QKeySequence::New);
    fileMenu->addAction("Новая вкладка", this, SLOT(addTab()), QKeySequence::AddTab);
    fileMenu->addAction("Новый документ", this, SLOT(addDocument()), QKeySequence("Ctrl+D"));
    fileMenu->addSeparator();
    QString closeWindowKst("Alt+F4");
#ifdef Q_OS_MAC
    closeWindowKst = "Ctrl+Meta+W";
#endif
    fileMenu->addAction("Закрыть окно", this, SLOT(closeWindow()), QKeySequence(closeWindowKst));
    fileMenu->addAction("Закрыть вкладку", this, SLOT(closeTab()), QKeySequence::Close);

    QMenu *windowMenu = menuBar()->addMenu("Окно");
    m_toggleFullScreenAction = windowMenu->addAction("Полноэкранный режим", this, SLOT(toggleFullScreen()), QKeySequence::FullScreen);
    m_toggleFullScreenAction->setCheckable(true);
    windowMenu->addSeparator();
    windowMenu->addAction("Следующая вкладка", this, SLOT(nextTab()), QKeySequence::NextChild);
    windowMenu->addAction("Предыдущая вкладка", this, SLOT(previousTab()), QKeySequence::PreviousChild);
}
