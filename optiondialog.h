#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QStringList>
#include "serial.h"
#include "protocol.h"

namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = nullptr);
    ~OptionDialog();

private slots:
    void on_pushButton_clicked();
    void onNewSerialPort(QStringList);

    void on_btnRefresh_clicked();
    void on_comboPorts_activated(const QString &arg1);

    void on_comboBaudrate_activated(const QString &arg1);

    void on_comboDatabits_activated(const QString &arg1);

    void on_comboStopbits_activated(const QString &arg1);

    void on_comboParity_activated(const QString &arg1);

    void on_pushButton_2_clicked();

private:
    Ui::OptionDialog *ui;
    Serial *serial;
};

#endif // OPTIONDIALOG_H
