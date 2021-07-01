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
    void keyPressEvent(QKeyEvent *evt);
    void keyReleaseEvent(QKeyEvent *evt);

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

    void on_actionTunning_triggered();

    void on_btnReadConfig_clicked();

    void on_tblPID_cellChanged(int row, int column);

    void on_btnWriteConfig_clicked();

    void on_pitchSlider_sliderMoved(int position);

    void on_pitchSlider_sliderReleased();

    void on_throttleSlider_sliderMoved(int position);

    void on_rollSlider_sliderMoved(int position);

    void on_rollSlider_sliderReleased();

    void on_yawSlider_sliderMoved(int position);

    void on_yawSlider_sliderReleased();

private:
    Ui::MainWindow *ui;
    QLabel *lblStatus;
    ConnectorBridge *bridge;

    int m_serialTimerId;
    WebviewBridge *webviewBridge;
    OptionDialog *winOption;

    int16_t rcCommand[4] = {0, 0, 0, 1150};
    bool keyWPressed = false, keySPressed = false, keyAPressed = false, keyDPressed = false, keyUpPressed = false, keyDownPressed = false, keyLeftPressed = false, keyRightPressed = false;

    enum CellType {
        ctP = 1000,
        ctI,
        ctD
    };

    PIDResponse pidConfig;
};
#endif // MAINWINDOW_H
