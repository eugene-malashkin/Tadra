#ifndef HEADBAR_H
#define HEADBAR_H

#include "graphicwidget.h"
#include "tabcontroller.h"

class HeadBar;

class TabBar : public GraphicObject, public TabResponsibility
{
    Q_OBJECT

public:
    TabBar(HeadBar *headBar);
    HeadBar* headBar() const;
    void setData(const TabData &value) override;
    TabData data() const override;
    int dropIndex(const QPointF &globalPos) const override;
    void moveCurrentTab(const QPointF &globalPos) override;
    void fixCurrentTab() override;
    TabController* tabController() const;
    void paint(QPainter *painter) override;
    void handleEvent(UserEvent event) override;
    QSizeF sizeConstraint(const QSizeF &supposedSize) const override;

private:
    HeadBar *m_headBar;
    TabData m_data;
    bool m_isMovingCurrentTab;
    QPointF m_movingCurrentTabPos;
    QPointF m_clickPoint;
    QPointF m_clickOffset;
    QUuid m_clickTabUid;
    bool m_isDragging;
    QRectF tabRect(int index) const;
    QRectF newButtonRect() const;
    QRectF closeButtonRect(int index) const;
};

class HeadBar : public GraphicWidget
{
    Q_OBJECT

public:
    HeadBar(QWidget *parent = NULL);
    TabController* tabController() const;

private:
    TabBar *m_tabBar;
    TabController *m_tabController;
};

#endif // HEADBAR_H
