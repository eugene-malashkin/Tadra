#include "graphicbutton.h"
#include <QFontMetricsF>
#include <float.h>
#include <math.h>
#include "floatroutine.h"
#include "design.h"

#include <QDebug>

//******************************************************************************************************
/*!
 *\class GraphicButton
 *\brief Кнопка.
*/
//******************************************************************************************************

GraphicButton::GraphicButton(QObject *parent)
    :GraphicObject(parent)
    ,m_text()
    ,m_icon()
    ,m_toolTip()
    ,m_style(StyleIconOnly)
    ,m_isEnabled(true)
    ,m_iconSize(Design::instance()->size(Design::GraphicButtonDefaultIconSize), Design::instance()->size(Design::GraphicButtonDefaultIconSize))
    ,m_font()
    ,m_textColor(Design::instance()->color(Design::GraphicButtonTextColor))
    ,m_currentState(StateNormal)
{

}

GraphicButton::GraphicButton(const QIcon &icon, const QString &text, const QString &toolTip, QObject *parent)
    :GraphicObject(parent)
    ,m_text(text)
    ,m_icon(icon)
    ,m_toolTip(toolTip)
    ,m_style(StyleIconOnly)
    ,m_iconSize(Design::instance()->size(Design::GraphicButtonDefaultIconSize), Design::instance()->size(Design::GraphicButtonDefaultIconSize))
    ,m_font()
    ,m_textColor(Design::instance()->color(Design::GraphicButtonTextColor))
    ,m_currentState(StateNormal)
{

}

void GraphicButton::setText(const QString &value)
{
    if (m_text != value)
    {
        m_text = value;
        updateAndRedraw();
    }
}

QString GraphicButton::text() const
{
    return m_text;
}

void GraphicButton::setIcon(const QIcon &value)
{
    m_icon = value; // Для QIcon нет оператора сравнения
    updateAndRedraw();
}

QIcon GraphicButton::icon() const
{
    return m_icon;
}

void GraphicButton::setToolTip(const QString &value)
{
    if (m_toolTip != value)
    {
        m_toolTip = value;
        updateAndRedraw();
    }
}

QString GraphicButton::toolTip() const
{
    return m_toolTip;
}

void GraphicButton::setStyle(Style value)
{
    if (m_style != value)
    {
        m_style = value;
        updateAndRedraw();
    }
}

GraphicButton::Style GraphicButton::style() const
{
    return m_style;
}

void GraphicButton::setIconSize(const QSizeF &value)
{
    if (m_iconSize != value)
    {
        m_iconSize = value;
        updateAndRedraw();
    }
}

void GraphicButton::setEnabled(bool value)
{
    if (m_isEnabled != value)
    {
        m_isEnabled = value;
        m_currentState = (value)? StateNormal : StateDisabled;
        if (!m_isEnabled)
        {
            setHandleUsing(false);
        }
        updateAndRedraw();
    }
}

bool GraphicButton::isEnabled() const
{
    return m_isEnabled;
}

QSizeF GraphicButton::iconSize() const
{
    return m_iconSize;
}

void GraphicButton::setFont(const QFont &value)
{
    if (m_font != value)
    {
        m_font = value;
        updateAndRedraw();
    }
}

QFont GraphicButton::font() const
{
    return m_font;
}

void GraphicButton::setTextColor(const QColor &value)
{
    if (m_textColor != value)
    {
        m_textColor = value;
        updateAndRedraw();
    }
}

QColor GraphicButton::textColor() const
{
    return m_textColor;
}

GraphicButton::State GraphicButton::currentState() const
{
    return m_currentState;
}

QSizeF GraphicButton::sizeConstraint(const QSizeF &) const
{
    QSizeF result = iconSize();
    if (m_style == StyleTextBesideIcon)
    {
        QFontMetricsF fm(m_font);
        result.setWidth(
                    result.width() +
                    Design::instance()->size(Design::GraphicButtonIconTextSpacing) +
                    fm.width(m_text)
                    );
        result.setHeight(qMax(result.height(), fm.height()));
    }
    return result;
}

bool GraphicButton::isDown() const
{
    return (m_currentState == StateDown);
}

void GraphicButton::paint(QPainter *painter)
{
    if (m_style == StyleIconOnly)
    {
        double remainHorizontalSpace = (rect().width() - m_iconSize.width());
        QRect iconRect(iround( rect().left() + remainHorizontalSpace/2.0 ), iround( (rect().top() + rect().bottom() - m_iconSize.height())/2.0 ), iround( m_iconSize.width() ), iround( m_iconSize.height() ));
        m_icon.paint(painter, iconRect, Qt::AlignCenter, iconMode());
    }

    if (m_style == StyleTextBesideIcon)
    {
        QFontMetricsF fm(m_font);
        double remainHorizontalSpace = (rect().width() - m_iconSize.width() - fm.width(m_text));
        QRect iconRect(iround( rect().left() + remainHorizontalSpace/3.0 ), iround( (rect().top() + rect().bottom() - m_iconSize.height())/2.0 ), iround( m_iconSize.width() ), iround( m_iconSize.height() ));
        m_icon.paint(painter, iconRect, Qt::AlignCenter, iconMode());

        QRect textRect(iconRect.right() + iround(Design::instance()->size(Design::GraphicButtonIconTextSpacing)), iround( (rect().top() + rect().bottom() - fm.height())/2.0 ), ceil(fm.width(m_text)), ceil(fm.height()));
        painter->setPen(m_textColor);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_text);
    }
}

void GraphicButton::handleEvent(UserEvent event)
{
    if (!isEnabled())
    {
        return;
    }
    if (event.type == UserEvent::MouseDown)
    {
        if (event.button == Qt::LeftButton)
        {
            setHandleUsing(true);
            setCurrentState(StateDown);
        }
        else
        {
            setHandleUsing(false);
            setCurrentState(StateHover);
        }
    }
    if (event.type == UserEvent::MouseUp)
    {
        setHandleUsing(false);

        State previousState = currentState();
        setCurrentState(StateHover);

        if ((event.button == Qt::LeftButton) && (previousState == StateDown))
        {
            emit clicked();
        }
    }
    if (event.type == UserEvent::MouseMove)
    {
        if (!handleUsing())
        {
            setCurrentState(StateHover);
        }
        else
        {
            setCurrentState( (rect().contains(event.mousePosition))? StateDown : StateHover );
        }
    }
    if (event.type == UserEvent::MouseLeave)
    {
        setCurrentState(StateNormal);
    }
}

HitInfo GraphicButton::hitInfo(const QPointF &point) const
{
    HitInfo hitInfo;
    if (rect().contains(point))
    {
        hitInfo.result = true;
        if (!m_toolTip.isEmpty())
        {
            hitInfo.hint = wrapHintToHtml(m_toolTip);
        }
    }
    return hitInfo;
}

void GraphicButton::setCurrentState(State value)
{
    if (m_currentState != value)
    {
        if (value == StateDown)
        {
            emit down();
        }
        else if (m_currentState == StateDown)
        {
            emit up();
        }
        m_currentState = value;
        redraw();
    }
}

QIcon::Mode GraphicButton::iconMode() const
{
    QIcon::Mode result = QIcon::Normal;
    if (m_currentState == StateDisabled)    result = QIcon::Disabled;
    if (m_currentState == StateHover)       result = QIcon::Active;
    if (m_currentState == StateDown)        result = QIcon::Selected;
    return result;
}
