#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      winOption(nullptr)
{
    ui->setupUi(this);
    ui->dashboardADI->verticalScrollBar()->setEnabled(false);
    ui->dashboardADI->horizontalScrollBar()->setEnabled(false);
    ui->dashboardPFD->verticalScrollBar()->setEnabled(false);
    ui->dashboardPFD->horizontalScrollBar()->setEnabled(false);
    ui->dashboardALT->verticalScrollBar()->setEnabled(false);
    ui->dashboardALT->horizontalScrollBar()->setEnabled(false);
    ui->dashboardHSI->verticalScrollBar()->setEnabled(false);
    ui->dashboardHSI->horizontalScrollBar()->setEnabled(false);
    ui->dashboardNAV->verticalScrollBar()->setEnabled(false);
    ui->dashboardNAV->horizontalScrollBar()->setEnabled(false);
    ui->dashboardVSI->verticalScrollBar()->setEnabled(false);
    ui->dashboardVSI->horizontalScrollBar()->setEnabled(false);
    serial = Serial::instance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent( QTimerEvent *event )
{
    QMainWindow::timerEvent( event );

    serial->SendCommand(MSP_ATTITUDE);
    ui->dashboardADI->update();
}

void MainWindow::onNewSerialResponse(uint8_t command, const char *payload, uint8_t s)
{
    switch (command)
    {
    case MSP_ATTITUDE:
        AttitudeResponse att;
        att.Unmarshel(payload, s);
        ui->dashboardADI->setRoll(att.AngleX / 10.f);
        ui->dashboardADI->setPitch(-att.AngleY / 10.f);
        qDebug("x: %d, y: %d, heading: %d", att.AngleX, att.AngleY, att.Heading);
        break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(winOption == nullptr)
    {
        winOption = new OptionDialog;
    }
    winOption->show();
}

void MainWindow::on_pushButton_2_clicked()
{
    if (serial->Connect())
    {
        ui->statusbar->showMessage("连接成功", 2000);
        connect(serial, SIGNAL(newResponse(uint8_t, const char*, uint8_t)), this, SLOT(onNewSerialResponse(uint8_t, const char*, uint8_t)));
        m_serialTimerId = startTimer(100);
    }
    else
    {
        ui->statusbar->showMessage("连接失败，请检查端口是否被占用", 2000);
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    serial->Disconnect();
    ui->statusbar->showMessage("已断开连接", 2000);
    disconnect(serial, SIGNAL(newResponse(uint8_t, const char*, uint8_t)), this, SLOT(onNewSerialResponse(uint8_t, const char*, uint8_t)));
    if (m_serialTimerId)
    {
        killTimer(m_serialTimerId);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    serial->SendCommand(MSP_ACC_CALIBRATION);
}

void MainWindow::on_pushButton_5_clicked()
{
    serial->SendCommand(MSP_ARM);
}

void MainWindow::on_pushButton_6_clicked()
{
    serial->SendCommand(MSP_DIS_ARM);
}
