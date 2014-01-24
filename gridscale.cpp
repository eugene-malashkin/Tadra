#include "gridscale.h"
#include <float.h>
#include <math.h>
#include "floatroutine.h"

#include <QDebug>

//******************************************************************************************************
/*!
 *\class GridScale
 *\brief Шкала ячеек Document.
*/
//******************************************************************************************************

GridScale::GridScale()
    :m_gridSize(32)
    ,m_screenSize(128)
    ,m_gridCount(4)
    ,m_redemption(0)
{
}

void GridScale::setGridSize(int value)
{
    int v = (value > 0)? value : 1;
    if (m_gridSize != v)
    {
        m_gridSize = v;
        compute();
    }
}

int GridScale::gridSize() const
{
    return m_gridSize;
}

void GridScale::setScreenSize(double value)
{
    double v = (value > 0)? value : 1;
    if (m_screenSize != v)
    {
        m_screenSize = v;
        compute();
    }
}

double GridScale::screenSize() const
{
    return m_screenSize;
}

int GridScale::gridCount() const
{
    return m_gridCount;
}

double GridScale::gridToScreen(int grid) const
{
    if (grid == 0)
    {
        return 0;
    }
    else if (grid == m_gridCount)
    {
        return m_screenSize;
    }
    else
    {
        return m_redemption + grid*m_gridSize;
    }
}

int GridScale::screenToGrid(double screen) const
{
    return iround( (screen - m_redemption)/double(m_gridSize) );
}

double GridScale::roundScreen(double screen) const
{
    int grid = screenToGrid(screen);
    return gridToScreen(grid);
}

void GridScale::compute()
{
    int isize = int(m_screenSize);
    m_gridCount =  isize / m_gridSize;
    m_redemption = isize % m_gridSize / 2;
}
