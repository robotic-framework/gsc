#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include "option_dialog.h"
#include "serial.h"
#include "protocol.h"
#include "webview_bridge.h"

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
    Serial *serial;
    BLE *ble;
    int m_serialTimerId;
    WebviewBridge *webviewBridge;
};
#endif // MAIN_WINDOW_H
