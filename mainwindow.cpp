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

    serial->SendCommand(MSP_ATTITUDE);
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

void MainWindow::onWebviewLoadFinished(bool f)
{
//    QString cmd = QString("addMarker(%1, %2)").arg(QString::number(50.f, 'f', 6)).arg(QString::number(50.f, 'f', 6));
//    ui->webView->page()->runJavaScript(cmd);
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
