#include "currencychartwidget.h"
#include <QDomDocument>
#include <float.h>
#include <math.h>
#include "floatroutine.h"
#include "colorroutine.h"
#include "numeral.h"
#include "design.h"

#include <QDebug>

// TODO: Заменить значениями из Design
static const double ChartFloatScaleWidth = 64;
static const double ChartDateTimeScaleHeight = 24;
static const double ChartMinWidth = 192;
static const double ChartMinHeight = 128;
static const QColor ChartRaiseColor("#00a300");
static const QColor ChartUnchangedColor("#505050");
static const QColor ChartFallColor("#bf0000");
static const QColor ChartTextColor(Qt::white);
static const QColor ChartScaleLineColor(Qt::gray);
static const double ChartFloatScaleMargin = 6;
static const QColor ChartLineColor1(Qt::white);
static const double ChartLineWidth1(5);
static const QColor ChartLineColor2("#008080");
static const double ChartLineWidth2(1);
static const double ChartLastMargin(8);
static const double ChartLastHeight(24);
static const QColor ChartLastTextColor(Qt::black);
static const QColor ChartLastBgColor(Qt::white);
static const int ChartMinimalMarkSpacing(10);

//******************************************************************************************************
/*!
 *\struct CurrencyChartRow
 *\brief Строка с историческими данными для графика валюты.
*/
//******************************************************************************************************

CurrencyChartRow::CurrencyChartRow()
    : date()
    , value(getNaN())
    , nominal(getNaN())
{

}

CurrencyChartRow::CurrencyChartRow(const QDate &aDate, double aValue, double aNominal)
    : date(aDate)
    , value(aValue)
    , nominal(aNominal)
{

}

bool CurrencyChartRow::operator == (const CurrencyChartRow &another)
{
    return (date == another.date) && (doubleEquals(value, another.value)) && (doubleEquals(nominal, another.nominal));
}

bool CurrencyChartRow::operator != (const CurrencyChartRow &another)
{
    return !(operator ==(another));
}

bool CurrencyChartRow::isValid() const
{
    return (date.isValid()) && (!isNaN(value)) && (!isNaN(nominal));
}

QString CurrencyChartRow::toString() const
{
    return QString("%1 %2 (%3)").arg(date.toString("dd.MM.yyyy")).arg(value).arg(nominal);
}

bool CurrencyChartRow::lessThan(const CurrencyChartRow &r1, const CurrencyChartRow &r2)
{
    return r1.date < r2.date;
}


//******************************************************************************************************
/*!
 *\class CurrencyChartTable
 *\brief Данные (исторические) для графика.
*/
//******************************************************************************************************

bool CurrencyChartTable::isValid() const
{
    bool result = true;
    for (int i = 0; i < count(); i++)
    {
        if (!(this->operator [](i)).isValid())
        {
            result = false;
            break;
        }
    }
    return result;
}

QStringList CurrencyChartTable::toStringList() const
{
    QStringList result;
    result << QString("Table, rows count = %1").arg(count());
    for (int i = 0; i < count(); i++)
    {
        result << (this->operator [](i)).toString();
    }
    return result;
}


//******************************************************************************************************
/*!
 *\class CurrencyChartDataSource
 *\brief Источник данных для графика валюты.
*/
//******************************************************************************************************

CurrencyChartDataSource::CurrencyChartDataSource(QObject *parent)
    : QObject(parent)
    , m_instrument()
    , m_queryAction(NULL)
    , m_manager(NULL)
    , m_isLoading()
    , m_errorString()
    , m_table()
{
    m_queryAction = new TimelyAction(this);
    m_queryAction->setPeriodInSeconds(20);
    connect(m_queryAction, SIGNAL(triggered()), this, SLOT(query()));
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onManagerFinished(QNetworkReply*)));
}

void CurrencyChartDataSource::setInstrument(const CurrencyInstrument &value)
{
    if (m_instrument != value)
    {
        m_instrument = value;
        emit instrumentChanged();
        setTable(CurrencyChartTable());
        m_queryAction->actShortly();
    }
}

CurrencyInstrument CurrencyChartDataSource::instrument() const
{
    return m_instrument;
}

bool CurrencyChartDataSource::isLoading() const
{
    return m_isLoading;
}

bool CurrencyChartDataSource::isOK() const
{
    return (m_errorString.isEmpty());
}

QString CurrencyChartDataSource::errorString() const
{
    return m_errorString;
}

