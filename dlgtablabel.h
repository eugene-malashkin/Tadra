#ifndef DLGTABLABEL_H
#define DLGTABLABEL_H

#include <QDialog>

namespace Ui {
class DlgTabLabel;
}

class DlgTabLabel : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTabLabel(QWidget *parent = NULL);
    void setLabel(const QString &value);
    QString label() const;
    ~DlgTabLabel() override;

private slots:
    void on_okButton_clicked();

private:
    Ui::DlgTabLabel *ui;
    QString m_label;
};

#endif // DLGTABLABEL_H
