#include "documentlayer.h"
#include "documentbox.h"
#include <QPainter>
#include <QMouseEvent>
#include "floatroutine.h"
#include "design.h"

#include <QDebug>

DocumentLayer::DocumentLayer(QWidget *parent)
    :QWidget(parent)
    ,m_gridSize(16)
    ,m_horizontalScale()
    ,m_verticalScale()
    ,m_horizontalGridCoordinateGenerator()
    ,m_verticalGridCoordinateGenerator()
    ,m_clickPoint()
    ,m_primaryBoxBound(RectBoundTypeNull)
    ,m_boxDrags()
{
    m_horizontalScale.setGridSize(m_gridSize);
    m_verticalScale.setGridSize(m_gridSize);

    m_horizontalGridCoordinateGenerator.setMinimalSegmentGridLength(minimumBoxGridSize().width());
    m_verticalGridCoordinateGenerator.setMinimalSegmentGridLength(minimumBoxGridSize().height());
    setMouseTracking(true);
}

int DocumentLayer::gridSize() const
{
    return m_gridSize;
}

const GridScale& DocumentLayer::horizontalScale() const
{
    return m_horizontalScale;
}

const GridScale& DocumentLayer::verticalScale() const
{
    return m_verticalScale;
}

QPoint DocumentLayer::gridToScreen(const QPoint &gridPoint) const
{
    return QPoint(m_horizontalScale.gridToScreen(gridPoint.x()), m_verticalScale.gridToScreen(gridPoint.y()));
}

QPoint DocumentLayer::screenToGrid(const QPoint &screenPoint) const
{
    return QPoint(m_horizontalScale.screenToGrid(screenPoint.x()), m_verticalScale.screenToGrid(screenPoint.y()));
}

QPointF DocumentLayer::roundScreenPoint(const QPointF &point) const
{
    return QPointF(m_horizontalScale.roundScreen(point.x()), m_verticalScale.roundScreen(point.y()));
}

DocumentBox* DocumentLayer::createBox()
{
    DocumentBox *result = new DocumentBox(this);
    connect(result, SIGNAL(resizing(RectBoundType,QPoint)), this, SLOT(onBoxResizing(RectBoundType,QPoint)));
    connect(result, SIGNAL(moving(QPoint)), this, SLOT(onBoxMoving(QPoint)));
    connect(result, SIGNAL(wideModeChanging()), this, SLOT(onBoxWideModeChanging()));
    connect(result, SIGNAL(closing()), this, SLOT(onBoxClosing()));
    result->setStackRect(QRect(0, 0, 1, 1));
    stackCoordinatesChanged();
    adjustMinimumSize();
    buildGridFromStack();
    buildScreenFromGrid();
    result->setVisible(true);
    return result;
}

DocumentBoxList DocumentLayer::boxes() const
{
    return findChildren<DocumentBox*>();
}

void DocumentLayer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(0, 0, width(), height(), Design::instance()->color(Design::DocumentLayerBgColor));
    //painter.fillRect(0, 0, width(), height(), Qt::yellow);

    /*
    for (int i = 0; i <= m_horizontalScale.gridCount(); i++)
    {
        double x = m_horizontalScale.gridToScreen(i);
        painter.drawLine(x, 0, x, height());
    }
    for (int i = 0; i <= m_verticalScale.gridCount(); i++)
    {
        double y = m_verticalScale.gridToScreen(i);
        painter.drawLine(0, y, width(), y);
    }
    */
}

void DocumentLayer::resizeEvent(QResizeEvent *)
{
    m_horizontalScale.setScreenSize(width());
    m_verticalScale.setScreenSize(height());
    buildGridFromStack();
    buildScreenFromGrid();
}

void DocumentLayer::mousePressEvent(QMouseEvent * event)
{
    m_boxDrags.clear();

    if (event->button() == Qt::LeftButton)
    {
        m_clickPoint = event->pos();
        buildBoxDrags(m_clickPoint);
    }
}