const CurrencyChartTable& CurrencyChartDataSource::table() const
{
    return m_table;
}

void CurrencyChartDataSource::query()
{
    if (!instrument().isValid())
    {
        return;
    }

    m_isLoading = true;
    m_errorString = QString();

    QNetworkRequest networkRequest;
    networkRequest.setUrl(QUrl(url()));
    m_manager->get(networkRequest);

    emit loading();
}

void CurrencyChartDataSource::onManagerFinished(QNetworkReply *reply)
{
    m_isLoading = false;
    m_errorString = reply->errorString();
    parseReply(reply->readAll());
    emit done(reply->error() == QNetworkReply::NoError);
}

QString CurrencyChartDataSource::inputDateFormat()
{
    return QString("dd/MM/yyyy");
}

QString CurrencyChartDataSource::outputDateFormat()
{
    return QString("dd.MM.yyyy");
}

QString CurrencyChartDataSource::url() const
{
    QDate firstDate = QDate::currentDate().addMonths(-12).addDays(2);
    QDate lastDate = QDate::currentDate().addDays(1);
    QString result = QString("http://www.cbr.ru/scripts/XML_dynamic.asp?date_req1=%1&date_req2=%2&VAL_NM_RQ=%3")
            .arg(firstDate.toString(inputDateFormat()))
            .arg(lastDate.toString(inputDateFormat()))
            .arg(instrument().id);
    return result;
}

void CurrencyChartDataSource::setTable(const CurrencyChartTable &value)
{
    if (m_table != value)
    {
        m_table = value;
        emit tableChanged();
    }
}

void CurrencyChartDataSource::parseReply(const QByteArray &replyData)
{
    QDomDocument document;
    if (document.setContent(replyData))
    {
        CurrencyChartTable tmpTable;
        QDomElement de = document.documentElement();
        for (QDomElement recordElement = de.firstChildElement("Record"); !recordElement.isNull(); recordElement = recordElement.nextSiblingElement("Record"))
        {
            QDate date = QDate::fromString(recordElement.attribute("Date"), outputDateFormat());
            bool valueOk = false;
            double value = recordElement.firstChildElement("Value").text().replace(",", ".").toDouble(&valueOk);
            bool nominalOk = false;
            double nominal = recordElement.firstChildElement("Nominal").text().replace(",", ".").toDouble(&nominalOk);
            if ((date.isValid()) && (valueOk) && (nominalOk))
            {
                tmpTable << CurrencyChartRow(date, value, nominal);
            }
        }
        setTable(tmpTable);
    }
    qSort(m_table.begin(), m_table.end(), CurrencyChartRow::lessThan);
}


//******************************************************************************************************
/*!
 *\class CurrencyChartWorkspace
 *\brief Рабочее пространство графика цены.
*/
//******************************************************************************************************

CurrencyChartWorkspace::CurrencyChartWorkspace(QObject *parent)
    : GraphicObject(parent)
    , m_instrument()
    , m_table()
    , m_dateTimeScale()
    , m_floatScale()
{
    QFont font;
    QFont scaleFont = Design::instance()->font(Design::ChartFont);
    m_dateTimeScale.setOrientation(Qt::Horizontal);
    m_dateTimeScale.setIntradayFlag(false);
    m_dateTimeScale.setMinimalMarkSpacing(ChartMinimalMarkSpacing);
    m_dateTimeScale.setFont(scaleFont);
    m_floatScale.setOrientation(Qt::Vertical);
    m_floatScale.setMinimalMarkSpacing(ChartMinimalMarkSpacing);
    m_floatScale.setFont(scaleFont);
}

void CurrencyChartWorkspace::setInstrument(const CurrencyInstrument &value)
{
    m_instrument = value;
}

CurrencyInstrument CurrencyChartWorkspace::instrument() const
{
    return m_instrument;
}

void CurrencyChartWorkspace::setTable(const CurrencyChartTable &value)
{
    m_table = value;
}

CurrencyChartTable CurrencyChartWorkspace::table() const
{
    return m_table;
}

void CurrencyChartWorkspace::paint(QPainter *painter)
{
    if (!m_table.isEmpty())
    {
        paintBackground(painter);
        paintDateTimeScale(painter);
        paintFloatScale(painter);
        paintIndicator(painter);
        paintLast(painter);
    }
}

QSizeF CurrencyChartWorkspace::sizeConstraint(const QSizeF &) const
{
    return QSizeF(ChartMinWidth, ChartMinHeight);
}

