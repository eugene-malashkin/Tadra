#include "gridcoordinategenerator.h"
#include <float.h>
#include <math.h>
#include "floatroutine.h"
#include "indexsortheplert.h"

#include <QDebug>

//******************************************************************************************************
/*!
 *\struct StackSegment
*/
//******************************************************************************************************

StackSegment::StackSegment()
    :min(getNaN())
    ,max(getNaN())
{

}

StackSegment::StackSegment(double aMin, double aMax)
    :min(aMin)
    ,max(aMax)
{

}

bool StackSegment::isValid() const
{
    return (!isNaN(min)) && (!isNaN(max)) && (doubleLess(min, max));
}

double StackSegment::length() const
{
    return (max - min);
}

bool StackSegment::operator==(const StackSegment &other) const
{
    return (min == other.min) && (max == other.max);
}

bool StackSegment::operator!=(const StackSegment &other) const
{
    return !(operator ==(other));
}


//******************************************************************************************************
/*!
 *\class StackSegmentList
*/
//******************************************************************************************************

bool StackSegmentList::isValid() const
{
    bool result = true;
    for (auto iter = constBegin(); iter != constEnd(); ++iter)
    {
        if (!iter->isValid())
        {
            result = false;
            break;
        }
    }
    return result;
}


//******************************************************************************************************
/*!
 *\struct GridSegment
*/
//******************************************************************************************************

GridSegment::GridSegment()
    :min(0)
    ,max(0)
{

}

GridSegment::GridSegment(int aMin, int aMax)
    :min(aMin)
    ,max(aMax)
{

}

bool GridSegment::isValid() const
{
    return (min >= 0) && (max > min);
}

int GridSegment::length() const
{
    return (max - min);
}

bool GridSegment::operator==(const GridSegment &other) const
{
    return (min == other.min) && (max == other.max);
}

bool GridSegment::operator!=(const GridSegment &other) const
{
    return !(operator ==(other));
}


//******************************************************************************************************
/*!
 *\class GridCoordinateGenerator
*/
//******************************************************************************************************

GridCoordinateGenerator::GridCoordinateGenerator()
    :m_minimalSegmentGridLength(1)
    ,m_stackSegmentList()
    ,m_supposedGridSpace()
    ,m_gridSpace()
    ,m_gridSegmentList()
    ,m_vertexIndexes()
    ,m_stackCoordinates()
    ,m_adjacencyMatrix()
    ,m_paths()
    ,m_maxPathLength(0)
    ,m_longestPath()
    ,m_gridCoordinates()
{
}

void GridCoordinateGenerator::setMinimalSegmentGridLength(int value)
{
    int correctedValue = qMax(value, 1);
    if (m_minimalSegmentGridLength != correctedValue)
    {
        m_minimalSegmentGridLength = correctedValue;
        computeAll();
    }
}

int GridCoordinateGenerator::minimalSegmentGridLength() const
{
    return m_minimalSegmentGridLength;
}

void GridCoordinateGenerator::setStackSegmentList(const StackSegmentList &value)
{
    if (m_stackSegmentList != value)
    {
        m_stackSegmentList = value;
        computeAll();
    }
}

StackSegmentList GridCoordinateGenerator::stackSegmentList() const
{
    return m_stackSegmentList;
}

int GridCoordinateGenerator::minimalGridSpaceLength() const
{
    return m_maxPathLength * m_minimalSegmentGridLength;
}

void GridCoordinateGenerator::setSupposedGridSpace(const GridSegment &value)
{
    if (m_supposedGridSpace != value)
    {
        m_supposedGridSpace = value;
        computeGridSpace();
        computeGridCoordinates();
        computeGridSegmentList();
    }
}

GridSegment GridCoordinateGenerator::supposedGridSpace() const
{
    return m_supposedGridSpace;
}

GridSegment GridCoordinateGenerator::gridSpace() const
{
    return m_gridSpace;
}

GridSegmentList GridCoordinateGenerator::gridSegmentList() const
{
    return m_gridSegmentList;
}

