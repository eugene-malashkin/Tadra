#include "searchengine.h"
#include <QDomDocument>
#include <QVector>

#include <QDebug>

//******************************************************************************************************
/*!
 *\class SearchEngine
 *\brief Поисковик инструментов.
*/
//******************************************************************************************************

SearchEngine::SearchEngine()
    : QObject()
    , SingletonT<SearchEngine>()
    , m_instruments()
{
}

void SearchEngine::loadInstruments()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl("http://www.cbr.ru/scripts/XML_val.asp")));
}

CurrencyInstrumentRankedMap SearchEngine::variants(const QString &query) const
{
    CurrencyInstrumentRankedMap result;
    QStringList querySentence = query.toUpper().split(" ", QString::SkipEmptyParts);
    if (!querySentence.isEmpty())
    {
        int relevance = maximalRelevantDistance(querySentence);
        foreach (const CurrencyInstrument &instrument, m_instruments)
        {
            QStringList baseSentence = instrument.name.toUpper().split(" ", QString::SkipEmptyParts);
            double d = sentenceDistance(querySentence, baseSentence);
            if (d <= relevance)
            {
                result.insert(d, instrument);
            }
        }

    }
    return result;
}

void SearchEngine::onReplyFinished(QNetworkReply *reply)
{
    m_instruments.clear();

    QDomDocument doc;
    if (doc.setContent(reply->readAll()))
    {
        QDomElement root = doc.documentElement();
        for (QDomElement itemElement = root.firstChildElement("Item"); !itemElement.isNull(); itemElement = itemElement.nextSiblingElement("Item"))
        {
            QString id = itemElement.attribute("ID");
            QString name = itemElement.firstChildElement("Name").text();
            if ((!id.isEmpty()) && (!name.isEmpty()))
            {
                m_instruments << CurrencyInstrument(id, name);
            }
        }
    }
}

int SearchEngine::wordDistance(const QString &queryWord, const QString &baseWord)
{
    if ((queryWord.isEmpty()) || (baseWord.isEmpty()))
    {
        return 0;
    }

    typedef QVector<int> IntVector;
    QList<IntVector> matrix;
    for (int i = 0; i < queryWord.length()+1; i++)
    {
        IntVector row;
        row.fill(0, baseWord.length()+1);
        matrix << row;
    }
    for (int i = 0; i < queryWord.length()+1; i++)
    {
        matrix[i][0] = i;
    }
    for (int j = 0; j < baseWord.length()+1; j++)
    {
        matrix[0][j] = j;
    }
    for (int i = 1; i < queryWord.length()+1; i++)
    {
        for (int j = 1; j < baseWord.length()+1; j++)
        {
            int match = (queryWord[i-1] == baseWord[j-1]) ? 0 : 1;
            matrix[i][j] = qMin(qMin(matrix[i-1][j] + 1, matrix[i][j-1] + 1), matrix[i-1][j-1] + match);
        }
    }

    // В отличие от расстояния Левенштейна, берётся не последнее значение последней строки, а минимальное значение
    // последней строки.
    IntVector lastRow = matrix.last();
    int result = lastRow[0];
    for (int j = 1; j < lastRow.count(); j++)
    {
        result = qMin(result, lastRow[j]);
    }
    return result;
}

int SearchEngine::wordInSentenceDistance(const QStringList &querySentence, const QStringList &baseSentence, int queryWordIndex, int baseWordIndex)
{
    int result = wordDistance(querySentence[queryWordIndex], baseSentence[baseWordIndex]);
    if (queryWordIndex != baseWordIndex)
    {
        // Несовпадение позиций слов карается штрафом +1
        result++;
    }
    return result;
}

int SearchEngine::sentenceDistance(const QStringList &querySentence, const QStringList &baseSentence)
{
    if ((querySentence.isEmpty()) || (baseSentence.isEmpty()))
    {
        return 0;
    }
    int result = 0;
    for (int i = 0; i < querySentence.length(); i++)
    {
        int bestWordDistance = wordInSentenceDistance(querySentence, baseSentence, i, 0);
        for (int j = 0; j < baseSentence.count(); j++)
        {
            bestWordDistance = qMin(bestWordDistance, wordInSentenceDistance(querySentence, baseSentence, i, j));
        }
        result += bestWordDistance;
    }
    return result;
}

int SearchEngine::maximalRelevantDistance(const QStringList &querySentence)
{
    int summaryLength = 0;
    foreach (const QString &queryWord, querySentence)
    {
        summaryLength += queryWord.length();
    }
    // Несовпадений должно быть менее половины от длины запроса
    return summaryLength / 2;
}
