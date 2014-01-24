#ifndef INDEXSORTHEPLERT_H
#define INDEXSORTHEPLERT_H

#include <QVector>
#include <QList>

template <typename T>
class IndexSortHelperT
{
public:
    enum CollectionType
    {
        CollectionTypeList,
        CollectionTypeVector
    };

public:
    IndexSortHelperT()
        :m_baseList()
        ,m_baseVector()
        ,m_collectionType(CollectionTypeList)
        ,m_desc(false)
    {

    }

    void setBaseList(const QList<T> &list)
    {
        m_baseList = list;
        m_collectionType = CollectionTypeList;
        m_indexes.resize(m_baseList.count());
        for (int i = 0; i < m_baseList.count(); i++)
        {
            m_indexes[i] = i;
        }
    }

    const QList<T>& baseList() const
    {
        return m_baseList;
    }

    void setBaseVector(const QVector<T> &vector)
    {
        m_baseVector = vector;
        m_collectionType = CollectionTypeVector;
        m_indexes.resize(m_baseVector.count());
        for (int i = 0; i < m_baseVector.count(); i++)
        {
            m_indexes[i] = i;
        }
    }

    const QVector<T>& baseVector() const
    {
        return m_baseVector;
    }

    CollectionType collectionType() const
    {
        return m_collectionType;
    }

    void setDesc(bool value)
    {
        m_desc = value;
    }

    bool desc() const
    {
        return m_desc;
    }

    virtual bool isO1LessThanO2(const T &o1, const T &o2) const = 0;

    virtual bool compare(const T &o1, const T &o2) const
    {
        return isO1LessThanO2(o1, o2) && (!m_desc);
    }

    bool operator()(int index1, int index2)
    {
        return compare(
                    (m_collectionType == CollectionTypeList)? m_baseList[index1] : m_baseVector[index1],
                    (m_collectionType == CollectionTypeList)? m_baseList[index2] : m_baseVector[index2]
                    );
    }

    const QVector<int>& indexes() const
    {
        return m_indexes;
    }

    QVector<int>& indexes()
    {
        return m_indexes;
    }

private:
    QList<T> m_baseList;
    QVector<T> m_baseVector;
    CollectionType m_collectionType;
    bool m_desc;
    QVector<int> m_indexes;
};


#endif // INDEXSORTHEPLERT_H
