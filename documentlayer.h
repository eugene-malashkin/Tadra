#ifndef DOCUMENTLAYER_H
#define DOCUMENTLAYER_H

#include <QWidget>
#include "base.h"
#include "gridscale.h"
#include "placeroutine.h"
#include "documentbox.h"
#include "gridcoordinategenerator.h"

class DocumentLayer : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentLayer(QWidget *parent = NULL);
    int gridSize() const;
    const GridScale& horizontalScale() const;
    const GridScale& verticalScale() const;
    QPoint gridToScreen(const QPoint &gridPoint) const;
    QPoint screenToGrid(const QPoint &screenPoint) const;
    QPointF roundScreenPoint(const QPointF &point) const;
    DocumentBox* createBox();
    DocumentBoxList boxes() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onBoxResizing(RectBoundType boundType, const QPoint &point);
    void onBoxMoving(const QPoint &point);
    void onBoxWideModeChanging();
    void onBoxClosing();

private:
    int m_gridSize;
    GridScale m_horizontalScale;
    GridScale m_verticalScale;
    GridCoordinateGenerator m_horizontalGridCoordinateGenerator;
    GridCoordinateGenerator m_verticalGridCoordinateGenerator;
    QPoint m_clickPoint;
    RectBoundType m_primaryBoxBound;
    QHash<DocumentBoxPtr, RectBoundType> m_boxDrags;

    QRectF computeOccupiedStackRect() const;
    QRect computeOccupiedGridRect() const;
    RectBoundType findBound(DocumentBox *box, const QPoint &point) const;
    void computeBound(const QPoint &point, DocumentBoxPtr &box, RectBoundType &bound) const;
    void buildBoxDrags(const QPoint &point);
    void adjustMinimumSize();
    QRect resizingBoxGridRect(DocumentBox *box, RectBoundType boundType, const QPoint &gridPoint) const;
    QRect resizingBoxRange(DocumentBox *box, RectBoundType boundType) const;
    void buildGridFromStack();
    void buildStackFromGrid();
    void buildScreenFromGrid();
    QSize minimumBoxGridSize() const;
    QSize computeMinimumGridSize() const;
    void stackCoordinatesChanged();
    DocumentBox* widenedBox() const;

    static void computeOccupiedTargetCoordinates(
            double occupiedStackCoordinate1,
            double occupiedStackCoordinate2,
            double minimalTargetSize,
            double actualTargetSize,
            double &occupiedTargetCoordinate1,
            double &occupiedTargetCoordinate2);
    static QPointF transform(const QPointF &p, const QRectF &s, const QRectF &d);
    static QRectF transform(const QRectF &p, const QRectF &s, const QRectF &d);
};

#endif // DOCUMENTLAYER_H
