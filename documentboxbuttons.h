#ifndef DOCUMENTBOXBUTTONS_H
#define DOCUMENTBOXBUTTONS_H

#include "graphicbutton.h"

class DocumentBoxButtons : public GraphicObject
{
    Q_OBJECT

public:
    DocumentBoxButtons(QObject *parent = NULL);
    void setWideMode(bool value);
    bool isWideMode() const;
    QSizeF sizeConstraint(const QSizeF &supposedSize) const override;
    void resize() override;

signals:
    void wideModeButtonClicked();
    void closeButtonClicked();

private:
    bool m_isWideMode;
    GraphicButton *m_wideModeButton;
    GraphicButton *m_closeButton;
    QIcon m_wideModeOffIcon;
    QIcon m_wideModeOnIcon;
    void initializeWideModeIcons();
    void updateWideModeIconAndToolTip();
};

#endif // DOCUMENTBOXBUTTONS_H
