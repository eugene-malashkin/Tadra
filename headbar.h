#ifndef HEADBAR_H
#define HEADBAR_H

#include <QIcon>
#include "graphicwidget.h"
#include "tabcontroller.h"

class HeadBar;

typedef QMap<QUuid,double> NumberMap;

struct TabDrawInfo
{
    int index;
    QUuid uid;
    QRectF tabRect;
    QRectF textRect;
    QRectF closeButtonRect;
    QString text;
    QString elidedText;
    bool isHover;
    bool isActive;
    bool isMoving;
    QIcon::Mode iconMode;
    TabDrawInfo();
};
typedef QMap<QUuid,TabDrawInfo> TabDrawMap;

class TabSwitcherObject : public GraphicObject, public TabResponsibility
{
    Q_OBJECT

public:
    TabSwitcherObject(HeadBar *headBar);
    HeadBar* headBar() const;
    void setData(const TabData &value) override;
    TabData data() const override;
    int dropIndex(const QPointF &globalPos) const override;
    void moveCurrentTab(const QPointF &globalPos) override;
    void fixCurrentTab() override;
    void ceaseMoving() override;
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
    QIcon m_closeIcon;
    QRectF tabRectFormer(int index) const;
    QRectF newButtonRect() const;
    QRectF closeButtonRect(int index) const;
    NumberMap currentCoordinates() const;
    double currentTabWidth() const;
    TabDrawMap tabDrawMap() const;
    void paintTab(QPainter *painter, const TabDrawInfo &info);
};

class HeadBar : public GraphicWidget
{
    Q_OBJECT

public:
    HeadBar(QWidget *parent = NULL);
    TabController* tabController() const;

private:
    TabSwitcherObject *m_tabBar;
    TabController *m_tabController;
};

#endif // HEADBAR_H