void DocumentLayer::mouseMoveEvent(QMouseEvent * event)
{
    if (m_boxDrags.isEmpty())
    {
        DocumentBoxPtr box;
        RectBoundType bound;
        computeBound(event->pos(), box, bound);
        setCursor(rectBoundTypeToCursor(bound));
    }
    else
    {
        // Можно ли проводить действия над группой коробок
        QRect range(0, 0, m_horizontalScale.gridCount()+1, m_verticalScale.gridCount()+1);
        for (auto iter = m_boxDrags.constBegin(); iter != m_boxDrags.constEnd(); ++iter)
        {
            DocumentBox *box = iter.key();
            QRect boxRange = resizingBoxRange(box, iter.value());
            range = range.intersected(boxRange);
        }

        QPoint gridPoint = screenToGrid(event->pos());
        QPoint inRangeGridPoint(
                    qBound(range.left(), gridPoint.x(), range.right()),
                    qBound(range.top(), gridPoint.y(), range.bottom()));

        bool hasChanges = false;
        for (auto iter = m_boxDrags.constBegin(); iter != m_boxDrags.constEnd(); ++iter)
        {
            DocumentBox *box = iter.key();
            QRect gridRect = resizingBoxGridRect(box, iter.value(), inRangeGridPoint);
            if (box->gridRect() != gridRect)
            {
                box->setGridRect(gridRect);
                hasChanges = true;
            }
        }
        if (hasChanges)
        {
            buildStackFromGrid();
            adjustMinimumSize();
            buildScreenFromGrid();
            update();
        }

        setCursor(rectBoundTypeToCursor(m_primaryBoxBound));
    }
}

void DocumentLayer::mouseReleaseEvent(QMouseEvent * /*event*/)
{
    m_boxDrags.clear();
}

void DocumentLayer::leaveEvent(QEvent *)
{

}

void DocumentLayer::onBoxResizing(RectBoundType boundType, const QPoint &point)
{
    DocumentBox *box = qobject_cast<DocumentBox*>(sender());
    if (box == NULL)
    {
        return;
    }

    QRect gridRect = resizingBoxGridRect(box, boundType, screenToGrid(point));
    if (box->gridRect() != gridRect)
    {
        box->setGridRect(gridRect);
        buildStackFromGrid();
        adjustMinimumSize();
        buildScreenFromGrid();
        update();
    }
}

void DocumentLayer::onBoxMoving(const QPoint &point)
{
    DocumentBox *box = qobject_cast<DocumentBox*>(sender());
    if (box == NULL)
    {
        return;
    }

    QRect gridRect = box->gridRect();
    QPoint gridPoint = screenToGrid(point);
    gridRect = QRect(
                qBound(0, gridPoint.x(), m_horizontalScale.gridCount() - gridRect.width()),
                qBound(0, gridPoint.y(), m_verticalScale.gridCount() - gridRect.height()),
                gridRect.width(),
                gridRect.height()
                );
    if (box->gridRect() != gridRect)
    {
        box->setGridRect(gridRect);
        buildStackFromGrid();
        adjustMinimumSize();
        buildScreenFromGrid();
        update();
    }
}

void DocumentLayer::onBoxWideModeChanging()
{
    DocumentBox *box = qobject_cast<DocumentBox*>(sender());
    if (box == NULL)
    {
        return;
    }

    bool isWideMode = !(box->isWideMode());
    box->setWideMode(isWideMode);
    box->raise();
    buildScreenFromGrid();
}

void DocumentLayer::onBoxClosing()
{
    DocumentBox *box = qobject_cast<DocumentBox*>(sender());
    if (box == NULL)
    {
        return;
    }

    box->setVisible(false);
    box->setParent(NULL);
    box->deleteLater();
    stackCoordinatesChanged();
    adjustMinimumSize();
}

QRectF DocumentLayer::computeOccupiedStackRect() const
{
    QRectF result;
    DocumentBoxList list = boxes();
    foreach (DocumentBox *box, list)
    {
        if (result.isNull())
        {
            result = box->stackRect();
        }
        else
        {
            result = result.united(box->stackRect());
        }
    }
    return result;
}

