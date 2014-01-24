#include "headbar.h"
#include <QApplication>
#include <QFontMetricsF>
#include "design.h"

#include <QDebug>

//******************************************************************************************************
/*!
 *\class TabDrawInfo
 *\brief Запись с информацией о том, как рисовать вкладку.
*/
//******************************************************************************************************

TabDrawInfo::TabDrawInfo()
    : index(-1)
    , uid()
    , tabRect()
    , textRect()
    , closeButtonRect()
    , text()
    , elidedText()
    , isHover(false)
    , isActive(false)
    , isMoving(false)
    , iconMode(QIcon::Normal)
{

}


//******************************************************************************************************
/*!
 *\class TabBar
 *\brief Отображение вкладок, с кнопками.
*/
//******************************************************************************************************

TabSwitcherObject::TabSwitcherObject(HeadBar *headBar)
    : GraphicObject(headBar)
    , m_headBar(headBar)
    , m_data()
    , m_isMovingCurrentTab(false)
    , m_movingCurrentTabPos()
    , m_clickPoint()
    , m_clickOffset()
    , m_clickTabUid()
    , m_isDragging(false)
    , m_closeIcon()
{
    m_closeIcon.addFile("://resources/CloseNormal.png", QSize(), QIcon::Normal);
    m_closeIcon.addFile("://resources/CloseHover.png", QSize(), QIcon::Active);
    m_closeIcon.addFile("://resources/CloseDown.png", QSize(), QIcon::Selected);
}

HeadBar* TabSwitcherObject::headBar() const
{
    return m_headBar;
}

void TabSwitcherObject::setData(const TabData &value)
{
    if (m_data != value)
    {
        m_data = value;
        redraw();
    }
}

TabData TabSwitcherObject::data() const
{
    return m_data;
}

int TabSwitcherObject::dropIndex(const QPointF &globalPos) const
{
    int result = -1;
    QPointF pos = supervisor()->positionFromGlobalToLocal(globalPos);
    if (rect().contains(pos))
    {
        result = qBound(0, int((pos.x() - rect().left()) / 100), m_data.items.count());
    }
    return result;
}

void TabSwitcherObject::moveCurrentTab(const QPointF &globalPos)
{
    m_isMovingCurrentTab = true;
    m_movingCurrentTabPos = supervisor()->positionFromGlobalToLocal(globalPos);
    redraw();
}

void TabSwitcherObject::fixCurrentTab()
{
    m_isMovingCurrentTab = false;
    redraw();
}

void TabSwitcherObject::ceaseMoving()
{
    m_isMovingCurrentTab = false;
    m_movingCurrentTabPos = QPointF();
    m_clickPoint = QPointF();
    m_clickOffset = QPointF();
    m_clickTabUid = QUuid();
    m_isDragging = false;
    redraw();
}

TabController* TabSwitcherObject::tabController() const
{
    return headBar()->tabController();
}

void TabSwitcherObject::paint(QPainter *painter)
{
    TabDrawMap map = tabDrawMap();
    foreach (const TabDrawInfo &info, map)
    {
        if (!info.isActive)
        {
            paintTab(painter, info);
        }
    }
    if ((data().currentIndex >= 0) && (data().currentIndex < data().items.count()))
    {
        QUuid currentUid = data().items[data().currentIndex].uid;
        if (map.contains(currentUid))
        {
            paintTab(painter, map[currentUid]);
        }
    }

    painter->setBrush(Qt::green);
    painter->drawRect(newButtonRect());
}

