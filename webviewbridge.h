#ifndef WEBVIEWBRIDGE_H
#define WEBVIEWBRIDGE_H

#include <QObject>
#include "ui_mainwindow.h"

class WebviewBridge : public QObject
{
    Q_OBJECT
public:
    explicit WebviewBridge(Ui::MainWindow *ui, QObject *parent = nullptr);

public slots:
    void addMarker(QString id, qreal lng, qreal lat);
    void setDistance(qreal distance);
    void removeMarker(QString id);

public:
    void removeMarkerFromApp(QString id, int index);
    void clearMarker();

signals:
    void onMarkerRemoved(QString, int);
    void onMarkerClear();

private:
    Ui::MainWindow *ui;

};

#endif // WEBVIEWBRIDGE_H
