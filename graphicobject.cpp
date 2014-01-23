#include "graphicobject.h"

#include <QDebug>

//******************************************************************************************************
/*!
 *\struct UserEvent
 *\brief Пользовательское событие
*/
//******************************************************************************************************

UserEvent::UserEvent()
    :type(Unknown),
     button(Qt::NoButton),
     buttons(Qt::NoButton),
     mousePosition(0, 0),
     key(0)
{

}

QString UserEvent::toString() const
{
    QString result;
    switch (type)
    {
    case MouseDown:
        result = QString("MouseDown %1, (%2, %3)").arg(button).arg(mousePosition.x()).arg(mousePosition.y());
        break;
    case MouseUp:
        result = QString("MouseUp %1, (%2, %3)").arg(button).arg(mousePosition.x()).arg(mousePosition.y());;
        break;
    case MouseMove:
        result = QString("MouseMove (%1, %2)").arg(mousePosition.x()).arg(mousePosition.y());
        break;
    case MouseLeave:
        result = "MouseLeave";
        break;
    case MouseDoubleClick:
        result = QString("MouseDoubleClick (%1, %2)").arg(mousePosition.x()).arg(mousePosition.y());
        break;
    case KeyPress:
        result = QString("KeyPress, %1").arg(key);
        break;
    case KeyRelease:
        result = QString("KeyRelease, %1").arg(key);
        break;
    default:
        result = "unknown";
        break;
    };
    return result;
}


//******************************************************************************************************
/*!
 *\struct HitInfo
 *\brief Результат hit теста
*/
//******************************************************************************************************

HitInfo::HitInfo()
    :result(false)
    ,cursor(Qt::ArrowCursor)
    ,hint()
{

}


//******************************************************************************************************
/*!
 *\class GraphicObject
 *\brief Базовый класс графических объектов
*/
//******************************************************************************************************

GraphicObject::GraphicObject(QObject *parent)
    :QObject(parent)
    ,m_supervisor(NULL)
    ,m_requestVisiuble(true)
    ,m_canVisible(true)
    ,m_drawIndex(0)
    ,m_handleIndex(0)
    ,m_childrenObjects()
    ,m_rect()
    ,m_handleUsing()
    ,m_lastSendedChild(NULL)
{

}

void GraphicObject::setSupervisor(AbstractGraphicSupervisor *value)
{
    m_supervisor = value;
    foreach (GraphicObject *obj, m_childrenObjects)
    {
        obj->setSupervisor(m_supervisor);
    }
}

AbstractGraphicSupervisor* GraphicObject::supervisor() const
{
    return m_supervisor;
}

int GraphicObject::registerChildObject(GraphicObject *obj, bool autoComputeOrderIndex)
{
    int result = m_childrenObjects.indexOf(obj);
    if (result < 0)
    {
        m_childrenObjects << obj;
        obj->setSupervisor(supervisor());
        result = m_childrenObjects.count()-1;
        if (autoComputeOrderIndex)
        {
            assignOrderIndexes();
        }
    }
    return result;
}

bool GraphicObject::unregisterChildObject(GraphicObject *obj)
{
    int count = m_childrenObjects.removeAll(obj);
    return (count > 0);
}

bool GraphicObject::unregisterChildObject(int objIndex)
{
    bool result = false;
    if ((objIndex >= 0) && (objIndex < m_childrenObjects.count()))
    {
        m_childrenObjects.removeAt(objIndex);
        result = true;
    }
    return result;
}

GraphicObjectList GraphicObject::childrenObjects() const
{
    return m_childrenObjects;
}

GraphicObjectList GraphicObject::childrenVisibleObjects(bool orderDesc, bool orderByHandleIndex) const
{
    // Формируем список видимых объектов
    GraphicObjectList result;
    foreach (GraphicObject *obj, m_childrenObjects)
    {
        if (obj->visible())
        {
            result << obj;
        }
    }

    // Сортируем список согласно параметрам
    if ((!orderDesc) && (!orderByHandleIndex))
    {
        qSort(result.begin(), result.end(), drawIndexLessThan);
    }
    if ((orderDesc) && (!orderByHandleIndex))
    {
        qSort(result.begin(), result.end(), drawIndexGreaterThan);
    }
    if ((!orderDesc) && (orderByHandleIndex))
    {
        qSort(result.begin(), result.end(), handleIndexLessThan);
    }
    if ((orderDesc) && (orderByHandleIndex))
    {
        qSort(result.begin(), result.end(), handleIndexGreaterThan);
    }
    return result;
}

