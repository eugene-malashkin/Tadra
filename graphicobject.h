#ifndef GRAPHICOBJECT_H
#define GRAPHICOBJECT_H

#include <QObject>
#include <QPainter>
#include <QList>
#include <QPointer>
#include <QVector>
#include <QSizeF>
#include <QCursor>

class GraphicObject;
typedef QList<GraphicObject*> GraphicObjectList;
typedef QPointer<GraphicObject> GraphicObjectPtr;

class AbstractGraphicSupervisor
{
public:
    virtual void redraw() = 0;
    virtual void updateAndRedraw() = 0;
    virtual QPointF positionFromLocalToGlobal(const QPointF &p) = 0;
    virtual QPointF positionFromGlobalToLocal(const QPointF &p) = 0;
};

struct UserEvent
{
    enum Type
    {
        Unknown,
        MouseDown,
        MouseUp,
        MouseMove,
        MouseLeave,
        MouseDoubleClick,
        KeyPress,
        KeyRelease
    };
    Type type;
    Qt::MouseButton button;
    Qt::MouseButtons buttons;
    QPoint mousePosition;
    int key;
    UserEvent();
    QString toString() const;
};

struct HitInfo
{
    bool result;
    QCursor cursor;
    QString hint;
    HitInfo();
};

class GraphicObject : public QObject
{
    Q_OBJECT

public:
    GraphicObject(QObject *parent = NULL);
    void setSupervisor(AbstractGraphicSupervisor *value);
    AbstractGraphicSupervisor* supervisor() const;
    int registerChildObject(GraphicObject *obj, bool autoComputeOrderIndex = true);
    bool unregisterChildObject(GraphicObject *obj);
    bool unregisterChildObject(int objIndex);
    GraphicObjectList childrenObjects() const;
    GraphicObjectList childrenVisibleObjects(bool orderDesc = false, bool orderByHandleIndex = false) const;
    void bringChildObjectToFront(GraphicObject *obj);
    void moveChildObjectBackwards(GraphicObject *obj);
    void moveChildObjectForwards(GraphicObject *obj);
    void putChildObjectToBack(GraphicObject *obj);
    void setRequestVisible(bool value);
    bool requestVisible() const;
    void setCanVisible(bool value);
    bool canVisible() const;
    virtual bool visible() const;
    void setOrderIndex(int value);
    int orderIndex() const;
    void setDrawIndex(int value);
    int drawIndex() const;
    void setHandleIndex(int value);
    int handleIndex() const;
    void setRect(const QRectF &supposedRect);
    void setRect(double left, double top, double width, double height);
    QRectF rect() const;
    void setHandleUsing(bool value);
    bool handleUsing() const;
    virtual void paint(QPainter *painter);
    virtual QSizeF sizeConstraint(const QSizeF &supposedSize) const;
    virtual void resize();
    virtual void update();
    virtual HitInfo hitInfo(const QPointF &point) const;
    virtual void handleEvent(UserEvent event);
    void redraw();
    void updateAndRedraw();

protected:
    void paintChildrenObjects(QPainter *painter);
    void setRectToChildrenObjects(const QRectF &rect);
    void updateChildrenObjects();
    GraphicObject* findHitChildObject(const QPointF &point, HitInfo &hitInfo) const;
    void handleEventByChildrenObjects(UserEvent event);
    static QString wrapHintToHtml(const QString &text);

private:
    AbstractGraphicSupervisor *m_supervisor;
    bool m_requestVisiuble;
    bool m_canVisible;
    int m_drawIndex;
    int m_handleIndex;
    GraphicObjectList m_childrenObjects;
    QRectF m_rect;
    bool m_handleUsing;
    GraphicObjectPtr m_lastSendedChild;
    void assignOrderIndexes();
    GraphicObject* findHandleUsingChildObject() const;
    void sendMouseLeaveMessage(GraphicObject *obj);
    void sendKeyMessage(UserEvent event);
    void sendMouseMessage(GraphicObject *obj, UserEvent event);
    static bool drawIndexLessThan(GraphicObject *obj1, GraphicObject *obj2);
    static bool drawIndexGreaterThan(GraphicObject *obj1, GraphicObject *obj2);
    static bool handleIndexLessThan(GraphicObject *obj1, GraphicObject *obj2);
    static bool handleIndexGreaterThan(GraphicObject *obj1, GraphicObject *obj2);
};

#endif // GRAPHICOBJECT_H
