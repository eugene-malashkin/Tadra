#ifndef TABCONTROLLER_H
#define TABCONTROLLER_H

#include <QObject>
#include "tabdata.h"

class TabResponsibility
{
public:
    virtual void setData(const TabData &value) = 0;
    virtual TabData data() const = 0;
    virtual int dropIndex(const QPointF &globalPos) const = 0;
    virtual void moveCurrentTab(const QPointF &globalPos) = 0;
    virtual void fixCurrentTab() = 0;
    virtual void ceaseMoving() = 0;
};

class TabController : public QObject, public TabResponsibility
{
    Q_OBJECT

public:
    TabController(TabResponsibility *base, QObject *parent = NULL);
    TabResponsibility* base() const;
    void setData(const TabData &value) override;
    TabData data() const override;
    int dropIndex(const QPointF &globalPos) const override;
    void moveCurrentTab(const QPointF &globalPos) override;
    void fixCurrentTab() override;
    void ceaseMoving() override;

signals:
    void tabToBeActivated(int index);
    void tabToBeAdded();
    void tabToBeRemoved(int index);
    void tabToBeginDragging(QUuid uid);
    void tabToContinueDragging(QUuid uid, QPointF globalPos);
    void tabToBeDropped(QUuid uid, QPointF globalPos);

private:
    TabResponsibility *m_base;
};

#endif // TABCONTROLLER_H
