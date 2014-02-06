#include "documentbox.h"
#include <QPainter>
#include <QMouseEvent>
#include <QSpacerItem>
#include "documentlayer.h"
#include "currencychartwidget.h"
#include "searchinput.h"
#include "design.h"
#include "placeroutine.h"
#include "floatroutine.h"
#include "graphicwidget.h"
#include "documentboxbuttons.h"

#include <QDebug>

DocumentBox::DocumentBox(DocumentLayer *parent)
    :QWidget(parent)
    ,m_layer(parent)
    ,m_orderIndex(0)
    ,m_centralLayout(NULL)
    ,m_headerLayout(NULL)
    ,m_searchInput(NULL)
    ,m_buttonsWidget(NULL)
    ,m_buttonsObject(NULL)
    ,m_chartWidget(NULL)
    ,m_stackRect()
    ,m_gridRect()
    ,m_screenRect()
    ,m_isWideMode(false)
    ,m_clickBound(RectBoundTypeNull)
    ,m_clickPoint()
{
    setMouseTracking(true);
    m_centralLayout = new QVBoxLayout;
    int m = Design::instance()->size(Design::DocumentBoxBorderSize);
    m_centralLayout->setContentsMargins(m, m, m, m);
    setLayout(m_centralLayout);

    m_headerLayout = new QHBoxLayout;
    m_searchInput = new SearchInput;
    m_searchInput->installEventFilter(this);
    m_searchInput->setStyleSheet(Design::instance()->styleSheet(Design::QueryEditStyleSheet));
    connect(m_searchInput, SIGNAL(instrumentChanged()), this, SLOT(onInstrumentChanged()));
    m_headerLayout->addWidget(m_searchInput);

    QSpacerItem *headerSpacer = new QSpacerItem(8, 16, QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_headerLayout->addItem(headerSpacer);

    m_buttonsWidget = new GraphicWidget;
    m_headerLayout->addWidget(m_buttonsWidget);
    m_buttonsObject = new DocumentBoxButtons(this);
    m_buttonsObject->setSupervisor(m_buttonsWidget);
    m_buttonsWidget->setGraphicObject(m_buttonsObject);
    m_buttonsWidget->updateAndRedraw();
    connect(m_buttonsObject, SIGNAL(wideModeButtonClicked()), this, SLOT(onWideModeButtonClicked()));
    connect(m_buttonsObject, SIGNAL(closeButtonClicked()), this, SLOT(onCloseButtonClicked()));

    m_centralLayout->addLayout(m_headerLayout);

    m_chartWidget = new CurrencyChartWidget;
    m_chartWidget->installEventFilter(this);
    m_centralLayout->addWidget(m_chartWidget);
}

DocumentLayer* DocumentBox::layer() const
{
    return m_layer;
}

void DocumentBox::setOrderIndex(int value)
{
    m_orderIndex = value;
}

int DocumentBox::orderIndex() const
{
    return m_orderIndex;
}

void DocumentBox::setStackRect(const QRectF &value)
{
    m_stackRect = value;
}

QRectF DocumentBox::stackRect() const
{
    return m_stackRect;
}

void DocumentBox::setGridRect(const QRect &value)
{
    m_gridRect = value;
}

QRect DocumentBox::gridRect() const
{
    return m_gridRect;
}

void DocumentBox::setScreenRect(const QRect &value)
{
    m_screenRect = value;
}

QRect DocumentBox::screenRect() const
{
    return m_screenRect;
}

void DocumentBox::setWideMode(bool value)
{
    m_isWideMode = value;
    m_buttonsObject->setWideMode(m_isWideMode);
}

bool DocumentBox::isWideMode() const
{
    return m_isWideMode;
}

void DocumentBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (m_isWideMode)
    {
        painter.fillRect(0, 0, width(), height(), Design::instance()->color(Design::DocumentBoxBgColor));
        painter.setPen(Design::instance()->color(Design::DocumentBoxOutFocusLineColor));
        painter.drawLine(0, 0, width(), 0);
    }
    else
    {
        QPen pen(Qt::black);
        pen.setWidth(1);
        painter.setPen(Design::instance()->color(Design::DocumentBoxOutFocusLineColor));
        painter.setBrush(Design::instance()->color(Design::DocumentBoxBgColor));
        painter.drawRect(0, 0, width()-1, height()-1);
    }
}

void DocumentBox::mousePressEvent(QMouseEvent *event)
{
    if (m_isWideMode)
    {
        return;
    }

    // Сбрасываем тип изменения размера
    m_clickBound = RectBoundTypeNull;
    if (event->button() == Qt::LeftButton)
    {
        m_clickBound = findBound(event->pos());
        m_clickPoint = event->pos();
    }
    setFocus();
}

void DocumentBox::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isWideMode)
    {
        setCursor(Qt::ArrowCursor);
        return;
    }

    // Устанавливаем курсор
    RectBoundType bound = m_clickBound;
    if (bound == RectBoundTypeNull)
    {
        bound = findBound(event->pos());
    }
    setCursor(rectBoundTypeToCursor(bound));

    if ((m_clickBound >= RectBoundTypeTopLeft) && (m_clickBound <= RectBoundTypeLeft))
    {
        // Идёт изменение размера
        emit resizing(m_clickBound, event->pos() + pos());
    }
    if (m_clickBound == RectBoundTypeTitle)
    {
        // Идёт перемещение
        emit moving(pos() + event->pos() - m_clickPoint);
    }
}

void DocumentBox::mouseReleaseEvent(QMouseEvent *)
{
    if (m_isWideMode)
    {
        return;
    }

    // Сбрасываем тип изменения размера
    m_clickBound = RectBoundTypeNull;
}

bool DocumentBox::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        raise();
    }
    return QWidget::eventFilter(object, event);
}

void DocumentBox::focusInEvent(QFocusEvent *)
{
    raise();
}

void DocumentBox::onWideModeButtonClicked()
{
    emit wideModeChanging();
}

void DocumentBox::onCloseButtonClicked()
{
    emit closing();
}

void DocumentBox::onInstrumentChanged()
{
    m_chartWidget->setInstrument(m_searchInput->instrument());
}

RectBoundType DocumentBox::findBound(const QPoint &point) const
{
    RectBoundType result = findRectBoundType(
                QRect(0, 0, width(), height()),
                Design::instance()->size(Design::DocumentBoxBorderSize),
                Design::instance()->size(Design::DocumentBoxCornerSize),
                Design::instance()->size(Design::DocumentBoxTitleSize),
                point);
    return result;
}