void CurrencyChartWorkspace::resize()
{
    m_dateTimeScale.setScreenPoints(ScreenPoints(rect().left(), rect().right() - ChartFloatScaleWidth));
    m_floatScale.setScreenPoints(ScreenPoints(rect().bottom() - ChartDateTimeScaleHeight - ChartFloatScaleMargin, rect().top() + ChartFloatScaleMargin));
}

void CurrencyChartWorkspace::update()
{
    QList<QDateTime> dateTimeList;
    FloatRange floatRange;
    computeRanges(dateTimeList, floatRange);
    m_dateTimeScale.setValues(dateTimeList);
    m_floatScale.setLogicRange(floatRange);
}

QRectF CurrencyChartWorkspace::outputRect() const
{
    return QRectF(rect().left(), rect().top(), rect().width() - ChartFloatScaleWidth, rect().height() - ChartDateTimeScaleHeight);
}

double CurrencyChartWorkspace::changePercent() const
{
    double result = getNaN();
    if (table().count() >= 2)
    {
        double open = table().first().value;
        if (open > FLT_EPSILON)
        {
            result = (table().last().value - open) / open;
        }
    }
    return result;
}

QColor CurrencyChartWorkspace::baseColor() const
{
    double k = qBound(-0.05, changePercent(), +0.05)/0.05;
    if (k > FLT_EPSILON)
    {
        // Зелёный
        return blendColor(ChartUnchangedColor, ChartRaiseColor, k);
    }
    else if (k < -FLT_EPSILON)
    {
        // Красный
        return blendColor(ChartUnchangedColor, ChartFallColor, -k);
    }
    else
    {
        return ChartUnchangedColor;
    }
}

void CurrencyChartWorkspace::computeRanges(QList<QDateTime> &dateTimeList, FloatRange &floatRange) const
{
    dateTimeList.clear();
    floatRange.clear();
    foreach (const CurrencyChartRow &row, m_table)
    {
        dateTimeList << QDateTime(row.date);
        floatRange << row.value;
    }
}

void CurrencyChartWorkspace::paintBackground(QPainter *painter)
{
    QColor bgColor = baseColor();
    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0, bgColor);
    gradient.setColorAt(1, bgColor.darker());
    painter->fillRect(rect(), QBrush(gradient));
}

void CurrencyChartWorkspace::paintDateTimeScale(QPainter *painter)
{
    QPen linePen(ChartScaleLineColor);
    linePen.setStyle(Qt::DashLine);
    QPen textPen(ChartTextColor);

    QFont font = Design::instance()->font(Design::ChartFont);
    painter->setFont(font);
    QFontMetricsF fm(font);
    DateTimeScaleMarkList markList = m_dateTimeScale.markList();
    foreach (const DateTimeScaleMark &mark, markList)
    {
        painter->setPen(linePen);
        painter->drawLine(mark.position, rect().top(), mark.position, rect().bottom());
        double textWidth = fm.width(mark.text)+1;
        QRectF textRect(mark.position + ChartMinimalMarkSpacing/2.0, rect().bottom() - ChartDateTimeScaleHeight, textWidth, ChartDateTimeScaleHeight);
        painter->setPen(textPen);
        painter->drawText(textRect, Qt::AlignCenter, mark.text);
    }
}

void CurrencyChartWorkspace::paintFloatScale(QPainter *painter)
{
    QPen linePen(ChartScaleLineColor);
    linePen.setStyle(Qt::DashLine);
    QPen textPen(ChartTextColor);

    QFont font = Design::instance()->font(Design::ChartFont);
    painter->setFont(font);
    QFontMetricsF fm(font);
    FloatScaleMarkList markList = m_floatScale.markList();
    foreach (const FloatScaleMark &mark, markList)
    {
        painter->setPen(linePen);
        painter->drawLine(rect().left(), mark.position, rect().right() - ChartFloatScaleWidth, mark.position);
        QRectF textRect(rect().right() - ChartFloatScaleWidth + ChartFloatScaleMargin, mark.position - fm.height()/2, ChartFloatScaleWidth - ChartFloatScaleMargin, fm.height());
        painter->setPen(textPen);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, mark.text);
    }
}

QPointF CurrencyChartWorkspace::screenPoint(int index) const
{
    return QPointF(m_dateTimeScale.logicToScreen(index), m_floatScale.logicToScreen(m_table[index].value));
}

