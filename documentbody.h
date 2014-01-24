#ifndef DOCUMENTBODY_H
#define DOCUMENTBODY_H

#include <QWidget>

class DocumentBody : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentBody(QWidget *parent = NULL);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // DOCUMENTBODY_H
