#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include "protocol.h"
#include "webview_bridge.h"
#include "optiondialog.h"
#include "connector_bridge.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void timerEvent( QTimerEvent *event );

private slots:
    void onNewSerialResponse(uint8_t command, const char *payload, uint8_t s);
    void on_btnConfig_clicked();
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_actionDashboard_triggered();
    void on_actionPlanner_triggered();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_chkAltHold_stateChanged(int arg1);
    void on_horizontalSlider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    QLabel *lblStatus;
    ConnectorBridge *bridge;

    int m_serialTimerId;
    WebviewBridge *webviewBridge;
    OptionDialog *winOption;
};
#endif // MAINWINDOW_H
