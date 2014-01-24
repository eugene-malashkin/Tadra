#ifndef GRIDSCALE_H
#define GRIDSCALE_H

class GridScale
{
public:
    GridScale();
    void setGridSize(int value);
    int gridSize() const;
    void setScreenSize(double value);
    double screenSize() const;
    int gridCount() const;
    double gridToScreen(int grid) const;
    int screenToGrid(double screen) const;
    double roundScreen(double screen) const;

private:
    int m_gridSize;
    double m_screenSize;
    int m_gridCount;
    double m_redemption;
    void compute();
};

#endif // GRIDSCALE_H
