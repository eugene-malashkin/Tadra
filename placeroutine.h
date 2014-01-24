#ifndef PLACEROUTINE_H
#define PLACEROUTINE_H

#include <QString>
#include <QRect>
#include <QCursor>

enum RectBoundType
{
    RectBoundTypeNull,
    RectBoundTypeTopLeft,
    RectBoundTypeTop,
    RectBoundTypeTopRight,
    RectBoundTypeRight,
    RectBoundTypeBottomRight,
    RectBoundTypeBottom,
    RectBoundTypeBottomLeft,
    RectBoundTypeLeft,
    RectBoundTypeTitle
};
QString rectBoundTypeToString(RectBoundType en);
QCursor rectBoundTypeToCursor(RectBoundType en);

RectBoundType findRectBoundType(const QRect &rect, int borderSize, int cornerSize, int titleSize, const QPoint &point);
bool isPointOnJointCorner(const QPoint &corner, int borderSize, int cornerSize, const QPoint &point);
RectBoundType findJointBoundType(const QRect &rect, int borderSize, int cornerSize, const QPoint &point);

#endif // PLACEROUTINE_H