void GraphicObject::bringChildObjectToFront(GraphicObject *obj)
{
    GraphicObjectList list = childrenVisibleObjects();
    if (list.contains(obj))
    {
        obj->setOrderIndex(list.last()->orderIndex() + 1);
        assignOrderIndexes();
    }
}

void GraphicObject::moveChildObjectBackwards(GraphicObject *obj)
{
    assignOrderIndexes();
    GraphicObjectList list = childrenVisibleObjects();
    int index = list.indexOf(obj);
    if (index > 0)
    {
        int swap = list[index-1]->orderIndex();
        list[index-1]->setOrderIndex(list[index]->orderIndex());
        list[index]->setOrderIndex(swap);
    }
}

void GraphicObject::moveChildObjectForwards(GraphicObject *obj)
{
    assignOrderIndexes();
    GraphicObjectList list = childrenVisibleObjects();
    int index = list.indexOf(obj);
    if ((index >= 0) && (index < list.count()-1))
    {
        int swap = list[index+1]->orderIndex();
        list[index+1]->setOrderIndex(list[index]->orderIndex());
        list[index]->setOrderIndex(swap);
    }
}

void GraphicObject::putChildObjectToBack(GraphicObject *obj)
{
    GraphicObjectList list = childrenVisibleObjects();
    if (list.contains(obj))
    {
        obj->setOrderIndex(list.first()->orderIndex() - 1);
        assignOrderIndexes();
    }
}

void GraphicObject::setRequestVisible(bool value)
{
    m_requestVisiuble = value;
}

bool GraphicObject::requestVisible() const
{
    return m_requestVisiuble;
}

void GraphicObject::setCanVisible(bool value)
{
    m_canVisible = value;
}

bool GraphicObject::canVisible() const
{
    return m_canVisible;
}

bool GraphicObject::visible() const
{
    return m_requestVisiuble && m_canVisible;
}

void GraphicObject::setOrderIndex(int value)
{
    m_drawIndex = value;
    m_handleIndex = value;
}

int GraphicObject::orderIndex() const
{
    return m_drawIndex;
}

void GraphicObject::setDrawIndex(int value)
{
    m_drawIndex = value;
}

int GraphicObject::drawIndex() const
{
    return m_drawIndex;
}

void GraphicObject::setHandleIndex(int value)
{
    m_handleIndex = value;
}

int GraphicObject::handleIndex() const
{
    return m_handleIndex;
}

void GraphicObject::setRect(const QRectF &supposedRect)
{
    QRectF appliedRect = supposedRect;
    QSizeF constraint = sizeConstraint(supposedRect.size());
    if (appliedRect.width() < constraint.width())
    {
        appliedRect.setRight(appliedRect.left() + constraint.width());
    }
    if (appliedRect.height() < constraint.height())
    {
        appliedRect.setBottom(appliedRect.top() + constraint.height());
    }
    if (m_rect != appliedRect)
    {
        m_rect = appliedRect;
        resize();
    }
}

void GraphicObject::setRect(double left, double top, double width, double height)
{
    setRect(QRectF(left, top, width, height));
}

QRectF GraphicObject::rect() const
{
    return m_rect;
}

void GraphicObject::setHandleUsing(bool value)
{
    m_handleUsing = value;
    if (!value)
    {
        GraphicObject *obj = findHandleUsingChildObject();
        if (obj != NULL)
        {
            obj->setHandleUsing(false);
        }
    }
}

bool GraphicObject::handleUsing() const
{
    bool result = m_handleUsing;
    if (!result)
    {
        result = (findHandleUsingChildObject() != NULL);
    }
    return result;
}

void GraphicObject::paint(QPainter *painter)
{
    paintChildrenObjects(painter);
}

void GraphicObject::resize()
{

}

void GraphicObject::update()
{
    updateChildrenObjects();
}

HitInfo GraphicObject::hitInfo(const QPointF &point) const
{
    HitInfo result;
    findHitChildObject(point, result);
    return result;
}

void GraphicObject::handleEvent(UserEvent event)
{
    handleEventByChildrenObjects(event);
}

QSizeF GraphicObject::sizeConstraint(const QSizeF &) const
{
    return QSizeF(0, 0);
}

void GraphicObject::redraw()
{
    if (m_supervisor != NULL)
    {
        m_supervisor->redraw();
    }
}

void GraphicObject::updateAndRedraw()
{
    if (m_supervisor != NULL)
    {
        m_supervisor->updateAndRedraw();
    }
}

void GraphicObject::paintChildrenObjects(QPainter *painter)
{
    GraphicObjectList list = childrenVisibleObjects();
    foreach (GraphicObject *obj, list)
    {
        obj->paint(painter);
    }
}

