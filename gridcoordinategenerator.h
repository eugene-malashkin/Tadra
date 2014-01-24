#ifndef GRIDCOORDINATEGENERATOR_H
#define GRIDCOORDINATEGENERATOR_H

#include "base.h"
#include <QStringList>

struct StackSegment
{
    double min;
    double max;
    StackSegment();
    StackSegment(double aMin, double aMax);
    bool isValid() const;
    double length() const;
    bool operator==(const StackSegment &other) const;
    bool operator!=(const StackSegment &other) const;
};

class StackSegmentList : public QList<StackSegment>
{
public:
    bool isValid() const;
};

struct GridSegment
{
    int min;
    int max;
    GridSegment();
    GridSegment(int aMin, int aMax);
    bool isValid() const;
    int length() const;
    bool operator==(const GridSegment &other) const;
    bool operator!=(const GridSegment &other) const;
};
typedef QList<GridSegment> GridSegmentList;

class GridCoordinateGenerator
{
public:
    GridCoordinateGenerator();
    void setMinimalSegmentGridLength(int value);
    int minimalSegmentGridLength() const;
    void setStackSegmentList(const StackSegmentList &value);
    StackSegmentList stackSegmentList() const;
    int minimalGridSpaceLength() const;
    void setSupposedGridSpace(const GridSegment &value);
    GridSegment supposedGridSpace() const;
    GridSegment gridSpace() const;
    GridSegmentList gridSegmentList() const;

public:
    QStringList debugInformation() const;

private:
    int m_minimalSegmentGridLength;
    StackSegmentList m_stackSegmentList;
    GridSegment m_supposedGridSpace;
    GridSegment m_gridSpace;
    GridSegmentList m_gridSegmentList;
    IntVector m_vertexIndexes;
    DoubleVector m_stackCoordinates;
    IntVectorList m_adjacencyMatrix;
    IntVectorList m_paths;
    int m_maxPathLength;
    IntVector m_longestPath;
    IntVector m_gridCoordinates;

    void buildVertexes();
    void buildAdjacencyMatrix();
    void computeGraph();
    IntVectorList pathsFrom(int vertex) const;
    int pathLength(const IntVector &path) const;
    void computePaths();
    void computeLongestPath();
    void computeGridSpace();
    IntVector gridCoordinatesForPath(const GridSegment &gridSpace, const IntVector &path) const;
    void setGridCoordinatesForPath(const GridSegment &gridSpace, const IntVector &path);
    IntVector findBearingPath1(int vertex, const IntVector &path) const;
    IntVector findBearingPath1(int vertex) const;

    void computeGridCoordinates();
    void computeGridSegmentList();
    void computeAll();

    static void reduceStackCoordinates(const DoubleVector &rawSortedStackCoordinates, const IntVector &rawSortedIndexes, DoubleVector &reducedStackCoordinates, IntVector &reducedIndexes);
};

#endif // GRIDCOORDINATEGENERATOR_H