QStringList GridCoordinateGenerator::debugInformation() const
{
    QStringList result;

    QStringList segmentCoordinatesSt;
    for (int i = 0; i < m_stackSegmentList.count(); i++)
    {
        int vertex1 = m_vertexIndexes[i*2];
        int vertex2 = m_vertexIndexes[i*2 + 1];
        segmentCoordinatesSt << QString("<%1, %2>").arg(m_stackCoordinates[vertex1], 0, 'f', 3).arg(m_stackCoordinates[vertex2], 0, 'f', 3);
    }
    result << QString("Segments stack coordinates: (%1)").arg(segmentCoordinatesSt.join(", "));

    QStringList stackCoordinatesSt;
    foreach (double sc, m_stackCoordinates)
    {
        stackCoordinatesSt << QString::number(sc, 'f', 3);
    }
    result << QString("Vertexes stack coordinates: (%1)").arg(stackCoordinatesSt.join(", "));

    QStringList adjacenciesSt;
    for (int i = 0; i < m_stackCoordinates.count(); i++)
    {
        QStringList toSt;
        for (int j = i + 1; j < m_stackCoordinates.count(); j++)
        {
            if (m_adjacencyMatrix[i][j] >= 1)
            {
                toSt << QString::number(j);
            }
        }
        if (toSt.isEmpty())
        {
            adjacenciesSt << QString("%1 <nothing>").arg(i);
        }
        else
        {
            adjacenciesSt << QString("%1 <%2>").arg(i).arg(toSt.join(" "));
        }
    }
    result << QString("Adjacencies: (%1)").arg(adjacenciesSt.join(", "));

    QStringList pathSt;
    foreach (const IntVector &path, m_paths)
    {
        QStringList vs;
        foreach (int index, path)
        {
            vs << QString::number(index);
        }
        pathSt << QString("<%1>").arg(vs.join(" "));
    }
    result << "*** paths";
    result << pathSt;
    result << "*** end paths";

    result << QString("Max path length: %1").arg(m_maxPathLength);
    result << QString("Grid space: [%1, %2]").arg(m_gridSpace.min).arg(m_gridSpace.max);

    QStringList gridCoordinateSt;
    foreach (int c, m_gridCoordinates)
    {
        gridCoordinateSt << QString::number(c);
    }
    result << QString("Grid coordinates: (%1)").arg(gridCoordinateSt.join(", "));

    return result;
}

class CoordinateIndexSortHelper : public IndexSortHelperT<double>
{
public:
    bool isO1LessThanO2(const double &o1, const double &o2) const override
    {
        return o1 < o2;
    }
};

void GridCoordinateGenerator::buildVertexes()
{
    DoubleVector stackCoordinates;
    for (int i = 0; i < m_stackSegmentList.count(); i++)
    {
        stackCoordinates << m_stackSegmentList[i].min;
        stackCoordinates << m_stackSegmentList[i].max;
    }

    CoordinateIndexSortHelper sorter;
    sorter.setBaseVector(stackCoordinates);
    qSort(sorter.indexes().begin(), sorter.indexes().end(), sorter);

    m_stackCoordinates.clear();
    m_vertexIndexes.clear();
    reduceStackCoordinates(stackCoordinates, sorter.indexes(), m_stackCoordinates, m_vertexIndexes);
}

void GridCoordinateGenerator::buildAdjacencyMatrix()
{
    // Начальное заполнение
    m_adjacencyMatrix.clear();
    for (int i = 0; i < m_stackCoordinates.count(); i++)
    {
        IntVector row;
        row.fill(-1, m_stackCoordinates.count());    // -1 нет связи между вершинами
        if (i > 0)
        {
            row[i-1] = 0;   // 0 вершины связаны между собой, но не ограничены по дистанции
        }
        if (i < m_stackCoordinates.count()-1)
        {
            row[i+1] = 0;
        }
        m_adjacencyMatrix << row;
    }

    // Заполнение ограничениями по дистанции
    for (int i = 0; i < m_stackSegmentList.count(); i++)
    {
        int vertex1 = m_vertexIndexes[i*2];
        int vertex2 = m_vertexIndexes[i*2 + 1];
        m_adjacencyMatrix[vertex1][vertex2] = 1;    // 1 вершины имеют ограничение по дистанции
        m_adjacencyMatrix[vertex2][vertex1] = 1;
    }
}

void GridCoordinateGenerator::computeGraph()
{
    buildVertexes();
    buildAdjacencyMatrix();
}

IntVectorList GridCoordinateGenerator::pathsFrom(int vertex) const
{
    IntVectorList result;

    // Путь, состоящий из текущей вершины
    IntVector currentVertex;
    currentVertex << vertex;

    // Ищем низлежащие пути
    for (int i = vertex + 1; i < m_stackCoordinates.count(); i++)
    {
        if (m_adjacencyMatrix[vertex][i] >= 0)
        {
            IntVectorList flowingSubpaths = pathsFrom(i);
            foreach (const IntVector &flowingSubpath, flowingSubpaths)
            {
                IntVector path;
                path << currentVertex;
                path << flowingSubpath;
                result << path;
            }
        }
    }

    if (result.isEmpty())
    {
        // Низлежащих путей нет
        result << currentVertex;
    }
    return result;
}

