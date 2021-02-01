#include <QtWebEngineWidgets>
#include <QWebChannel>
#include "main_window.h"
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

    QUrl url(QString("qrc://resources/webview/index.html"));
    ui->webView->load(url);

    QWebChannel *webviewChan = new QWebChannel(this);
    webviewBridge = new WebviewBridge(ui, this);
    webviewChan->registerObject("bridge", webviewBridge);
    ui->webView->page()->setWebChannel(webviewChan);

    serial = Serial::instance();
    ble = BLE::instance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent( QTimerEvent *event )
{
    QMainWindow::timerEvent( event );
    if (winOption->GetProtocolType() == SERIAL)
    {
        serial->SendCommand(MSP_RAW_IMU);
        serial->SendCommand(MSP_RAW_BARO);
        serial->SendCommand(MSP_STATUS);
        serial->SendCommand(MSP_ATTITUDE);
        serial->SendCommand(MSP_ALTITUDE);
        serial->SendCommand(MSP_MOTOR);
    }
    else
    {
        ble->SendCommand(MSP_RAW_IMU);
        ble->SendCommand(MSP_RAW_BARO);
        ble->SendCommand(MSP_STATUS);
        ble->SendCommand(MSP_ATTITUDE);
        ble->SendCommand(MSP_ALTITUDE);
        ble->SendCommand(MSP_MOTOR);
    }

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

        ui->lblRoll->setText(QString::number(att.AngleX / 10.f));
        ui->lblPitch->setText(QString::number(att.AngleY / 10.f));
        ui->lblYaw->setText(QString::number(att.Heading));
        break;

    case MSP_ALTITUDE:
        AltitudeResponse alt;
        alt.Unmarshel(payload, s);
        ui->lblAlt->setText(QString::number(alt.Alt / 100.f));
        ui->lblVario->setText(QString::number(alt.Vario));
        break;

    case MSP_MOTOR:
        MotorResponse motors;
        motors.Unmarshel(payload, s);
        ui->motor1->setCurrentValue(motors.Motors[0]);
        ui->motor2->setCurrentValue(motors.Motors[1]);
        ui->motor3->setCurrentValue(motors.Motors[2]);
        ui->motor4->setCurrentValue(motors.Motors[3]);
        break;

    case MSP_ALT_HOLD:
        int32_t altHold = UnmarshelAltHold(payload);
        ui->chkAltHold->setText(ui->chkAltHold->text().append(QString("[ %1cm ]").arg(altHold)));
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

void MainWindow::on_btnConnect_clicked()
{
    if (winOption->GetProtocolType() == SERIAL)
    {
        if (serial->Connect())
        {
            ui->statusbar->showMessage("连接成功", 2000);
            connect(serial, SIGNAL(newResponse(uint8_t, const char*, uint8_t)), this, SLOT(onNewSerialResponse(uint8_t, const char*, uint8_t)));
        }
        else
        {
            ui->statusbar->showMessage("连接失败，请检查端口是否被占用", 2000);
        }
    }
    else if (winOption->GetProtocolType() == BLE4)
    {
        if (ble->Connect()) {
            ui->statusbar->showMessage("连接成功", 2000);
            connect(ble, SIGNAL(newResponse(uint8_t, const char*, uint8_t)), this, SLOT(onNewSerialResponse(uint8_t, const char*, uint8_t)));
        } else {
            ui->statusbar->showMessage("蓝牙连接失败，未选择蓝牙设备", 2000);
        }
    }
    m_serialTimerId = startTimer(50);
}

void MainWindow::on_btnDisconnect_clicked()
{
    if (winOption->GetProtocolType() == SERIAL)
    {
        serial->Disconnect();
        ui->statusbar->showMessage("已断开连接", 2000);
        disconnect(serial, SIGNAL(newResponse(uint8_t, const char*, uint8_t)), this, SLOT(onNewSerialResponse(uint8_t, const char*, uint8_t)));
    }
    else
    {
        ble->Disconnect();
        disconnect(ble, SIGNAL(newResponse(uint8_t, const char*, uint8_t)), this, SLOT(onNewSerialResponse(uint8_t, const char*, uint8_t)));
    }
    ui->statusbar->showMessage("已断开连接", 2000);
    if (m_serialTimerId)
    {
        killTimer(m_serialTimerId);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if (winOption->GetProtocolType() == SERIAL)
    {
        serial->SendCommand(MSP_ACC_CALIBRATION);
        serial->SendCommand(MSP_MAG_CALIBRATION);
    }
    else
    {
        ble->SendCommand(MSP_ACC_CALIBRATION);
        ble->SendCommand(MSP_MAG_CALIBRATION);
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if (winOption->GetProtocolType() == SERIAL)
    {
        serial->SendCommand(MSP_ARM);
    }
    else
    {
        ble->SendCommand(MSP_ARM);
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    if (winOption->GetProtocolType() == SERIAL)
    {
        serial->SendCommand(MSP_DIS_ARM);
    }
    else
    {
        ble->SendCommand(MSP_DIS_ARM);
    }
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

void MainWindow::on_chkAltHold_stateChanged(int arg1)
{
    if (arg1)
    {
        serial->SendCommand(MSP_ALT_HOLD);
    }
    else
    {
        ui->chkAltHold->setText("AltHold");
        serial->SendCommand(MSP_ALT_UNLOCK);
    }
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    uint16_t p = position;
    qDebug("MSP_TEST_ALTHOLD alt: %d", p);
    serial->SendCommand(MSP_TEST_ALTHOLD, (char *)&p, 2);
}
