#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QObject>
#include <QNetworkReply>
#include <QMultiMap>
#include <QStringList>
#include "singletont.h"
#include "currencyinstrument.h"

typedef QMultiMap<double,CurrencyInstrument> CurrencyInstrumentRankedMap;

class SearchEngine : public QObject, public SingletonT<SearchEngine>
{
    Q_OBJECT

public:
    SearchEngine();
    void loadInstruments();
    CurrencyInstrumentRankedMap variants(const QString &query) const;

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QList<CurrencyInstrument> m_instruments;
    static int wordDistance(const QString &queryWord, const QString &baseWord);
    static int wordInSentenceDistance(const QStringList &querySentence, const QStringList &baseSentence, int queryWordIndex, int baseWordIndex);
    static int sentenceDistance(const QStringList &querySentence, const QStringList &baseSentence);
    static int maximalRelevantDistance(const QStringList &querySentence);
};

#endif // SEARCHENGINE_H