void GraphicObject::setRectToChildrenObjects(const QRectF &rect)
{
    GraphicObjectList list = childrenVisibleObjects();
    foreach (GraphicObject *obj, list)
    {
        obj->setRect(rect);
    }
}

void GraphicObject::updateChildrenObjects()
{
    foreach (GraphicObject *obj, m_childrenObjects)
    {
        obj->update();
    }
}

GraphicObject* GraphicObject::findHitChildObject(const QPointF &point, HitInfo &hitInfo) const
{
    GraphicObject *result = NULL;
    hitInfo = HitInfo();

    GraphicObjectList list = childrenVisibleObjects(true, true);

    foreach (GraphicObject *obj, list)
    {
        HitInfo hi = obj->hitInfo(point);
        if (hi.result)
        {
            result = obj;
            hitInfo = hi;
            break;
        }
    }
    return result;
}

void GraphicObject::handleEventByChildrenObjects(UserEvent event)
{
    if (event.type == UserEvent::MouseLeave)
    {
        sendMouseLeaveMessage(m_lastSendedChild);
        m_lastSendedChild = NULL;
    }
    if ((event.type == UserEvent::KeyPress) || (event.type == UserEvent::KeyRelease))
    {
        sendKeyMessage(event);
    }
    if ((event.type == UserEvent::MouseDown) ||
        (event.type == UserEvent::MouseUp) ||
        (event.type == UserEvent::MouseMove) ||
        (event.type == UserEvent::MouseDoubleClick))
    {
        GraphicObject *handleUsingChild = findHandleUsingChildObject();
        if (handleUsingChild)
        {
            sendMouseMessage(handleUsingChild, event);
        }
        else
        {
            HitInfo hitInfo;
            GraphicObject *childObject = findHitChildObject(event.mousePosition, hitInfo);
            sendMouseMessage(childObject, event);
        }
    }
}

QString GraphicObject::wrapHintToHtml(const QString &text)
{
    QString hintTemplate =
            "<html>"
            "<body>"
            "<table border='0' cellpadding='4' cellspacing='0'>"
            "<tr><td>%1</td></tr>"
            "</table>"
            "</body>"
            "</html>";
    return hintTemplate.arg(text);
}

void GraphicObject::assignOrderIndexes()
{
    GraphicObjectList tmp = childrenVisibleObjects();
    for (int i = 0; i < tmp.count(); i++)
    {
        tmp[i]->setOrderIndex(i + 1);
    }
}

GraphicObject* GraphicObject::findHandleUsingChildObject() const
{
    GraphicObject *result = NULL;
    foreach (GraphicObject *obj, m_childrenObjects)
    {
        if ((obj->visible()) && (obj->handleUsing()))
        {
            result = obj;
            break;
        }
    }
    return result;
}

void GraphicObject::sendMouseLeaveMessage(GraphicObject *obj)
{
    if (obj)
    {
        UserEvent event;
        event.type = UserEvent::MouseLeave;
        obj->handleEvent(event);
    }
}

void GraphicObject::sendKeyMessage(UserEvent event)
{
    foreach (GraphicObject *obj, m_childrenObjects)
    {
        if ((obj->visible()) && (obj->handleUsing()))
        {
            obj->handleEvent(event);
        }
    }
}

void GraphicObject::sendMouseMessage(GraphicObject *obj, UserEvent event)
{
    if (obj)
    {
        if (obj != m_lastSendedChild)
        {
            sendMouseLeaveMessage(m_lastSendedChild);
        }
        m_lastSendedChild = obj;
        obj->handleEvent(event);
    }
    else
    {
        GraphicObjectPtr sendTo = m_lastSendedChild;
        m_lastSendedChild = NULL;
        sendMouseLeaveMessage(sendTo);
    }
}

bool GraphicObject::drawIndexLessThan(GraphicObject *obj1, GraphicObject *obj2)
{
    return obj1->drawIndex() < obj2->drawIndex();
}

bool GraphicObject::drawIndexGreaterThan(GraphicObject *obj1, GraphicObject *obj2)
{
    return obj1->drawIndex() >= obj2->drawIndex();
}

bool GraphicObject::handleIndexLessThan(GraphicObject *obj1, GraphicObject *obj2)
{
    return obj1->handleIndex() < obj2->handleIndex();
}

bool GraphicObject::handleIndexGreaterThan(GraphicObject *obj1, GraphicObject *obj2)
{
    return obj1->handleIndex() >= obj2->handleIndex();
}