void TabSwitcherObject::handleEvent(UserEvent event)
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
                if (tabRectFormer(i).contains(event.mousePosition))
                {
                    if (data().currentIndex != i)
                    {
                        emit tabController()->tabToBeActivated(i);
                    }
                    m_clickPoint = event.mousePosition;
                    m_clickOffset = event.mousePosition - tabRectFormer(i).center();
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

QSizeF TabSwitcherObject::sizeConstraint(const QSizeF &) const
{
    return QSizeF(Design::instance()->size(Design::TabSwitcherMinWidth), Design::instance()->size(Design::TabSwitcherMinHeight));
}


QRectF TabSwitcherObject::tabRectFormer(int index) const
{
    return QRect(rect().left() + index*100, rect().top(), 100, rect().height());
}

QRectF TabSwitcherObject::newButtonRect() const
{
    return QRectF(rect().left() + data().items.count()*100.0 + 10.0, rect().top() + 10.0, 10.0, 10.0);
}

QRectF TabSwitcherObject::closeButtonRect(int index) const
{
    return QRectF(rect().left() + index*100.0 + 10.0, rect().top() + 10.0, 10.0, 10.0);
}

NumberMap TabSwitcherObject::currentCoordinates() const
{
    NumberMap result;
    for (int i = 0; i < m_data.items.count(); i++)
    {
        double value = rect().left() + double(i)*currentTabWidth();
        if ((m_data.currentIndex == i) && (m_isMovingCurrentTab))
        {
            value = m_movingCurrentTabPos.x() - currentTabWidth()/2.0;
        }
        result[m_data.items[i].uid] = value;
    }
    return result;
}

double TabSwitcherObject::currentTabWidth() const
{
    return 100;
}

TabDrawMap TabSwitcherObject::tabDrawMap() const
{
    TabDrawMap result;
    NumberMap coordinates = currentCoordinates();
    double buttonSize = Design::instance()->size(Design::GraphicButtonDefaultIconSize);
    double elementSpacing = Design::instance()->size(Design::TabElementSpacing);
    for (int i = 0; i < m_data.items.count(); i++)
    {
        TabDrawInfo item;
        QUuid uid = m_data.items[i].uid;
        item.index = i;
        item.uid = uid;
        double left = coordinates[uid];
        item.tabRect = QRectF(left, rect().top(), currentTabWidth(), rect().height());
        item.textRect = QRectF(item.tabRect.left() + elementSpacing, rect().top(), currentTabWidth() - buttonSize - elementSpacing*3.0, rect().height());
        item.closeButtonRect = QRectF(item.tabRect.right() - buttonSize - elementSpacing, item.tabRect.center().y() - buttonSize/2.0, buttonSize, buttonSize);
        item.text = m_data.items[i].label;
        item.isActive = (i == m_data.currentIndex);
        item.isMoving = item.isActive && m_isMovingCurrentTab;
        result[uid] = item;
    }
    return result;
}

void TabSwitcherObject::paintTab(QPainter *painter, const TabDrawInfo &info)
{
    QColor textColor = Design::instance()->color(Design::TabNormalTextColor);
    QColor bgColor = Design::instance()->color(Design::TabNormalBgColor);
    if (info.isActive)
    {
        textColor = Design::instance()->color(Design::TabActiveTextColor);
        bgColor = Design::instance()->color(Design::TabActiveBgColor);
    }
    else if (info.isHover)
    {
        textColor = Design::instance()->color(Design::TabHoverTextColor);
        bgColor = Design::instance()->color(Design::TabHoverBgColor);
    }
    painter->setPen(Design::instance()->color(Design::TabLineColor));
    painter->setBrush(bgColor);

    QPainterPath path;
    double radius = 16;
    path.moveTo(info.tabRect.bottomLeft());
    path.lineTo(info.tabRect.left(), info.tabRect.top() + radius);
    path.arcTo(info.tabRect.left(), info.tabRect.top(), radius, radius, 180.0, -90.0);
    path.lineTo(info.tabRect.right()-radius, info.tabRect.top());
    path.arcTo(info.tabRect.right()-radius, info.tabRect.top(), radius, radius, 90.0, -90.0);
    path.lineTo(info.tabRect.bottomRight());
    path.lineTo(info.tabRect.bottomLeft());

    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path);
    QFont font;
    QFontMetricsF fm(font);
    QString elidedText = fm.elidedText(info.text, Qt::ElideMiddle, info.textRect.width());
    painter->setPen(textColor);
    painter->drawText(info.textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
    if (!info.isMoving)
    {
        m_closeIcon.paint(painter, info.closeButtonRect.toRect(), Qt::AlignCenter, info.iconMode);
    }
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
    m_tabBar = new TabSwitcherObject(this);
    m_tabBar->setSupervisor(this);
    setGraphicObject(m_tabBar);
    m_tabController = new TabController(m_tabBar, this);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

TabController* HeadBar::tabController() const
{
    return m_tabController;
}
