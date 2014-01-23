#ifndef GRAPHICWIDGET_H
#define GRAPHICWIDGET_H

#include <QWidget>
#include "graphicobject.h"

class HintWindow;

class GraphicWidget : public QWidget, public AbstractGraphicSupervisor
{
    Q_OBJECT

public:
    GraphicWidget(QWidget *parent = NULL);
    ~GraphicWidget() override;
    void setGraphicObject(GraphicObject *value);
    GraphicObject* graphicObject() const;
    void redraw() override;
    void updateAndRedraw() override;
    QPointF positionFromLocalToGlobal(const QPointF &p) override;
    QPointF positionFromGlobalToLocal(const QPointF &p) override;

signals:
    void keyPressed(QKeyEvent *event);
    void keyReleased(QKeyEvent *event);
    void mousePress(QMouseEvent *event, bool &processed);
    void mouseRelease(QMouseEvent *event, bool &processed);
    void mouseMove(QMouseEvent *event, bool &processed);

protected:
    void timerEvent(QTimerEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    GraphicObject *m_graphicObject;
    bool m_contextMenuShown;
    HintWindow *m_hintWindow;
    int m_hintTimerID;
    QPointF m_lastMousePosition;
    bool processEvents() const;
    void killHintTimer();
    void destroyHintWindow();
    void showHintWindow(const QPointF &hintPosition, const QString &hintText);
    void applySizeConstraint();
};

#endif // GRAPHICWIDGET_H
