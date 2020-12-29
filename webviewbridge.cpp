#include "webviewbridge.h"

WebviewBridge::WebviewBridge(Ui::MainWindow *ui, QObject *parent) : QObject(parent),
    ui(ui)
{

}

void WebviewBridge::addMarker(QString id, qreal lng, qreal lat)
{
    ui->listWidget->addItem(QString("%1: %2, %3").arg(id).arg(lng).arg(lat));
    qDebug("receive new marker %s, %f, %f", id.toStdString().c_str(), lng, lat);
}

void WebviewBridge::removeMarker(QString id)
{
    qDebug("remove marker %s", id.toStdString().c_str());
}

void WebviewBridge::removeMarkerFromApp(QString id, int index)
{
    emit onMarkerRemoved(id, index);
}

void WebviewBridge::clearMarker()
{
    emit onMarkerClear();
}