QRect DocumentLayer::computeOccupiedGridRect() const
{
    QRect result;
    DocumentBoxList list = boxes();
    foreach (DocumentBox *box, list)
    {
        if (result.isNull())
        {
            result = box->gridRect();
        }
        else
        {
            result = result.united(box->gridRect());
        }
    }
    return result;
}

RectBoundType DocumentLayer::findBound(DocumentBox *box, const QPoint &point) const
{
    RectBoundType result = findJointBoundType(
                box->screenRect(),
                Design::instance()->size(Design::DocumentBoxBorderSize),
                Design::instance()->size(Design::DocumentBoxCornerSize), point);
    return result;
}

void DocumentLayer::computeBound(const QPoint &point, DocumentBoxPtr &box, RectBoundType &bound) const
{
    box = NULL;
    bound = RectBoundTypeNull;

    DocumentBoxList list = boxes();
    for (int i = list.count()-1; i >= 0; i--)
    {
        RectBoundType currentBound = findBound(list[i], point);
        if (currentBound != RectBoundTypeNull)
        {
            box = list[i];
            bound = currentBound;
            break;
        }
    }
}

void DocumentLayer::buildBoxDrags(const QPoint &point)
{
    m_primaryBoxBound = RectBoundTypeNull;
    m_boxDrags.clear();

    // Первая коробка
    int firstBoxIndex = -1;
    RectBoundType firstBoundType = RectBoundTypeNull;

    DocumentBoxList list = boxes();
    for (int i = list.count()-1; i >= 0; i--)
    {
        RectBoundType boundType = findBound(list[i], point);
        if (boundType != RectBoundTypeNull)
        {
            bool doAdd = false;
            if (firstBoxIndex < 0)
            {
                firstBoxIndex = i;
                firstBoundType = boundType;
                m_primaryBoxBound = boundType;
                doAdd = true;
            }
            else if ((i != firstBoxIndex) && (boundType != firstBoundType))
            {
                doAdd = true;
            }
            if (doAdd)
            {
                m_boxDrags[list[i]] = boundType;
            }
        }
    }
}

void DocumentLayer::adjustMinimumSize()
{
    QSize minimumGridSize = computeMinimumGridSize();
    setMinimumSize(minimumGridSize.width()*m_gridSize, minimumGridSize.height()*m_gridSize);
}

QRect DocumentLayer::resizingBoxGridRect(DocumentBox *box, RectBoundType boundType, const QPoint &gridPoint) const
{
    QRect range = resizingBoxRange(box, boundType);
    QPoint inRangeGridPoint(
                qBound(range.left(), gridPoint.x(), range.right()),
                qBound(range.top(), gridPoint.y(), range.bottom()));

    QRect result = box->gridRect();

    if ((boundType == RectBoundTypeTop) || (boundType == RectBoundTypeTopLeft) || (boundType == RectBoundTypeTopRight))
    {
        result.setTop(inRangeGridPoint.y());
    }
    if ((boundType == RectBoundTypeBottom) || (boundType == RectBoundTypeBottomLeft) || (boundType == RectBoundTypeBottomRight))
    {
        result.setBottom(inRangeGridPoint.y()-1);
    }
    if ((boundType == RectBoundTypeLeft) || (boundType == RectBoundTypeTopLeft) || (boundType == RectBoundTypeBottomLeft))
    {
        result.setLeft(inRangeGridPoint.x());
    }
    if ((boundType == RectBoundTypeRight) || (boundType == RectBoundTypeTopRight) || (boundType == RectBoundTypeBottomRight))
    {
        result.setRight(inRangeGridPoint.x()-1);
    }
    return result;
}

