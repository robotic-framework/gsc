#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QList>
#include <QListWidgetItem>
#include <QBluetoothDeviceInfo>
#include "serial.h"
#include "ble.h"
#include "protocol.h"

namespace Ui {
    class OptionDialog;
}

enum ProtocolType
{
    SERIAL,
    BLE4
};

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = nullptr);
    ~OptionDialog();

    ProtocolType GetProtocolType() { return _type; };
    QBluetoothDeviceInfo GetCurrentBle() { return currentBle; };

private slots:
    void on_pushButton_clicked();
    void onNewSerialPort(QStringList);
    void onNewBLEDeviceAdd(const QBluetoothDeviceInfo &info);

    void on_btnRefresh_clicked();
    void on_comboPorts_activated(const QString &arg1);

    void on_comboBaudrate_activated(const QString &arg1);

    void on_comboDatabits_activated(const QString &arg1);

    void on_comboStopbits_activated(const QString &arg1);

    void on_comboParity_activated(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_btnRefreshBLE_clicked();

    void on_listBLE_itemClicked(QListWidgetItem *item);

    void on_tabWidget_currentChanged(int index);

private:
    Ui::OptionDialog *ui;
    Serial *serial;
    BLE *ble;
    QList<QBluetoothDeviceInfo> deviceList;
    QBluetoothDeviceInfo currentBle;

    ProtocolType _type = SERIAL;
};

#endif // OPTIONDIALOG_H
