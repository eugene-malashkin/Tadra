#ifndef CURRENCYCHARTWIDGET_H
#define CURRENCYCHARTWIDGET_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDate>
#include "currencyinstrument.h"
#include "graphicwidget.h"
#include "timelyaction.h"
#include "chartroutine.h"

struct CurrencyChartRow
{
    QDate date;
    double value;
    double nominal;
    CurrencyChartRow();
    CurrencyChartRow(const QDate &aDate, double aValue, double aNominal);
    bool operator == (const CurrencyChartRow &another);
    bool operator != (const CurrencyChartRow &another);
    bool isValid() const;
    QString toString() const;
    static bool lessThan(const CurrencyChartRow &r1, const CurrencyChartRow &r2);
};

class CurrencyChartTable : public QList<CurrencyChartRow>
{
public:
    bool isValid() const;
    QStringList toStringList() const;
};

class CurrencyChartDataSource : public QObject
{
    Q_OBJECT

public:
    CurrencyChartDataSource(QObject *parent = NULL);
    void setInstrument(const CurrencyInstrument &value);
    CurrencyInstrument instrument() const;
    bool isLoading() const;
    bool isOK() const;
    QString errorString() const;
    const CurrencyChartTable& table() const;

signals:
    void instrumentChanged();
    void loading();
    void done(bool ok);
    void tableChanged();

private slots:
    void query();
    void onManagerFinished(QNetworkReply *reply);

private:
    CurrencyInstrument m_instrument;
    TimelyAction *m_queryAction;
    QNetworkAccessManager *m_manager;
    bool m_isLoading;
    QString m_errorString;
    CurrencyChartTable m_table;
    static QString inputDateFormat();
    static QString outputDateFormat();
    QString url() const;
    void setTable(const CurrencyChartTable &value);
    void parseReply(const QByteArray &replyData);
};

class CurrencyChartWorkspace : public GraphicObject
{
    Q_OBJECT

public:
    CurrencyChartWorkspace(QObject *parent = NULL);
    void setInstrument(const CurrencyInstrument &value);
    CurrencyInstrument instrument() const;
    void setTable(const CurrencyChartTable &value);
    CurrencyChartTable table() const;
    void paint(QPainter *painter) override;
    QSizeF sizeConstraint(const QSizeF &supposedSize) const override;
    void resize() override;
    void update() override;

private:
    CurrencyInstrument m_instrument;
    CurrencyChartTable m_table;
    DateTimeScale m_dateTimeScale;
    FloatScale m_floatScale;
    QRectF outputRect() const;
    double changePercent() const;
    QColor baseColor() const;
    void computeRanges(QList<QDateTime> &dateTimeList, FloatRange &floatRange) const;
    void paintBackground(QPainter *painter);
    void paintDateTimeScale(QPainter *painter);
    void paintFloatScale(QPainter *painter);
    QPointF screenPoint(int index) const;
    void paintIndicator(QPainter *painter);
    void paintLast(QPainter *painter);
};

class CurrencyChartWidget : public GraphicWidget
{
    Q_OBJECT

public:
    CurrencyChartWidget(QWidget *parent = NULL);
    void setInstrument(const CurrencyInstrument &value);
    CurrencyInstrument instrument() const;
    CurrencyChartDataSource* dataSource() const;
    CurrencyChartWorkspace* workspace() const;

private slots:
    void onDataSourceInstrumentChanged();
    void onDataSourceTableChanged();

private:
    CurrencyChartDataSource *m_dataSource;
    CurrencyChartWorkspace *m_workspace;
};

#endif // CURRENCYCHARTWIDGET_H