void CurrencyChartWorkspace::paintIndicator(QPainter *painter)
{
    if (!m_table.isEmpty())
    {
        QPolygonF pathLine;
        for (int i = 0; i < m_table.count(); i++)
        {
            pathLine << screenPoint(i);
        }
        QPolygonF pathFill = pathLine;
        double x1 = screenPoint(0).x();
        double x2 = screenPoint(m_table.count()-1).x();
        pathFill.insert(0, QPointF(x1, rect().bottom() - ChartDateTimeScaleHeight));
        pathFill.append(QPointF(x2, rect().bottom() - ChartDateTimeScaleHeight));

        QRectF gr = outputRect();
        QLinearGradient gradient(gr.topLeft(), gr.bottomLeft());
        gradient.setColorAt(0, QColor(0, 0, 0, 128));
        gradient.setColorAt(1, QColor(0, 0, 0, 0));
        painter->setPen(Qt::NoPen);
        painter->setBrush(gradient);
        painter->drawPolygon(pathFill);

        painter->setRenderHint(QPainter::Antialiasing);
        QPen pen1(ChartLineColor1);
        pen1.setWidthF(ChartLineWidth1);
        painter->setPen(pen1);
        painter->drawPolyline(pathLine);
        QPen pen2(ChartLineColor2);
        pen2.setWidthF(ChartLineWidth2);
        painter->setPen(pen2);
        painter->drawPolyline(pathLine);
    }
    painter->setRenderHint(QPainter::Antialiasing, false);
}

void CurrencyChartWorkspace::paintLast(QPainter *painter)
{
    if (m_table.isEmpty())
    {
        return;
    }
    double last = m_table.last().value;
    double y = m_floatScale.logicToScreen(last);
    QPolygonF polygon;
    polygon << QPointF(rect().right()-ChartFloatScaleWidth, y);
    polygon << QPointF(rect().right()-ChartFloatScaleWidth+ChartLastMargin, y-ChartLastHeight/2.0);
    polygon << QPointF(rect().right(), y-ChartLastHeight/2.0);
    polygon << QPointF(rect().right(), y+ChartLastHeight/2.0);
    polygon << QPointF(rect().right()-ChartFloatScaleWidth+ChartLastMargin, y+ChartLastHeight/2.0);
    polygon << QPointF(rect().right()-ChartFloatScaleWidth, y);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(ChartLastBgColor);
    painter->drawPolygon(polygon);

    QString text = Numeral::format(last);
    QRectF textRect(rect().right()-ChartFloatScaleWidth+ChartLastMargin, y-ChartLastHeight/2.0, ChartFloatScaleWidth-ChartLastMargin, ChartLastHeight);
    painter->setPen(ChartLastTextColor);
    painter->setFont(Design::instance()->font(Design::ChartLastFont));
    painter->drawText(textRect, Qt::AlignCenter, text);
}


//******************************************************************************************************
/*!
 *\class CurrencyChartWidget
 *\brief График валюты.
*/
//******************************************************************************************************

CurrencyChartWidget::CurrencyChartWidget(QWidget *parent)
    : GraphicWidget(parent)
    , m_dataSource(NULL)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_dataSource = new CurrencyChartDataSource(this);
    m_workspace = new CurrencyChartWorkspace(this);
    m_workspace->setSupervisor(this);
    setGraphicObject(m_workspace);

    connect(m_dataSource, SIGNAL(instrumentChanged()), this, SLOT(onDataSourceInstrumentChanged()));
    connect(m_dataSource, SIGNAL(tableChanged()), this, SLOT(onDataSourceTableChanged()));
}

void CurrencyChartWidget::setInstrument(const CurrencyInstrument &value)
{
    m_dataSource->setInstrument(value);
}

CurrencyInstrument CurrencyChartWidget::instrument() const
{
    return m_dataSource->instrument();
}

CurrencyChartDataSource* CurrencyChartWidget::dataSource() const
{
    return m_dataSource;
}

CurrencyChartWorkspace* CurrencyChartWidget::workspace() const
{
    return m_workspace;
}

void CurrencyChartWidget::onDataSourceInstrumentChanged()
{
    workspace()->setInstrument(dataSource()->instrument());
    workspace()->update();
    redraw();
}

void CurrencyChartWidget::onDataSourceTableChanged()
{
    workspace()->setTable(dataSource()->table());
    workspace()->update();
    redraw();
}
