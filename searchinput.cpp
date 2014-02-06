#include "searchinput.h"
#include <QKeyEvent>
#include "searchinputhighlight.h"

#include <QDebug>

//******************************************************************************************************
/*!
 *\class SearchInput
 *\brief Поле для ввода поискового запроса.
*/
//******************************************************************************************************

SearchInput::SearchInput(QWidget *parent)
    : QLineEdit(parent)
    , m_highlight(NULL)
    , m_instrument()
{
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
    m_highlight = new SearchInputHighlight(this);
    connect(m_highlight, SIGNAL(clicked()), this, SLOT(onHighlightClicked()));
}

CurrencyInstrument SearchInput::instrument() const
{
    return m_instrument;
}

void SearchInput::keyPressEvent(QKeyEvent *event)
{
    QLineEdit::keyPressEvent(event);
    if (event->key() == Qt::Key_Up)
    {
        m_highlight->previousIndex();
        takeHighlightedInstrument();
    }
    if (event->key() == Qt::Key_Down)
    {
        m_highlight->nextIndex();
        takeHighlightedInstrument();
    }
    if (event->key() == Qt::Key_Return)
    {
        takeHighlightedInstrument();
        m_highlight->hide();
    }
}

void SearchInput::onTextEdited(const QString &text)
{
    qDebug() << "text edited";
    // Запрос (текст без пробелов спереди и сзади)
    QString query = text.trimmed();

    // Получение всех вариантов
    CurrencyInstrumentRankedMap allVariants = SearchEngine::instance()->variants(query);

    // Ограничение вариантов (4 штуки with ties)
    QList<CurrencyInstrument> croppedVariants;
    int lastRank = 0;
    for (auto iter = allVariants.constBegin(); iter != allVariants.constEnd(); ++iter)
    {
        double rank = iter.key();
        if ((croppedVariants.count() < 4) || (rank == lastRank))
        {
            croppedVariants << iter.value();
        }
        else
        {
            break;
        }
        lastRank = rank;
    }

    QPoint hightlightPos = mapToGlobal(QPoint(0, 0)) + QPoint(0, height());
    m_highlight->move(hightlightPos);
    m_highlight->showInstruments(croppedVariants);
    m_highlight->setCurrentIndex(0);

    CurrencyInstrument topInstrument;
    if (!croppedVariants.isEmpty())
    {
        topInstrument = croppedVariants.first();
    }
    setInstrument(topInstrument);
}

void SearchInput::onHighlightClicked()
{
    takeHighlightedInstrument();
    m_highlight->hide();
}

void SearchInput::setInstrument(const CurrencyInstrument &value)
{
    if (m_instrument != value)
    {
        m_instrument = value;
        emit instrumentChanged();
    }
}

void SearchInput::takeHighlightedInstrument()
{
    int index = m_highlight->currentIndex();
    if ((index >= 0) && (index < m_highlight->instruments().count()))
    {
        CurrencyInstrument instrument = m_highlight->instruments()[index];
        setText(instrument.name);
        setInstrument(instrument);
    }
}
