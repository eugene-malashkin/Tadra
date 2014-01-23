#include "headbar.h"
#include <QApplication>

#include <QDebug>

//******************************************************************************************************
/*!
 *\class TabBar
 *\brief Отображение вкладок, с кнопками.
*/
//******************************************************************************************************

TabBar::TabBar(HeadBar *headBar)
    : GraphicObject(headBar)
    , m_headBar(headBar)
    , m_data()
    , m_isMovingCurrentTab(false)
    , m_movingCurrentTabPos()
    , m_clickPoint()
    , m_clickOffset()
    , m_clickTabUid()
    , m_isDragging(false)
{

}

HeadBar* TabBar::headBar() const
{
    return m_headBar;
}

void TabBar::setData(const TabData &value)
{
    if (m_data != value)
    {
        m_data = value;
        redraw();
    }
}

TabData TabBar::data() const
{
    return m_data;
}

int TabBar::dropIndex(const QPointF &globalPos) const
{
    int result = -1;
    QPointF pos = supervisor()->positionFromGlobalToLocal(globalPos);
    if (rect().contains(pos))
    {
        result = qBound(0, int((pos.x() - rect().left()) / 100), m_data.items.count());
    }
    return result;
}

void TabBar::moveCurrentTab(const QPointF &globalPos)
{
    m_isMovingCurrentTab = true;
    m_movingCurrentTabPos = supervisor()->positionFromGlobalToLocal(globalPos);
    redraw();
}

void TabBar::fixCurrentTab()
{
    m_isMovingCurrentTab = false;
    /*
    m_movingCurrentTabPos = QPointF();
    m_clickPoint = QPointF();
    m_clickOffset = QPointF();
    m_clickTabUid = QUuid();
    m_isDragging = false;
    */
    redraw();
}

void TabBar::ceaseMoving()
{
    m_isMovingCurrentTab = false;
    m_movingCurrentTabPos = QPointF();
    m_clickPoint = QPointF();
    m_clickOffset = QPointF();
    m_clickTabUid = QUuid();
    m_isDragging = false;
    redraw();
}

TabController* TabBar::tabController() const
{
    return headBar()->tabController();
}

void TabBar::paint(QPainter *painter)
{
    painter->fillRect(rect(), Qt::yellow);

    painter->setPen(Qt::black);

    for (int i = 0; i < data().items.count(); i++)
    {
        if (i != data().currentIndex)
        {
            QRectF r = tabRect(i);
            painter->setBrush(Qt::white);
            painter->drawRect(r);
            painter->drawText(r, Qt::AlignCenter, data().items[i].label);
            painter->setBrush(Qt::red);
            painter->drawRect(closeButtonRect(i));
        }
    }
    if ((data().currentIndex >= 0) && (data().currentIndex < data().items.count()))
    {
        double l = tabRect(data().currentIndex).left();
        if (m_isMovingCurrentTab)
        {
            l = m_movingCurrentTabPos.x()-50;
        }
        QRectF r(l, rect().top(), 100, rect().height());
        painter->setBrush(Qt::gray);
        painter->drawRect(r);
        painter->drawText(r, Qt::AlignCenter, data().items[data().currentIndex].label);
        if (!m_isMovingCurrentTab)
        {
            painter->setBrush(Qt::red);
            painter->drawRect(closeButtonRect(data().currentIndex));
        }
    }

    painter->setBrush(Qt::green);
    painter->drawRect(newButtonRect());
}

void TabBar::handleEvent(UserEvent event)
{
    if ((event.type == UserEvent::MouseDown) && (event.button == Qt::LeftButton))
    {
        if (newButtonRect().contains(event.mousePosition))
        {
            emit tabController()->tabToBeAdded();
        }
        else
        {
            for (int i = 0; i < data().items.count(); i++)
            {
                if (closeButtonRect(i).contains(event.mousePosition))
                {
                    emit tabController()->tabToBeRemoved(i);
                    return;
                }
            }

            for (int i = 0; i < data().items.count(); i++)
            {
                if (tabRect(i).contains(event.mousePosition))
                {
                    if (data().currentIndex != i)
                    {
                        emit tabController()->tabToBeActivated(i);
                    }
                    m_clickPoint = event.mousePosition;
                    m_clickOffset = event.mousePosition - tabRect(i).center();
                    m_clickTabUid = data().items[i].uid;
                    m_isDragging = false;
                    return;
                }
            }
        }
    }
    if ((event.type == UserEvent::MouseMove) && (!m_clickTabUid.isNull()))
    {
        if (!m_isDragging)
        {
            m_isDragging = ((event.mousePosition - m_clickPoint).manhattanLength() < QApplication::startDragDistance());
            if (m_isDragging)
            {
                emit tabController()->tabToBeginDragging(m_clickTabUid);
            }
        }
        if (m_isDragging)
        {
            emit tabController()->tabToContinueDragging(m_clickTabUid, supervisor()->positionFromLocalToGlobal(event.mousePosition) - m_clickOffset);
        }
    }
    if (event.type == UserEvent::MouseUp)
    {
        if (m_isDragging)
        {
            emit tabController()->tabToBeDropped(m_clickTabUid, supervisor()->positionFromLocalToGlobal(event.mousePosition) - m_clickOffset);
        }
        m_clickTabUid = QUuid();
        m_isDragging = false;
    }
}

QSizeF TabBar::sizeConstraint(const QSizeF &) const
{
    return QSizeF(20, 40);
}


QRectF TabBar::tabRect(int index) const
{
    return QRect(rect().left() + index*100, rect().top(), 100, rect().height());
}

QRectF TabBar::newButtonRect() const
{
    return QRectF(rect().left() + data().items.count()*100.0 + 10.0, rect().top() + 10.0, 10.0, 10.0);
}

QRectF TabBar::closeButtonRect(int index) const
{
    return QRectF(rect().left() + index*100.0 + 10.0, rect().top() + 10.0, 10.0, 10.0);
}


//******************************************************************************************************
/*!
 *\class HeadBar
 *\brief Верхняя панель, включая вкладки.
*/
//******************************************************************************************************

HeadBar::HeadBar(QWidget *parent)
    : GraphicWidget(parent)
    , m_tabBar(NULL)
    , m_tabController(NULL)
{
    m_tabBar = new TabBar(this);
    m_tabBar->setSupervisor(this);
    setGraphicObject(m_tabBar);
    m_tabController = new TabController(m_tabBar, this);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

TabController* HeadBar::tabController() const
{
    return m_tabController;
}
