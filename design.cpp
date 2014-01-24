#include "design.h"
#define _USE_MATH_DEFINES
#include <float.h>
#include <math.h>

//******************************************************************************************************
/*!
 *\class Design
 *\brief Класс, представляющий информацию о дизайне (размеры, цвета и пр.).
*/
//******************************************************************************************************

Design::Design()
    :SingletonT<Design>()
{
}

double Design::size(SizeKey key) const
{
    double result = 0;
    switch (key)
    {
    case DocumentLayerGridSize:
        result = 16;
        break;
    case DocumentBoxBorderSize:
        result = 8;
        break;
    case DocumentBoxCornerSize:
        result = 12;
        break;
    case DocumentBoxTitleSize:
        result = 32;
        break;
    case DocumentBoxSpacing:
        result = 8;
        break;
    case DocumentBoxMinimumGridWidth:
        result = 12;
        break;
    case DocumentBoxMinimumGridHeight:
        result = 8;
        break;
    case DocumentBoxButtonSpacing:
        result = 4;
        break;
    case HeadBarMinimalWidth:
        result = 480;
        break;
    case HeadBarMinimalHeight:
        result = 24;
        break;
    case HeadBarLeftMargin:
        result = 8;
        break;
    case GraphicButtonDefaultIconSize:
        result = 16;
        break;
    case GraphicButtonIconTextSpacing:
        result = 4;
        break;
    case TabMinWidth:
        result = 148;
        break;
    case TabMaxWidth:
        result = 240;
        break;
    case TabEmbryoWidth:
        result = 30;
        break;
    case TabEmbryoHeight:
        result = 16;
        break;
    case TabEmbryoSpacing:
        result = 6;
        break;
    case TabElementSpacing:
        result = 4;
        break;
    case TabSwitcherMinWidth:
        result = 400;
        break;
    case TabSwitcherMinHeight:
        result = 24;
        break;
    case TabRadius:
        result = 7;
        break;
    }
    return result;
}

QColor Design::color(ColorRey key) const
{
    QColor result;
    switch (key)
    {
    case DocumentLayerBgColor:
        result = QColor("#0c1b1d");
        break;
    case DocumentBoxBgColor:
        result = QColor("#505050");
        break;
    case DocumentBoxOutFocusLineColor:
        result = QColor("#202020");
        break;
    case DocumentBoxInFocusLineColor:
        result = QColor("#008080");
        break;
    case HeadBarBgColor:
        result = QColor("#505050");
        break;
    case GraphicButtonTextColor:
        result = QColor("#e0e0e0");
        break;
    case TabLineColor:
        result = QColor("#202020");
        break;
    case TabNormalBgColor:
        result = QColor("#808080");
        break;
    case TabNormalTextColor:
        result = QColor(Qt::black);
        break;
    case TabHoverBgColor:
        result = QColor("#989898");
        break;
    case TabHoverTextColor:
        result = QColor(Qt::black);
        break;
    case TabActiveBgColor:
        result = QColor("#008080");
        break;
    case TabActiveTextColor:
        result = QColor(Qt::white);
        break;
    }
    return result;
}

QString Design::styleSheet(StyleKey key) const
{
    QString result;
    switch (key)
    {
    case ApplicationStyle:
        result = "* {font-size:10pt; font-family: \"Tahoma\"}";
        break;
    case MainMenuStyleSheet:
        result =
                "QMenuBar {background-color:#505050; color:#e0e0e0} \n"
                "QMenuBar::item:selected {background-color:#008080; color:#ffffff} \n"
                ;
        break;
    case QueryEditStyleSheet:
        result =
                "QLineEdit {background-color:#808080; border: 1px solid #404040}";
        break;
    }

    return result;
}
