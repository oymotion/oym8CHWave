#ifndef DIALOGDEVICE_H
#define DIALOGDEVICE_H

#include <QDialog>

#include "gflistener.h"


namespace Ui {
class DialogDevice;
}

class DialogDevice : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDevice(std::shared_ptr<gf::Hub> &hub, std::shared_ptr<gfListener> &listener, QWidget *parent = nullptr);
    ~DialogDevice();

    int exec();
    QString getSelectedDevice();

private slots:
    void handleDeviceFound(QString devName, unsigned int rssi);
    void handleScanFinished();

    void on_pushButtonScan_clicked();

private:
    Ui::DialogDevice *ui;

    std::shared_ptr<gf::Hub> hub;
    std::shared_ptr<gfListener> listener;

    int scanTurnsLeft;
};

#endif // DIALOGDEVICE_H
