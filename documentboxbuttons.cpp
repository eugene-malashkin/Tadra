#include "documentboxbuttons.h"
#include "design.h"

//******************************************************************************************************
/*!
 *\class DocumentBoxButtons
 *\brief Группа кнопок в правом верхнем углу коробки с документом.
*/
//******************************************************************************************************

DocumentBoxButtons::DocumentBoxButtons(QObject *parent)
    :GraphicObject(parent)
    ,m_isWideMode(false)
    ,m_wideModeButton(NULL)
    ,m_closeButton(NULL)
    ,m_wideModeOffIcon()
    ,m_wideModeOnIcon()
{
    // Кнопка Распахнуть/Свернуть
    m_wideModeButton = new GraphicButton(this);
    initializeWideModeIcons();
    updateWideModeIconAndToolTip();
    connect(m_wideModeButton, SIGNAL(clicked()), this, SIGNAL(wideModeButtonClicked()));
    registerChildObject(m_wideModeButton);

    // Кнопка Закрыть
    m_closeButton = new GraphicButton(this);
    QIcon closeIcon;
    closeIcon.addFile("://resources/CloseNormal.png", QSize(), QIcon::Normal);
    closeIcon.addFile("://resources/CloseHover.png", QSize(), QIcon::Active);
    closeIcon.addFile("://resources/CloseDown.png", QSize(), QIcon::Selected);
    m_closeButton->setIcon(closeIcon);
    m_closeButton->setToolTip("Закрыть документ");
    connect(m_closeButton, SIGNAL(clicked()), this, SIGNAL(closeButtonClicked()));
    registerChildObject(m_closeButton);
}

void DocumentBoxButtons::setWideMode(bool value)
{
    if (m_isWideMode != value)
    {
        m_isWideMode = value;
        updateWideModeIconAndToolTip();
        updateAndRedraw();
    }
}

bool DocumentBoxButtons::isWideMode() const
{
    return m_isWideMode;
}

QSizeF DocumentBoxButtons::sizeConstraint(const QSizeF &) const
{
    int buttonSize = m_closeButton->iconSize().width();
    int buttonSpacing = Design::instance()->size(Design::DocumentBoxButtonSpacing);
    return QSizeF(buttonSize*2 + buttonSpacing, buttonSize);
}

void DocumentBoxButtons::resize()
{
    int buttonSize = m_closeButton->iconSize().width();
    int buttonSpacing = Design::instance()->size(Design::DocumentBoxButtonSpacing);
    int buttonTop = rect().top() + (rect().height() - buttonSize)/2;
    m_closeButton->setRect(QRectF(rect().right() - buttonSize, buttonTop, buttonSize, buttonSize));
    m_wideModeButton->setRect(QRectF(rect().right() - buttonSize*2 - buttonSpacing, buttonTop, buttonSize, buttonSize));
}

void DocumentBoxButtons::initializeWideModeIcons()
{
    m_wideModeOffIcon.addFile("://resources/WideModeOffNormal.png", QSize(), QIcon::Normal);
    m_wideModeOffIcon.addFile("://resources/WideModeOffHover.png", QSize(), QIcon::Active);
    m_wideModeOffIcon.addFile("://resources/WideModeOffDown.png", QSize(), QIcon::Selected);
    m_wideModeOnIcon.addFile("://resources/WideModeNormal.png", QSize(), QIcon::Normal);
    m_wideModeOnIcon.addFile("://resources/WideModeHover.png", QSize(), QIcon::Active);
    m_wideModeOnIcon.addFile("://resources/WideModeDown.png", QSize(), QIcon::Selected);
}

void DocumentBoxButtons::updateWideModeIconAndToolTip()
{
    if (m_isWideMode)
    {
        m_wideModeButton->setIcon(m_wideModeOffIcon);
        m_wideModeButton->setToolTip("Свернуть");
    }
    else
    {
        m_wideModeButton->setIcon(m_wideModeOnIcon);
        m_wideModeButton->setToolTip("Распахнуть");
    }
}
