#ifndef GRAPHICBUTTON_H
#define GRAPHICBUTTON_H

#include "graphicobject.h"
#include <QPixmap>
#include <QIcon>
#include <QFont>
#include <QMap>
#include <QAction>

class GraphicButton : public GraphicObject
{
    Q_OBJECT

public:
    enum State
    {
        StateNormal,
        StateDisabled,
        StateHover,
        StateDown
    };

    enum Style
    {
        StyleIconOnly,
        StyleTextBesideIcon
    };

public:
    GraphicButton(QObject *parent = NULL);
    GraphicButton(const QIcon &icon, const QString &text, const QString &toolTip, QObject *parent = NULL);
    void setText(const QString &value);
    QString text() const;
    void setIcon(const QIcon &value);
    QIcon icon() const;
    void setToolTip(const QString &value);
    QString toolTip() const;
    void setStyle(Style value);
    Style style() const;
    void setEnabled(bool value);
    bool isEnabled() const;
    void setIconSize(const QSizeF &value);
    QSizeF iconSize() const;
    void setFont(const QFont &value);
    QFont font() const;
    void setTextColor(const QColor &value);
    QColor textColor() const;
    State currentState() const;
    bool isDown() const;
    QSizeF sizeConstraint(const QSizeF &supposedSize) const override;
    void paint(QPainter *painter) override;
    void handleEvent(UserEvent event) override;
    HitInfo hitInfo(const QPointF &point) const override;

signals:
    void clicked();
    void down();
    void up();

private:
    QString m_text;
    QIcon m_icon;
    QString m_toolTip;
    Style m_style;
    bool m_isEnabled;
    QSizeF m_iconSize;
    QFont m_font;
    QColor m_textColor;
    State m_currentState;
    void setCurrentState(State value);
    QIcon::Mode iconMode() const;
};

#endif // GRAPHICBUTTON_H
