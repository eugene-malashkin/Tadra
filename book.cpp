#include "book.h"
#include "window.h"
#include <QSet>

//******************************************************************************************************
/*!
 *\class Book
 *\brief Книга, состоящая из листов (вкладок) с документами.
*/
//******************************************************************************************************

Book::Book(Window *window)
    : QWidget()
    , m_window(window)
    , m_sheets()
    , m_layout(NULL)
{
    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

Window* Book::window() const
{
    return m_window;
}

void Book::addSheet(const QString &label)
{
    // Формируем параметры вкладки
    TabInfo tabInfo(QUuid::createUuid(),
                    (!label.isEmpty()) ? label : newTabLabel());

    // Добавляем лист
    Sheet *sheet = new Sheet(this);
    sheet->setTabInfo(tabInfo);
    m_sheets << sheet;
    m_layout->addWidget(sheet);

    // Устанавливаем текущий лист
    int indexToBeCurrent = m_sheets.count()-1;
    bool wasCurrentIndexChanged = false;
    innerSetCurrentIndex(indexToBeCurrent, wasCurrentIndexChanged);

    // Сигнализируем об изменениях
    emit changed();
}

void Book::removeSheet(int index, bool doDelete)
{
    if ((index >= 0) && (index < m_sheets.count()))
    {
        // Удаляем лист
        Sheet *sheet = m_sheets[index];
        m_sheets.removeAt(index);
        if (doDelete)
        {
            sheet->deleteLater();
        }

        // Устанавливаем текущий лист
        int indexToBeCurrent = index;
        if (indexToBeCurrent > m_sheets.count()-1)
        {
            indexToBeCurrent = m_sheets.count()-1;
        }
        bool wasCurrentIndexChanged = false;
        innerSetCurrentIndex(indexToBeCurrent, wasCurrentIndexChanged);

        // Сигнаилизируем об изменениях
        emit changed();
    }
}

void Book::setSheetLabel(int index, const QString &label)
{
    if ((index >= 0) && (index < m_sheets.count()))
    {
        Sheet *sheet = m_sheets[index];
        TabInfo tabInfo = sheet->tabInfo();
        tabInfo.label = label;
        if (sheet->tabInfo() != tabInfo)
        {
            sheet->setTabInfo(tabInfo);
            emit changed();
        }
    }
}

QString Book::sheetLabel(int index) const
{
    QString result;
    if ((index >= 0) && (index < m_sheets.count()))
    {
        result = m_sheets[index]->tabInfo().label;
    }
    return result;
}

int Book::count() const
{
    return m_sheets.count();
}

bool Book::isEmpty() const
{
    return m_sheets.isEmpty();
}

SheetList Book::sheets() const
{
    return m_sheets;
}

void Book::setCurrentIndex(int value)
{
    bool wasChanged = false;
    innerSetCurrentIndex(value, wasChanged);
    if (wasChanged)
    {
        emit changed();
    }
}

int Book::currentIndex() const
{
    int result = -1;
    for (int i = 0; i < m_sheets.count(); i++)
    {
        if (m_sheets[i]->isActive())
        {
            result = i;
            break;
        }
    }
    return result;
}

Sheet* Book::currentSheet() const
{
    Sheet *result = NULL;
    int index = currentIndex();
    if (index >= 0)
    {
        result = m_sheets[index];
    }
    return result;
}

void Book::nextSheet()
{
    int ward = 0;
    int current = currentIndex();
    if ((current >= 0) && (current < count()-1))
    {
        ward = current + 1;
    }
    setCurrentIndex(ward);
}

void Book::previousSheet()
{
    int ward = 0;
    int current = currentIndex();
    if (current >= 0)
    {
        if (current == 0)
        {
            ward = count()-1;
        }
        else
        {
            ward = current - 1;
        }
    }
    setCurrentIndex(ward);
}

int Book::indexOf(const QUuid &tabUid) const
{
    int result = -1;
    for (int i = 0; i < m_sheets.count(); i++)
    {
        if (m_sheets[i]->tabInfo().uid == tabUid)
        {
            result = i;
            break;
        }
    }
    return result;
}

bool Book::contains(const QUuid &tabUid) const
{
    return (indexOf(tabUid) >= 0);
}

Sheet* Book::sheet(int index) const
{
    Sheet *result = NULL;
    if ((index >= 0) && (index < m_sheets.count()))
    {
        result = m_sheets[index];
    }
    return result;
}

Sheet* Book::sheet(const QUuid &tabUid) const
{
    return sheet(indexOf(tabUid));
}

TabData Book::tabData() const
{
    TabData result;
    for (int i = 0; i < m_sheets.count(); i++)
    {
        result.items << m_sheets[i]->tabInfo();
        if (m_sheets[i]->isActive())
        {
            result.currentIndex = i;
        }
    }
    return result;
}

bool Book::moveSheet(Book *fromBook, const QUuid &tabUid, int toIndex)
{
    bool result = false;
    int oldIndex = fromBook->indexOf(tabUid);
    if (oldIndex >= 0)
    {
        // Есть такой лист
        if (fromBook == this)
        {
            // Перемещение происходит внутри этой книги
            int correctedNewIndex = qBound(0, toIndex, m_sheets.count()-1);
            if (oldIndex != correctedNewIndex)
            {
                Sheet *sheet = m_sheets[oldIndex];
                if (oldIndex < correctedNewIndex)
                {
                    for (int i = oldIndex; i < correctedNewIndex; i++)
                    {
                        m_sheets[i] = m_sheets[i+1];
                    }
                }
                else if (oldIndex > correctedNewIndex)
                {
                    for (int i = oldIndex; i > correctedNewIndex; i--)
                    {
                        m_sheets[i] = m_sheets[i-1];
                    }
                }
                m_sheets[correctedNewIndex] = sheet;

                // Устанавливаем текущий лист
                bool wasCurrentIndexChanged = false;
                innerSetCurrentIndex(correctedNewIndex, wasCurrentIndexChanged);

                // Сигнаилизируем об изменениях
                emit changed();
            }
        }
        else
        {
            // Удаляем из той книги...
            Sheet *sheetToMove = fromBook->sheet(oldIndex);
            fromBook->removeSheet(oldIndex, false);

            // ...и вставляем в эту
            int correctedNewIndex = qBound(0, toIndex, m_sheets.count());
            m_sheets.insert(correctedNewIndex, sheetToMove);
            m_layout->addWidget(sheetToMove);

            // Устанавливаем текущий лист
            bool wasCurrentIndexChanged = false;
            innerSetCurrentIndex(correctedNewIndex, wasCurrentIndexChanged);

            // Сигнаилизируем об изменениях
            emit changed();

        }
    }
    return result;
}

void Book::innerSetCurrentIndex(int index, bool &wasChanged)
{
    wasChanged = false;

    // Гасим ненужные листы
    for (int i = 0; i < m_sheets.count(); i++)
    {
        if (i != index)
        {
            if (m_sheets[i]->isActive())
            {
                wasChanged = true;
            }
            m_sheets[i]->setActive(false);
        }
    }

    // Показываем нужных лист
    for (int i = 0; i < m_sheets.count(); i++)
    {
        if (i == index)
        {
            if (!m_sheets[i]->isActive())
            {
                wasChanged = true;
            }
            m_sheets[i]->setActive(true);
            break;
        }
    }
}

QString Book::newTabLabel() const
{
    QSet<QString> existingLabels;
    foreach (Sheet *sheet, m_sheets)
    {
        existingLabels << sheet->tabInfo().label;
    }
    QString result = ("Новая вкладка");
    if (existingLabels.contains(result))
    {
        for (int i = 1; i <= 99; i++)
        {
            QString proposingLabel = QString("Новая вкладка (%1)").arg(i);
            if (!existingLabels.contains(proposingLabel))
            {
                result = proposingLabel;
                break;
            }
        }
    }
    return result;
}
