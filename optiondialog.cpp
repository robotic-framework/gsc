#include "optiondialog.h"
#include "ui_optiondialog.h"

OptionDialog::OptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
    serial = Serial::instance();
    connect(serial, SIGNAL(newSerialPort(QStringList)), this, SLOT(onNewSerialPort(QStringList)));

    serial->ScanPorts();
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::on_pushButton_clicked()
{
    hide();
}

void OptionDialog::on_pushButton_2_clicked()
{
    hide();
}

void OptionDialog::onNewSerialPort(QStringList ports)
{
    ui->comboPorts->clear();
    ui->comboPorts->addItems(ports);

    if (serial->GetSelectedPort().isEmpty())
    {
        serial->SetSelectedPort(ports.constFirst());
    }
}

void OptionDialog::on_btnRefresh_clicked()
{
    serial->ScanPorts();
}

void OptionDialog::on_comboPorts_activated(const QString &arg1)
{
    serial->SetSelectedPort(arg1);
}

void OptionDialog::on_comboBaudrate_activated(const QString &arg1)
{
    serial->SetSelectedBaudrate(arg1);
}

void OptionDialog::on_comboDatabits_activated(const QString &arg1)
{
    serial->SetSelectedDatabits(arg1);
}

void OptionDialog::on_comboStopbits_activated(const QString &arg1)
{
    serial->SetSelectedStopbits(arg1);
}

void OptionDialog::on_comboParity_activated(const QString &arg1)
{
    serial->SetSelectedParity(arg1);
}
