#include "dlgtablabel.h"
#include "ui_dlgtablabel.h"
#include <QMessageBox>

DlgTabLabel::DlgTabLabel(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgTabLabel)
    , m_label()
{
    ui->setupUi(this);
}

void DlgTabLabel::setLabel(const QString &value)
{
    m_label = value;
    ui->lineEdit->setText(m_label);
}

QString DlgTabLabel::label() const
{
    return m_label;
}

DlgTabLabel::~DlgTabLabel()
{
    delete ui;
}

void DlgTabLabel::on_okButton_clicked()
{
    QString tmpLabel = ui->lineEdit->text().trimmed();
    if (tmpLabel.isEmpty())
    {
        QMessageBox::information(this, "Ошибка", "Пустое имя вкладки недопустимо. Укажите непустое название и повторите попытку.");
        return;
    }
    m_label = tmpLabel;
    accept();
}
