#include "optiondialog.h"
#include "ui_optiondialog.h"
#include "ble.h"
#include <QBluetoothUuid>
#include <QMessageBox>

OptionDialog::OptionDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::OptionDialog),
        _type(SERIAL),
        bridge(ConnectorBridge::instance()),
        serialConfig(new SerialInfo),
        bleConfig(new QBluetoothDeviceInfo) {
    ui->setupUi(this);

    serialConfig->rate = "115200";
    serialConfig->dataBits = "8";
    serialConfig->parity = "None";
    serialConfig->stopBits = "1";
    ui->comboBaudrate->setCurrentText(serialConfig->rate);
    ui->comboDatabits->setCurrentText(serialConfig->dataBits);
    ui->comboParity->setCurrentText(serialConfig->parity);
    ui->comboStopbits->setCurrentText(serialConfig->stopBits);
    bridge->SetConfig(serialConfig);

    connect(bridge, SIGNAL(newSerialPort(QStringList)), this, SLOT(onNewSerialPort(QStringList)));
    connect(bridge, SIGNAL(newDeviceInfo(const QBluetoothDeviceInfo &)), this,
            SLOT(onNewBLEDeviceAdd(const QBluetoothDeviceInfo &)));
    connect(bridge, &ConnectorBridge::scanFinished, this, [this]() {
        ui->btnRefreshBLE->setDisabled(false);
    });

    bridge->Scan();
}

OptionDialog::~OptionDialog() {
    delete ui;
}

void OptionDialog::on_btnConfirm_clicked() {
    if (_type == SERIAL) {
        if (!serialConfig->port.isEmpty() &&
            !serialConfig->rate.isEmpty() &&
            !serialConfig->dataBits.isEmpty() &&
            !serialConfig->parity.isEmpty() &&
            !serialConfig->stopBits.isEmpty()) {
            bridge->SetConfig(serialConfig);
            emit connectionConfirm(SERIAL, serialConfig);
        } else {
            QMessageBox::critical(nullptr, "未指定配置", "未选择有效的串口设备", QMessageBox::Ok);
            return;
        }
    } else {
        if (bleConfig->isValid()) {
            auto conf = new BLEInfo(*bleConfig);
            bridge->SetConfig(conf);
            emit connectionConfirm(BLE4, conf);
        } else {
            QMessageBox::critical(nullptr, "未指定配置", "未选择需要连接的蓝牙设备", QMessageBox::Ok);
            return;
        }
    }

    bridge->StopScan();
    hide();
}

void OptionDialog::on_btnCancel_clicked() {
    bridge->StopScan();
    hide();
}

void OptionDialog::onNewSerialPort(QStringList ports) {
    ui->comboPorts->clear();
    ui->comboPorts->addItems(ports);

    if (serialConfig->port.isEmpty()) {
        serialConfig->port = ports.constFirst();
        ui->comboPorts->setCurrentText(ports.constFirst());
    }
}

void OptionDialog::onNewBLEDeviceAdd(const QBluetoothDeviceInfo &info) {
    if (!(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) || info.name().isEmpty()) {
        return;
    }

    QString label = QString("%1 %2").arg(info.name(), info.deviceUuid().toString());
    QList<QListWidgetItem *> items = ui->listBLE->findItems(label, Qt::MatchExactly);
    if (items.empty()) {
        ui->listBLE->addItem(label);
        deviceList.append(info);
    }
}

void OptionDialog::on_btnRefresh_clicked() {
    Serial::instance()->Scan();
}

void OptionDialog::on_comboPorts_activated(const QString &arg1) {
    serialConfig->port = arg1;
}

void OptionDialog::on_comboBaudrate_activated(const QString &arg1) {
    serialConfig->rate = arg1;
}

void OptionDialog::on_comboDatabits_activated(const QString &arg1) {
    serialConfig->dataBits = arg1;
}

void OptionDialog::on_comboStopbits_activated(const QString &arg1) {
    serialConfig->stopBits = arg1;
}

void OptionDialog::on_comboParity_activated(const QString &arg1) {
    serialConfig->parity = arg1;
}

void OptionDialog::on_btnRefreshBLE_clicked() {
    ui->listBLE->clear();
    ui->btnRefreshBLE->setDisabled(true);
    BLE::instance()->Scan();
}


void OptionDialog::on_listBLE_itemClicked(QListWidgetItem *item) {
    QString text = item->text();
    QStringList tmp = text.split(" ");
    if (tmp.length() > 1) {
        ui->lblBLEName->setText(tmp[0]);
        ui->lblBLEAddr->setText(tmp[1]);

        for (int i = 0; i < deviceList.count(); i++) {
            if (deviceList[i].deviceUuid().toString() == tmp[1]) {
                bleConfig = &deviceList[i];
                break;
            }
        }
    }
}

void OptionDialog::on_tabWidget_currentChanged(int index) {
    switch (index) {
        case 0:
            _type = SERIAL;
            break;
        case 1:
            _type = BLE4;
            break;
    }
}