QRect DocumentLayer::resizingBoxRange(DocumentBox *box, RectBoundType boundType) const
{
    QRect result(0, 0, m_horizontalScale.gridCount()+1, m_verticalScale.gridCount()+1);

    // Текущие координаты коробки
    QRect gridRect = box->gridRect();

    // Сколько минимально прибавлять от противоположного угла
    int minIncWidth = minimumBoxGridSize().width() - 1;
    int minIncHeight = minimumBoxGridSize().height() - 1;

    if ((boundType == RectBoundTypeTop) || (boundType == RectBoundTypeTopLeft) || (boundType == RectBoundTypeTopRight))
    {
        result.setBottom(gridRect.bottom() - minIncHeight); // max Y
    }
    if ((boundType == RectBoundTypeBottom) || (boundType == RectBoundTypeBottomLeft) || (boundType == RectBoundTypeBottomRight))
    {
        result.setTop(gridRect.top() + minIncHeight + 1); // min Y
    }
    if ((boundType == RectBoundTypeLeft) || (boundType == RectBoundTypeTopLeft) || (boundType == RectBoundTypeBottomLeft))
    {
        result.setRight(gridRect.right() - minIncWidth); // max X
    }
    if ((boundType == RectBoundTypeRight) || (boundType == RectBoundTypeTopRight) || (boundType == RectBoundTypeBottomRight))
    {
        result.setLeft(gridRect.left() + minIncWidth + 1); // min X
    }
    return result;
}

void DocumentLayer::buildGridFromStack()
{
    QSize minimumGridSize = computeMinimumGridSize();
    QRectF occupiedStackRect = computeOccupiedStackRect();
    double gridFloatX1 = 0;
    double gridFloatX2 = 0;
    double gridFloatY1 = 0;
    double gridFloatY2 = 0;
    computeOccupiedTargetCoordinates(occupiedStackRect.left(), occupiedStackRect.right(), minimumGridSize.width(), m_horizontalScale.gridCount(), gridFloatX1, gridFloatX2);
    computeOccupiedTargetCoordinates(occupiedStackRect.top(), occupiedStackRect.bottom(), minimumGridSize.height(), m_verticalScale.gridCount(), gridFloatY1, gridFloatY2);

    GridSegment horizontalGridSpace(iround(gridFloatX1), iround(gridFloatX2));
    m_horizontalGridCoordinateGenerator.setSupposedGridSpace(horizontalGridSpace);
    GridSegment verticalGridSpace(iround(gridFloatY1), iround(gridFloatY2));
    m_verticalGridCoordinateGenerator.setSupposedGridSpace(verticalGridSpace);

    GridSegmentList horizontalGridCoordinates = m_horizontalGridCoordinateGenerator.gridSegmentList();
    GridSegmentList verticalGridCoordinates = m_verticalGridCoordinateGenerator.gridSegmentList();

    DocumentBoxList list = boxes();
    for (int i = 0; i < list.count(); i++)
    {
        DocumentBox *box = list[i];
        QRect gridRect(
                    horizontalGridCoordinates[i].min,
                    verticalGridCoordinates[i].min,
                    horizontalGridCoordinates[i].max - horizontalGridCoordinates[i].min,
                    verticalGridCoordinates[i].max - verticalGridCoordinates[i].min
                    );
        box->setGridRect(gridRect);
    }
}

void DocumentLayer::buildStackFromGrid()
{
    QRect fullGridRect = computeOccupiedGridRect();
    fullGridRect = fullGridRect.united(QRect(0, 0, m_horizontalScale.gridCount(), m_verticalScale.gridCount()));
    QRect fullStackRect(0, 0, 1, 1);
    DocumentBoxList list = boxes();
    foreach (DocumentBox *box, list)
    {
        QRectF stackRect = transform(box->gridRect(), fullGridRect, fullStackRect);
        box->setStackRect(stackRect);
    }
    stackCoordinatesChanged();
}

