#include "placeroutine.h"

QString rectBoundTypeToString(RectBoundType en)
{
    QString result;
    if (en == RectBoundTypeNull) result = "RectBoundTypeNull";
    if (en == RectBoundTypeTopLeft) result = "RectBoundTypeTopLeft";
    if (en == RectBoundTypeTop) result = "RectBoundTypeTop";
    if (en == RectBoundTypeTopRight) result = "RectBoundTypeTopRight";
    if (en == RectBoundTypeRight) result = "RectBoundTypeRight";
    if (en == RectBoundTypeBottomRight) result = "RectBoundTypeBottomRight";
    if (en == RectBoundTypeBottom) result = "RectBoundTypeBottom";
    if (en == RectBoundTypeBottomLeft) result = "RectBoundTypeBottomLeft";
    if (en == RectBoundTypeLeft) result = "RectBoundTypeLeft";
    if (en == RectBoundTypeTitle) result = "RectBoundTypeTitle";
    return result;
}

QCursor rectBoundTypeToCursor(RectBoundType en)
{
    QCursor result = Qt::ArrowCursor;
    switch (en) {
    case RectBoundTypeTopLeft:
    case RectBoundTypeBottomRight:
        result = Qt::SizeFDiagCursor;
        break;
    case RectBoundTypeTopRight:
    case RectBoundTypeBottomLeft:
        result = Qt::SizeBDiagCursor;
        break;
    case RectBoundTypeTop:
    case RectBoundTypeBottom:
        result = Qt::SizeVerCursor;
        break;
    case RectBoundTypeRight:
    case RectBoundTypeLeft:
        result = Qt::SizeHorCursor;
        break;
    default:
        break;
    }
    return result;
}

RectBoundType findRectBoundType(const QRect &rect, int borderSize, int cornerSize, int titleSize, const QPoint &point)
{
    RectBoundType result = RectBoundTypeNull;
    if (result == RectBoundTypeNull)
    {
        QRect corner1(rect.left(), rect.top(), cornerSize, borderSize);
        QRect corner2(rect.left(), rect.top(), borderSize, cornerSize);
        if ((corner1.contains(point)) || (corner2.contains(point)))
        {
            result = RectBoundTypeTopLeft;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect corner1(rect.right()-cornerSize, rect.top(), cornerSize, borderSize);
        QRect corner2(rect.right()-borderSize, rect.top(), borderSize, cornerSize);
        if ((corner1.contains(point)) || (corner2.contains(point)))
        {
            result = RectBoundTypeTopRight;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect corner1(rect.right()-cornerSize, rect.bottom()-borderSize, cornerSize, borderSize);
        QRect corner2(rect.right()-borderSize, rect.bottom()-cornerSize, borderSize, cornerSize);
        if ((corner1.contains(point)) || (corner2.contains(point)))
        {
            result = RectBoundTypeBottomRight;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect corner1(rect.left(), rect.bottom()-borderSize, cornerSize, borderSize);
        QRect corner2(rect.left(), rect.bottom()-cornerSize, borderSize, cornerSize);
        if ((corner1.contains(point)) || (corner2.contains(point)))
        {
            result = RectBoundTypeBottomLeft;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.left(), rect.top(), rect.width(), borderSize);
        if (bound.contains(point))
        {
            result = RectBoundTypeTop;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.right()-borderSize, rect.top(), borderSize, rect.height());
        if (bound.contains(point))
        {
            result = RectBoundTypeRight;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.left(), rect.bottom()-borderSize, rect.width(), borderSize);
        if (bound.contains(point))
        {
            result = RectBoundTypeBottom;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.left(), rect.top(), borderSize, rect.height());
        if (bound.contains(point))
        {
            result = RectBoundTypeLeft;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.left(), rect.top(), rect.width(), titleSize);
        if (bound.contains(point))
        {
            result = RectBoundTypeTitle;
        }
    }
    return result;
}

bool isPointOnJointCorner(const QPoint &corner, int borderSize, int cornerSize, const QPoint &point)
{
    QRect cornerRect1(corner.x()-cornerSize, corner.y()-borderSize, cornerSize*2, borderSize*2);
    QRect cornerRect2(corner.x()-borderSize, corner.y()-cornerSize, borderSize*2, cornerSize*2);
    return (cornerRect1.contains(point) || cornerRect2.contains(point));
}

RectBoundType findJointBoundType(const QRect &rect, int borderSize, int cornerSize, const QPoint &point)
{
    RectBoundType result = RectBoundTypeNull;
    if (result == RectBoundTypeNull)
    {
        if (isPointOnJointCorner(rect.topLeft(), borderSize, cornerSize, point))
        {
            result = RectBoundTypeTopLeft;
        }
    }
    if (result == RectBoundTypeNull)
    {
        if (isPointOnJointCorner(rect.topRight(), borderSize, cornerSize, point))
        {
            result = RectBoundTypeTopRight;
        }
    }
    if (result == RectBoundTypeNull)
    {
        if (isPointOnJointCorner(rect.bottomRight(), borderSize, cornerSize, point))
        {
            result = RectBoundTypeBottomRight;
        }
    }
    if (result == RectBoundTypeNull)
    {
        if (isPointOnJointCorner(rect.bottomLeft(), borderSize, cornerSize, point))
        {
            result = RectBoundTypeBottomLeft;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.left(), rect.top() - borderSize, rect.width(), borderSize*2);
        if (bound.contains(point))
        {
            result = RectBoundTypeTop;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.right()-borderSize, rect.top(), borderSize*2, rect.height());
        if (bound.contains(point))
        {
            result = RectBoundTypeRight;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.left(), rect.bottom()-borderSize, rect.width(), borderSize*2);
        if (bound.contains(point))
        {
            result = RectBoundTypeBottom;
        }
    }
    if (result == RectBoundTypeNull)
    {
        QRect bound(rect.left()-borderSize, rect.top(), borderSize*2, rect.height());
        if (bound.contains(point))
        {
            result = RectBoundTypeLeft;
        }
    }
    return result;
}
