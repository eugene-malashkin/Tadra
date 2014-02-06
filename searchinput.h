#ifndef SEARCHINPUT_H
#define SEARCHINPUT_H

#include <QLineEdit>
#include "searchengine.h"

class SearchInputHighlight;

class SearchInput : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchInput(QWidget *parent = NULL);
    CurrencyInstrument instrument() const;

signals:
    void instrumentChanged();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTextEdited(const QString &text);
    void onHighlightClicked();

private:
    SearchInputHighlight *m_highlight;
    CurrencyInstrument m_instrument;
    void setInstrument(const CurrencyInstrument &value);
    void takeHighlightedInstrument();
};

#endif // SEARCHINPUT_H