void DocumentLayer::buildScreenFromGrid()
{
    DocumentBox *wb = widenedBox();
    if (wb != NULL)
    {
        // Есть распахнутый документ
        wb->setGeometry(QRect(0, 0, width(), height()));
    }
    else
    {
        // Все документы в обычном режиме
        int fs = Design::instance()->size(Design::DocumentBoxSpacing);
        int hs = fs / 2;
        DocumentBoxList list = boxes();
        foreach (DocumentBox *box, list)
        {
            QRect gridRect = box->gridRect();
            int screenX1 = m_horizontalScale.gridToScreen(gridRect.left());
            int screenY1 = m_verticalScale.gridToScreen(gridRect.top());
            int screenX2 = m_horizontalScale.gridToScreen(gridRect.right()+1);
            int screenY2 = m_verticalScale.gridToScreen(gridRect.bottom()+1);
            QRect screenRect(screenX1, screenY1, screenX2 - screenX1 + 1, screenY2 - screenY1 + 1);
            box->setScreenRect(screenRect);
            QRect geometryRect = screenRect.adjusted(
                        (gridRect.left() <= 0) ? fs : hs,
                        (gridRect.top() <= 0) ? fs : hs,
                        (gridRect.right() >= m_horizontalScale.gridCount()-1) ? -fs : -hs,
                        (gridRect.bottom() >= m_verticalScale.gridCount()-1) ? -fs : -hs
                        );
            box->setGeometry(geometryRect);
        }
    }
}

QSize DocumentLayer::minimumBoxGridSize() const
{
    return QSize(
                Design::instance()->size(Design::DocumentBoxMinimumGridWidth),
                Design::instance()->size(Design::DocumentBoxMinimumGridHeight));
}

QSize DocumentLayer::computeMinimumGridSize() const
{
    QSize result(m_horizontalGridCoordinateGenerator.minimalGridSpaceLength(), m_verticalGridCoordinateGenerator.minimalGridSpaceLength());
    return result;
}

void DocumentLayer::stackCoordinatesChanged()
{
    StackSegmentList inputHorizontal;
    StackSegmentList inputVertical;
    DocumentBoxList list = boxes();
    foreach (DocumentBox *box, list)
    {
        QRectF stackRect = box->stackRect();
        inputHorizontal << StackSegment(stackRect.left(), stackRect.right());
        inputVertical << StackSegment(stackRect.top(), stackRect.bottom());
    }
    m_horizontalGridCoordinateGenerator.setStackSegmentList(inputHorizontal);
    m_verticalGridCoordinateGenerator.setStackSegmentList(inputVertical);
}

DocumentBox* DocumentLayer::widenedBox() const
{
    DocumentBox *result = NULL;
    DocumentBoxList list = boxes();
    foreach (DocumentBox *box, list)
    {
        if (box->isWideMode())
        {
            result = box;
            break;
        }
    }
    return result;
}

void DocumentLayer::computeOccupiedTargetCoordinates(double occupiedStackCoordinate1,
        double occupiedStackCoordinate2,
        double minimalTargetSize,
        double actualTargetSize,
        double &occupiedTargetCoordinate1,
        double &occupiedTargetCoordinate2)
{
    double supposedScreenSize = occupiedStackCoordinate2*actualTargetSize - occupiedStackCoordinate1*actualTargetSize;
    if (supposedScreenSize >= minimalTargetSize)
    {
        occupiedTargetCoordinate1 = occupiedStackCoordinate1 * actualTargetSize;
        occupiedTargetCoordinate2 = occupiedStackCoordinate2 * actualTargetSize;
    }
    else
    {
        occupiedTargetCoordinate1 = (actualTargetSize - minimalTargetSize)/2.0;
        occupiedTargetCoordinate2 = (actualTargetSize + minimalTargetSize)/2.0;
        double offset = 0;
        if (actualTargetSize > minimalTargetSize)
        {
            offset = occupiedStackCoordinate1 / (occupiedStackCoordinate1 + 1.0 - occupiedStackCoordinate2) * (actualTargetSize - minimalTargetSize) - occupiedTargetCoordinate1;
        }
        occupiedTargetCoordinate1 += offset;
        occupiedTargetCoordinate2 += offset;
    }
}

QPointF DocumentLayer::transform(const QPointF &p, const QRectF &s, const QRectF &d)
{
    return QPointF(
                (p.x() - s.left())/s.width()*d.width() + d.left(),
                (p.y() - s.top())/s.height()*d.height() + d.top()
                );
}

QRectF DocumentLayer::transform(const QRectF &p, const QRectF &s, const QRectF &d)
{
    QPointF topLeft(transform(p.topLeft(), s, d));
    QPointF bottomRight(transform(p.bottomRight(), s, d));
    return QRectF(topLeft, bottomRight);
}