int GridCoordinateGenerator::pathLength(const IntVector &path) const
{
    int sum = 0;
    for (int i = 0; i < path.count()-1; i++)
    {
        int vertex1 = path[i];
        int vertex2 = path[i+1];
        int w = m_adjacencyMatrix[vertex1][vertex2];
        if (w < 0)
        {
            // Некорректный путь
            return -1;
        }
        else
        {
            sum += w;
        }
    }
    return sum;
}

void GridCoordinateGenerator::computePaths()
{
    m_paths.clear();
    if (m_stackCoordinates.count() >= 2)
    {
        m_paths = pathsFrom(0);
    }
}

void GridCoordinateGenerator::computeLongestPath()
{
    m_longestPath = IntVector();
    m_maxPathLength = 0;
    foreach (const IntVector &path, m_paths)
    {
        int length = pathLength(path);
        if (m_maxPathLength < length)
        {
            m_maxPathLength = length;
            m_longestPath = path;
        }
    }
}

void GridCoordinateGenerator::computeGridSpace()
{
    // Вычисляем и устанавливаем используемый для фактического расчёта координат grid space
    GridSegment gridSpace = m_supposedGridSpace;
    int minLen = minimalGridSpaceLength();
    if (!gridSpace.isValid())
    {
        gridSpace.min = 0;
        gridSpace.max = minLen;
    }
    else if (gridSpace.length() < minLen)
    {
        gridSpace.max = gridSpace.min + minLen;
    }
    if (m_gridSpace != gridSpace)
    {
        m_gridSpace = gridSpace;
    }
}

IntVector GridCoordinateGenerator::gridCoordinatesForPath(const GridSegment &gridSpace, const IntVector &path) const
{
    IntVector result;

    // Начальная инициализация результата
    result.fill(-1, path.count());
    result[0] = gridSpace.min;

    // Заполняем результат минимальными grid-координатами (соответственно, только для промежуточных вершин)
    int currentGridCoordinate = gridSpace.min;
    for (int i = 1; i < path.count(); i++)
    {
        int edgeLength = 0;
        int vertex1 = path[i-1];
        int vertex2 = path[i];
        if (m_adjacencyMatrix[vertex1][vertex2] >= 1)
        {
            edgeLength = m_minimalSegmentGridLength;
        }
        currentGridCoordinate += edgeLength;
        result[i] = currentGridCoordinate;
    }

    // Заполняем свободное пространство в соответствии со stack-координатами
    double stackMin = m_stackCoordinates[path.first()];
    double stackMax = m_stackCoordinates[path.last()];
    int minGridSpace = pathLength(path)*m_minimalSegmentGridLength;
    int remainGridSpace = gridSpace.length() - minGridSpace;
    double remainStackSpace = (stackMax - stackMin)*(1 - double(minGridSpace)/double(gridSpace.length()));
    if ((remainGridSpace) && (doubleGreater(remainStackSpace, 0)))
    {
        double segmentMinStackLength = double(m_minimalSegmentGridLength)/double(gridSpace.length())*(stackMax - stackMin);
        double remainK = double(remainGridSpace) / remainStackSpace;
        for (int i = 1; i < path.count(); i++)
        {
            int vertex1 = path[i-1];
            int vertex2 = path[i];
            double edgeRemainStackLength = m_stackCoordinates[vertex2] - m_stackCoordinates[vertex1];
            if (m_adjacencyMatrix[vertex1][vertex2] >= 1)
            {
                edgeRemainStackLength -= segmentMinStackLength;
            }
            if (doubleGreater(edgeRemainStackLength, 0))
            {
                int edgeRemainGridLength = iround(edgeRemainStackLength * remainK);
                edgeRemainGridLength = qMin(edgeRemainGridLength, gridSpace.max - result.last());
                for (int j = i; j < path.count(); j++)
                {
                    result[j] += edgeRemainGridLength;
                }
            }
        }
    }

    // Окончание вычисления
    result[path.count()-1] = gridSpace.max;

    return result;
}

