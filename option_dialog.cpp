#include "option_dialog.h"
#include "ui_optiondialog.h"
#include <QBluetoothUuid>

OptionDialog::OptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);
    serial = Serial::instance();
    connect(serial, SIGNAL(newSerialPort(QStringList)), this, SLOT(onNewSerialPort(QStringList)));

    serial->Scan();

    ble = BLE::instance();
    connect(ble, SIGNAL(newDeviceAdd(const QBluetoothDeviceInfo &)), this, SLOT(onNewBLEDeviceAdd(const QBluetoothDeviceInfo &)));
    connect(ble, &BLE::scanFinished, this, [this]()
    {
        ui->btnRefreshBLE->setDisabled(false);
    });
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::on_pushButton_clicked()
{
    ble->StopScan();
    hide();
}

void OptionDialog::on_pushButton_2_clicked()
{
    ble->StopScan();
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

void OptionDialog::onNewBLEDeviceAdd(const QBluetoothDeviceInfo &info)
{
    if (!(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) || info.name().isEmpty())
    {
        return;
    }

    QString label = QString("%1 %2").arg(info.name(), info.deviceUuid().toString());
    QList<QListWidgetItem *> items = ui->listBLE->findItems(label, Qt::MatchExactly);
    if (items.empty())
    {
        ui->listBLE->addItem(label);
        deviceList.append(info);
    }
}

void OptionDialog::on_btnRefresh_clicked()
{
    serial->Scan();
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

void OptionDialog::on_btnRefreshBLE_clicked()
{
    ui->listBLE->clear();
    ui->btnRefreshBLE->setDisabled(true);
    ble->Scan();
}


void OptionDialog::on_listBLE_itemClicked(QListWidgetItem *item)
{
    QString text = item->text();
    QStringList tmp = text.split(" ");
    if (tmp.length() > 1)
    {
        ui->lblBLEName->setText(tmp[0]);
        ui->lblBLEAddr->setText(tmp[1]);

        for (int i = 0; i < deviceList.count(); i++)
        {
            if (deviceList[i].deviceUuid().toString() == tmp[1])
            {
                ble->SetCurrentDevice(deviceList[i]);
                break;
            }
        }
    }
}

void OptionDialog::on_tabWidget_currentChanged(int index)
{
    switch (index)
    {
    case 0:
        _type = SERIAL;
        break;
    case 1:
        _type = BLE4;
        break;
    }
}
