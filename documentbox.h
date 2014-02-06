#ifndef DOCUMENTBOX_H
#define DOCUMENTBOX_H

#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include "placeroutine.h"

class DocumentLayer;
class CurrencyChartWidget;
class GraphicWidget;
class DocumentBoxButtons;
class SearchInput;

class DocumentBox : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentBox(DocumentLayer *parent);
    DocumentLayer* layer() const;
    void setOrderIndex(int value);
    int orderIndex() const;
    void setStackRect(const QRectF &value);
    QRectF stackRect() const;
    void setGridRect(const QRect &value);
    QRect gridRect() const;
    void setScreenRect(const QRect &value);
    QRect screenRect() const;
    void setWideMode(bool value);
    bool isWideMode() const;

signals:
    void resizing(RectBoundType boundType, const QPoint &point);
    void moving(const QPoint &point);
    void wideModeChanging();
    void closing();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private slots:
    void onWideModeButtonClicked();
    void onCloseButtonClicked();
    void onInstrumentChanged();

private:
    DocumentLayer *m_layer;
    int m_orderIndex;
    QVBoxLayout *m_centralLayout;
    QHBoxLayout *m_headerLayout;
    SearchInput *m_searchInput;
    GraphicWidget *m_buttonsWidget;
    DocumentBoxButtons *m_buttonsObject;
    CurrencyChartWidget *m_chartWidget;
    QRectF m_stackRect;
    QRect m_gridRect;
    QRect m_screenRect;
    bool m_isWideMode;
    RectBoundType m_clickBound;
    QPoint m_clickPoint;
    RectBoundType findBound(const QPoint &point) const;
};

typedef QPointer<DocumentBox> DocumentBoxPtr;
typedef QList<DocumentBox*> DocumentBoxList;

#endif // DOCUMENTBOX_H
