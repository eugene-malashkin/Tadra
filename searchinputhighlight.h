#ifndef SEARCHINPUTHIGHLIGHT_H
#define SEARCHINPUTHIGHLIGHT_H

#include <QWidget>
#include "searchengine.h"

class SearchInputHighlight : public QWidget
{
    Q_OBJECT

public:
    explicit SearchInputHighlight(QWidget *parent = NULL);
    void showInstruments(const QList<CurrencyInstrument> value);
    QList<CurrencyInstrument> instruments() const;
    void setCurrentIndex(int value);
    int currentIndex() const;
    void nextIndex();
    void previousIndex();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QList<CurrencyInstrument> m_instruments;
    int m_currentIndex;
    void updateSize();
    int hitIndex(const QPoint &pos) const;
};

#endif // SEARCHINPUTHIGHLIGHT_H
