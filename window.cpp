#include "window.h"
#include <QMenuBar>
#include <QMenu>
#include <QApplication>
#include <QVBoxLayout>
#include "headbar.h"
#include "book.h"
#include "tabcontroller.h"

#include <QDebug>

Window::Window(bool isPreviewMode)
    : QMainWindow(NULL, Qt::WindowFlags((isPreviewMode) ? Qt::FramelessWindowHint : 0))
    , m_isPreviewMode(isPreviewMode)
    , m_headBar(NULL)
    , m_book(NULL)
    , m_previewWindow()
{
    initializeCentralWidget();
    initializeMenu();
    if (isPreviewMode)
    {
        menuBar()->setVisible(false);
    }
}

bool Window::isPreviewMode() const
{
    return m_isPreviewMode;
}

void Window::escapePreviewMode()
{
    menuBar()->setVisible(true);
    setWindowFlags(windowFlags() & (~Qt::FramelessWindowHint));
    show();
}

HeadBar* Window::headBar() const
{
    return m_headBar;
}

Book* Window::book() const
{
    return m_book;
}

void Window::onTabToBeActivated(int index)
{
    book()->setCurrentIndex(index);
}

void Window::onTabToBeAdded()
{
    book()->addSheet();
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
    Window *fromWindow = findTabSite(tabUid);
    if (fromWindow != NULL)
    {
        Window *toWindow = NULL;
        int toIndex = -1;
        receiveTabPlace(globalPos, toWindow, toIndex);
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
            m_previewWindow->book()->moveSheet(fromWindow->book(), tabUid, 0);
            m_previewWindow->move(globalPos.toPoint() + QPoint(20, 20));
            m_previewWindow->show();
        }
    }
}

void Window::onTabToBeDropped(QUuid tabUid, QPointF)
{
    if (!m_previewWindow.isNull())
    {
        Window *fromWindow = findTabSite(tabUid);
        if (fromWindow == m_previewWindow)
        {
            m_previewWindow->escapePreviewMode();
            m_previewWindow = NULL;
        }
        else
        {
            m_previewWindow->deleteLater();
        }
    }

    WindowList windows = windowList();
    foreach (Window *window, windows)
    {
        window->headBar()->tabController()->fixCurrentTab();
    }
}

void Window::addWindow()
{
    Window *next = new Window(false);
    next->show();
}

void Window::onBookChanged()
{
    headBar()->tabController()->setData(book()->tabData());
}

void Window::test1()
{
    showFullScreen();
}

void Window::test2()
{
    showNormal();
}

void Window::test3()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    show();
}

void Window::test4()
{
    setWindowFlags(windowFlags() & (~Qt::FramelessWindowHint));
    show();
}

Window::WindowList Window::windowList()
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

void Window::receiveTabPlace(const QPointF &globalPos, Window* &window, int &index)
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

Window* Window::findTabSite(const QUuid &tabUid)
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

    centralWidget->setVisible(true);
}

void Window::initializeMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("Файл");
    QAction *addWindowAction = fileMenu->addAction("Новое окно");
    connect(addWindowAction, SIGNAL(triggered()), this, SLOT(addWindow()));

    connect(fileMenu->addAction("Test 1"), SIGNAL(triggered()), this, SLOT(test1()));
    connect(fileMenu->addAction("Test 2"), SIGNAL(triggered()), this, SLOT(test2()));
    connect(fileMenu->addAction("Test 3"), SIGNAL(triggered()), this, SLOT(test3()));
    connect(fileMenu->addAction("Test 4"), SIGNAL(triggered()), this, SLOT(test4()));
}