void GridCoordinateGenerator::setGridCoordinatesForPath(const GridSegment &gridSpace, const IntVector &path)
{
    IntVector gpc = gridCoordinatesForPath(gridSpace, path);
    for (int i = 0; i < path.count(); i++)
    {
        int vertex = path[i];
        m_gridCoordinates[vertex] = gpc[i];
    }
}

IntVector GridCoordinateGenerator::findBearingPath1(int vertex, const IntVector &path) const
{
    IntVector result;
    int vertexIndex = path.indexOf(vertex);
    if (vertexIndex >= 0)
    {
        int startFrom = vertexIndex;
        for (int i = vertexIndex-1; i >= 0; i--)
        {
            startFrom = i;
            if (m_gridCoordinates[path[i]] >= 0)
            {
                break;
            }
        }
        int endTo = vertexIndex;
        for (int i = vertexIndex+1; i < path.count(); i++)
        {
            endTo = i;
            if (m_gridCoordinates[path[i]] >= 0)
            {
                break;
            }
        }
        result = path.mid(startFrom, endTo - startFrom + 1);
    }
    return result;
}

IntVector GridCoordinateGenerator::findBearingPath1(int vertex) const
{
    IntVector result;
    int resultLength = 0;
    foreach (const IntVector &path, m_paths)
    {
        IntVector currentPath = findBearingPath1(vertex, path);
        int currentLength = pathLength(currentPath);
        if (resultLength < currentLength)
        {
            resultLength = currentLength;
            result = currentPath;
        }
    }
    return result;
}

void GridCoordinateGenerator::computeGridCoordinates()
{
    m_gridCoordinates.fill(-1, m_stackCoordinates.count());
    if ((!m_paths.isEmpty()) && (m_longestPath.count() >= 2))
    {
        setGridCoordinatesForPath(m_gridSpace, m_longestPath);
        for (int vertex = 0; vertex < m_gridCoordinates.count(); vertex++)
        {
            if (m_gridCoordinates[vertex] == -1)
            {
                IntVector bearingPath = findBearingPath1(vertex);
                if (bearingPath.count() >= 3)
                {
                    GridSegment subGridSpace(m_gridCoordinates[bearingPath.first()], m_gridCoordinates[bearingPath.last()]);
                    setGridCoordinatesForPath(subGridSpace, bearingPath);
                }
            }
        }
    }
}

void GridCoordinateGenerator::computeGridSegmentList()
{
    m_gridSegmentList.clear();
    for (int i = 0; i < m_stackSegmentList.count(); i++)
    {
        int vertex1 = m_vertexIndexes[i*2];
        int vertex2 = m_vertexIndexes[i*2 + 1];
        int grid1 = m_gridCoordinates[vertex1];
        int grid2 = m_gridCoordinates[vertex2];
        m_gridSegmentList << GridSegment(grid1, grid2);
    }
}

void GridCoordinateGenerator::computeAll()
{
    computeGraph();
    computePaths();
    computeLongestPath();
    computeGridSpace();
    computeGridCoordinates();
    computeGridSegmentList();
}

void GridCoordinateGenerator::reduceStackCoordinates(const DoubleVector &rawSortedStackCoordinates, const IntVector &rawSortedIndexes, DoubleVector &reducedStackCoordinates, IntVector &reducedIndexes)
{
    static const double samplingCoef = 10000;

    if ((!rawSortedStackCoordinates.isEmpty()) && (rawSortedStackCoordinates.count() == rawSortedIndexes.count()))
    {
        double min = rawSortedStackCoordinates[rawSortedIndexes.first()];
        double max = rawSortedStackCoordinates[rawSortedIndexes.last()];
        double eps = (max - min) / samplingCoef;

        reducedIndexes.resize(rawSortedIndexes.count());

        // Добавляем первую координату
        int index = rawSortedIndexes[0];
        double lastAddedCoordinate = rawSortedStackCoordinates[index];
        reducedIndexes[index] = 0;
        reducedStackCoordinates << lastAddedCoordinate;

        // Добавляем следующие координаты, если они отличаются более чем на eps
        for (int i = 1; i < rawSortedStackCoordinates.count(); i++)
        {
            int index = rawSortedIndexes[i];
            double coordinate = rawSortedStackCoordinates[index];
            if (fabs(coordinate - lastAddedCoordinate) > eps)
            {
                reducedStackCoordinates << coordinate;
                lastAddedCoordinate = coordinate;
            }
            reducedIndexes[index] = reducedStackCoordinates.count() - 1;
        }
    }
}
