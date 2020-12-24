#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include "optiondialog.h"
#include "serial.h"
#include "protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    OptionDialog *winOption;

protected:
    void timerEvent( QTimerEvent *event );

private slots:
    void onNewSerialResponse(uint8_t command, const char *payload, uint8_t s);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    Serial *serial;
    int m_serialTimerId;
};
#endif // MAINWINDOW_H