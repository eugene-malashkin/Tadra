#ifndef DESIGN_H
#define DESIGN_H

#include "singletont.h"
#include <QFont>
#include <QColor>

class Design : public SingletonT<Design>
{
public:
    enum SizeKey
    {
        DocumentLayerGridSize,
        DocumentBoxBorderSize,
        DocumentBoxCornerSize,
        DocumentBoxTitleSize,
        DocumentBoxSpacing,
        DocumentBoxMinimumGridWidth,
        DocumentBoxMinimumGridHeight,
        DocumentBoxButtonSpacing,
        HeadBarMinimalWidth,
        HeadBarMinimalHeight,
        HeadBarLeftMargin,
        GraphicButtonDefaultIconSize,
        GraphicButtonIconTextSpacing,
        TabMinWidth,
        TabMaxWidth,
        TabEmbryoWidth,
        TabEmbryoHeight,
        TabEmbryoSpacing,
        TabElementSpacing,
        TabSwitcherMinWidth,
        TabSwitcherMinHeight,
        TabRadius
    };

    enum ColorRey
    {
        DocumentLayerBgColor,
        DocumentBoxBgColor,
        DocumentBoxOutFocusLineColor,
        DocumentBoxInFocusLineColor,
        HeadBarBgColor,
        GraphicButtonTextColor,
        TabLineColor,
        TabNormalBgColor,
        TabNormalTextColor,
        TabHoverBgColor,
        TabHoverTextColor,
        TabActiveBgColor,
        TabActiveTextColor
    };

    enum StyleKey
    {
        ApplicationStyle,
        MainMenuStyleSheet,
        QueryEditStyleSheet
    };

public:
    Design();
    double size(SizeKey key) const;
    QColor color(ColorRey key) const;
    QString styleSheet(StyleKey key) const;
};

#endif // DESIGN_H
