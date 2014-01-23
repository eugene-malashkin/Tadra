#include "graphicwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include "hintwindow.h"

#include <QDebug>

static const int hintDelay = 200;

//******************************************************************************************************
/*!
 *\class GraphicWidget
 *\brief Класс - виджет для отображения графического объекта
*/
//******************************************************************************************************

GraphicWidget::GraphicWidget(QWidget *parent)
    :QWidget(parent)
    ,m_graphicObject(NULL)
    ,m_contextMenuShown(false)
    ,m_hintWindow(NULL)
    ,m_hintTimerID(0)
    ,m_lastMousePosition()
{
    setMouseTracking(true);
}

GraphicWidget::~GraphicWidget()
{
    killHintTimer();
    destroyHintWindow();
}

void GraphicWidget::setGraphicObject(GraphicObject *value)
{
    if (m_graphicObject != value)
    {
        m_graphicObject = value;
        applySizeConstraint();
        update();
    }
}

GraphicObject* GraphicWidget::graphicObject() const
{
    return m_graphicObject;
}

void GraphicWidget::redraw()
{
    // /Перерисовка виджета
    update();
}

void GraphicWidget::updateAndRedraw()
{
    if ((m_graphicObject != NULL))
    {
        m_graphicObject->update();
        applySizeConstraint();
        update();
    }
}

QPointF GraphicWidget::positionFromLocalToGlobal(const QPointF &p)
{
    QPointF result = mapToGlobal(p.toPoint());
    return result;
}

QPointF GraphicWidget::positionFromGlobalToLocal(const QPointF &p)
{
    QPointF result = mapFromGlobal(p.toPoint());
    return result;
}

void GraphicWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_hintTimerID)
    {
        if (processEvents())
        {
            if (!graphicObject()->handleUsing())
            {
                HitInfo hitInfo = graphicObject()->hitInfo(m_lastMousePosition);
                showHintWindow(m_lastMousePosition, hitInfo.hint);
            }
            else
            {
                destroyHintWindow();
            }
        }
        killHintTimer();
    }
}

void GraphicWidget::paintEvent(QPaintEvent *)
{
    if (processEvents())
    {
        QPainter painter(this);
        graphicObject()->paint(&painter);
    }
}

void GraphicWidget::resizeEvent(QResizeEvent *)
{
    if (processEvents())
    {
        QRectF rect(0, 0, width(), height());
        graphicObject()->setRect(rect);
        redraw();
    }
}

void GraphicWidget::mousePressEvent(QMouseEvent *event)
{
    if (processEvents())
    {
        bool processed = false;
        emit mousePress(event, processed);
        if (!processed)
        {
            UserEvent ue;
            ue.type = UserEvent::MouseDown;
            ue.button = event->button();
            ue.buttons = event->buttons();
            ue.mousePosition = event->pos();
            graphicObject()->handleEvent(ue);
            if (graphicObject()->handleUsing())
            {
                destroyHintWindow();
            }
        }
    }
}

void GraphicWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (processEvents())
    {
        bool processed = false;
        emit mouseRelease(event, processed);
        if (!processed)
        {
            UserEvent ue;
            ue.type = UserEvent::MouseUp;
            ue.button = event->button();
            ue.buttons = event->buttons();
            ue.mousePosition = event->pos();
            graphicObject()->handleEvent(ue);
        }
    }
}

void GraphicWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (processEvents() && (!m_contextMenuShown))
    {
        bool processed = false;
        emit mouseMove(event, processed);
        if (!processed)
        {
            UserEvent ue;
            ue.type = UserEvent::MouseMove;
            ue.mousePosition = event->pos();
            ue.buttons = event->buttons();
            graphicObject()->handleEvent(ue);

            HitInfo hitInfo = graphicObject()->hitInfo(event->pos());
            setCursor(hitInfo.cursor);

            if (m_hintTimerID == 0)
            {
                // Запускаем таймер на показ подсказки
                m_hintTimerID = startTimer(hintDelay);
            }
            m_lastMousePosition = event->pos();
        }
    }
}

void GraphicWidget::leaveEvent(QEvent *)
{
    killHintTimer();
    destroyHintWindow();
    if (processEvents())
    {
        UserEvent ue;
        ue.type = UserEvent::MouseLeave;
        graphicObject()->handleEvent(ue);
    }
}

void GraphicWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (processEvents())
    {
        UserEvent ue;
        ue.type = UserEvent::MouseDoubleClick;
        ue.button = event->button();
        ue.mousePosition = event->pos();
        graphicObject()->handleEvent(ue);
    }
}

void GraphicWidget::keyPressEvent(QKeyEvent *event)
{
    emit keyPressed(event);
    if (processEvents())
    {
        UserEvent ue;
        ue.type = UserEvent::KeyPress;
        ue.key = event->key();
        graphicObject()->handleEvent(ue);
    }
}

void GraphicWidget::keyReleaseEvent(QKeyEvent *event)
{
    emit keyReleased(event);
    if (processEvents())
    {
        UserEvent ue;
        ue.type = UserEvent::KeyRelease;
        ue.key = event->key();
        graphicObject()->handleEvent(ue);
    }
}

void GraphicWidget::hideEvent(QHideEvent *)
{
    killHintTimer();
    destroyHintWindow();
}

bool GraphicWidget::processEvents() const
{
    return (m_graphicObject != NULL);
}

void GraphicWidget::killHintTimer()
{
    if (m_hintTimerID > 0)
    {
        killTimer(m_hintTimerID);
        m_hintTimerID = 0;
    }

}

void GraphicWidget::destroyHintWindow()
{
    if (m_hintWindow != NULL)
    {
        m_hintWindow->hide();
        m_hintWindow->deleteLater();
        m_hintWindow = NULL;
    }
}

void GraphicWidget::showHintWindow(const QPointF &hintPosition, const QString &hintText)
{
    destroyHintWindow();
    if (!hintText.isEmpty())
    {
        m_hintWindow = new HintWindow(this);
        m_hintWindow->setText(hintText);
        QPoint cursorPosition = mapToGlobal(QPoint(hintPosition.x(), hintPosition.y()));
        m_hintWindow->showAt(cursorPosition);
    }
}

void GraphicWidget::applySizeConstraint()
{
    QSizeF sc = m_graphicObject->sizeConstraint(size());
    setMinimumSize(QSize(sc.width(), sc.height()));
}
