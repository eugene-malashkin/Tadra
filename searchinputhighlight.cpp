#include "searchinputhighlight.h"
#include <QFontMetrics>
#include <QPainter>
#include <QMouseEvent>

#include <QDebug>

static const QColor SearchInputHighlightLineColor(Qt::black);
static const QColor SearchInputHighlightNormalTextColor(Qt::black);
static const QColor SearchInputHighlightNormalBgColor("#808080");
static const QColor SearchInputHighlightActiveTextColor(Qt::white);
static const QColor SearchInputHighlightActiveBgColor("#008080");
static const int SearchInputHighlightItemMargin(4);
static const int SearchInputHighlightItemHeight(18);

//******************************************************************************************************
/*!
 *\class SearchInputHighlight
 *\brief Подсветка близких вариантов поискового запроса.
*/
//******************************************************************************************************

SearchInputHighlight::SearchInputHighlight(QWidget *parent)
    : QWidget(parent, Qt::Window | Qt::FramelessWindowHint | Qt::ToolTip)
    , m_instruments()
    , m_currentIndex(-1)
{
    setMouseTracking(true);
}

void SearchInputHighlight::showInstruments(const QList<CurrencyInstrument> value)
{
    if (m_instruments != value)
    {
        m_instruments = value;
        if (!m_instruments.isEmpty())
        {
            updateSize();
        }
        update();
    }
    setVisible(!m_instruments.isEmpty());
}

QList<CurrencyInstrument> SearchInputHighlight::instruments() const
{
    return m_instruments;
}

void SearchInputHighlight::setCurrentIndex(int value)
{
    if (m_currentIndex != value)
    {
        m_currentIndex = value;
        update();
    }
}

int SearchInputHighlight::currentIndex() const
{
    return m_currentIndex;
}

void SearchInputHighlight::nextIndex()
{
    if (m_currentIndex < m_instruments.count()-1)
    {
        setCurrentIndex(m_currentIndex + 1);
    }
    else
    {
        setCurrentIndex(0);
    }
}

void SearchInputHighlight::previousIndex()
{
    if (m_currentIndex > 0)
    {
        setCurrentIndex(m_currentIndex - 1);
    }
    else
    {
        setCurrentIndex(m_instruments.count()-1);
    }
}

void SearchInputHighlight::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(SearchInputHighlightLineColor);
    painter.setBrush(SearchInputHighlightNormalBgColor);
    painter.drawRect(0, 0, width()-1, height()-1);

    for (int i = 0; i < m_instruments.count(); i++)
    {
        QRect itemRect(1, SearchInputHighlightItemMargin + i*SearchInputHighlightItemHeight, width()-2, SearchInputHighlightItemHeight);
        if (i == currentIndex())
        {
            painter.fillRect(itemRect, SearchInputHighlightActiveBgColor);
            painter.setPen(SearchInputHighlightActiveTextColor);
        }
        else
        {
            painter.setPen(SearchInputHighlightNormalTextColor);
        }
        QRect textRect = itemRect;
        textRect.setLeft(SearchInputHighlightItemMargin);
        textRect.setRight(width()-SearchInputHighlightItemMargin);
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_instruments[i].name);
    }
}

void SearchInputHighlight::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}

void SearchInputHighlight::mouseMoveEvent(QMouseEvent *event)
{
    setCurrentIndex(hitIndex(event->pos()));
}

void SearchInputHighlight::updateSize()
{
    QFontMetrics fm(font());
    int width = 0;
    foreach (const CurrencyInstrument &instrument, m_instruments)
    {
        width = qMax(width, fm.width(instrument.name));
    }
    int height = SearchInputHighlightItemHeight * m_instruments.count();
    resize(width + SearchInputHighlightItemMargin*2, height + SearchInputHighlightItemMargin*2);
}

int SearchInputHighlight::hitIndex(const QPoint &pos) const
{
    int result = -1;
    if (!m_instruments.isEmpty())
    {
        result = qBound(0, (pos.y() - SearchInputHighlightItemMargin) / SearchInputHighlightItemHeight, m_instruments.count());
    }
    return result;
}
