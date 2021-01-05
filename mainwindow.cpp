#include <QtWebEngineWidgets>
#include <QWebChannel>
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
    ui->lcdDistance->setStyleSheet("border: 0; color: red; font: bold;");

    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(onWebviewLoadFinished(bool)));
    QUrl url(QString("qrc://resources/webview/index.html"));
    ui->webView->load(url);

    QWebChannel *webviewChan = new QWebChannel(this);
    webviewBridge = new WebviewBridge(ui, this);
    webviewChan->registerObject("bridge", webviewBridge);
    ui->webView->page()->setWebChannel(webviewChan);

    serial = Serial::instance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent( QTimerEvent *event )
{
    QMainWindow::timerEvent( event );

    serial->SendCommand(MSP_RAW_IMU);
    serial->SendCommand(MSP_RAW_BARO);
    serial->SendCommand(MSP_STATUS);
    serial->SendCommand(MSP_ATTITUDE);
    serial->SendCommand(MSP_ALTITUDE);
    serial->SendCommand(MSP_MOTOR);

    ui->motor1->update();
    ui->motor2->update();
    ui->motor3->update();
    ui->motor4->update();
    ui->dashboardADI->update();
    ui->dashboardPFD->update();
    ui->dashboardHSI->update();
    ui->dashboardNAV->update();
}

void MainWindow::onNewSerialResponse(uint8_t command, const char *payload, uint8_t s)
{
    switch (command)
    {
    case MSP_RAW_IMU:
        RawIMUResponse imu;
        imu.Unmarshel(payload, s);
        ui->lblAccX->setText(QString::number(imu.Acc[0]));
        ui->lblAccY->setText(QString::number(imu.Acc[1]));
        ui->lblAccZ->setText(QString::number(imu.Acc[2]));

        ui->lblGyroX->setText(QString::number(imu.Gyro[0]));
        ui->lblGyroY->setText(QString::number(imu.Gyro[1]));
        ui->lblGyroZ->setText(QString::number(imu.Gyro[2]));

        ui->lblMagX->setText(QString::number(imu.Mag[0]));
        ui->lblMagY->setText(QString::number(imu.Mag[1]));
        ui->lblMagZ->setText(QString::number(imu.Mag[2]));
        break;

    case MSP_RAW_BARO:
        RawBaroResponse baro;
        baro.Unmarshel(payload, s);
        ui->lblBaroT->setText(QString::number(baro.CT / 100.f));
        ui->lblBaroP->setText(QString::number(baro.CP / 100.f));
        break;

    case MSP_STATUS:
        StatusResponse status;
        status.Unmarshel(payload, s);
        ui->lblCycleTime->setText(QString::number(status.CycleTime));
        ui->lblI2CError->setText(QString::number(status.I2CErrorCount));
        ui->swiAcc->setChecked(status.SensorAcc);
        ui->swiGyro->setChecked(status.SensorGyro);
        ui->swiMag->setChecked(status.SensorMag);
        ui->swiBaro->setChecked(status.SensorBaro);
        ui->swiSonar->setChecked(status.SensorSonar);
        ui->swiGPS->setChecked(status.SensorGPS);
        break;

    case MSP_ATTITUDE:
        AttitudeResponse att;
        att.Unmarshel(payload, s);
        ui->dashboardADI->setRoll(att.AngleX / 10.f);
        ui->dashboardADI->setPitch(-att.AngleY / 10.f);

        ui->dashboardPFD->setRoll(att.AngleX / 10.f);
        ui->dashboardPFD->setPitch(-att.AngleY / 10.f);
        ui->dashboardPFD->setHeading(att.Heading);

        ui->dashboardHSI->setHeading(att.Heading);
        ui->dashboardNAV->setHeading(att.Heading);
        break;

    case MSP_ALTITUDE:
        AltitudeResponse alt;
        alt.Unmarshel(payload, s);
        ui->lblAlt->setText(QString::number(alt.Alt / 100.f));
        break;

    case MSP_MOTOR:
        MotorResponse motors;
        motors.Unmarshel(payload, s);
        ui->motor1->setCurrentValue(motors.Motors[0]);
        ui->motor2->setCurrentValue(motors.Motors[1]);
        ui->motor3->setCurrentValue(motors.Motors[2]);
        ui->motor4->setCurrentValue(motors.Motors[3]);
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
        m_serialTimerId = startTimer(50);
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

void MainWindow::on_actionDashboard_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_actionPlanner_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_7_clicked()
{
    QStringList tmp = ui->listWidget->currentItem()->text().split(":");
    QString id = tmp[0];
    webviewBridge->removeMarkerFromApp(id, ui->listWidget->currentRow());
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

void MainWindow::on_pushButton_8_clicked()
{
    ui->listWidget->clear();
    webviewBridge->clearMarker();
}
