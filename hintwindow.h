#ifndef HINTWINDOW_H
#define HINTWINDOW_H

#include <QWidget>

namespace Ui {
class HintWindow;
}

class HintWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HintWindow(QWidget *parent = 0);
    ~HintWindow() override;
    void setText(const QString &value);
    QString text() const;
    void showAt(const QPoint &cursorPosition);

private:
    Ui::HintWindow *ui;
    static QSize screenSize(const QWidget *widget);
};

#endif // HINTWINDOW_H
