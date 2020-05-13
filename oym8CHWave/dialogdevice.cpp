#include "dialogdevice.h"
#include "ui_dialogdevice.h"

#include <QDebug>

#include "dialogdatasetting.h"



DialogDevice::DialogDevice(std::shared_ptr<gf::Hub> &hub, std::shared_ptr<gfListener> &listener, QWidget *parent) :
    QDialog(parent),
    listener(listener),
    hub(hub),
    ui(new Ui::DialogDevice)
{
    ui->setupUi(this);

    QObject::connect(listener.get(), SIGNAL(deviceFound(QString, unsigned int)),
                     this, SLOT(handleDeviceFound(QString, unsigned int)));

    QObject::connect(listener.get(), SIGNAL(scanFinished()),
                     this, SLOT(handleScanFinished()));
}

DialogDevice::~DialogDevice()
{
    QObject::disconnect(listener.get(), SIGNAL(deviceFound(QString, unsigned int)),
                     this, SLOT(handleDeviceFound(QString, unsigned int)));

    QObject::disconnect(listener.get(), SIGNAL(scanFinished()),
                     this, SLOT(handleScanFinished()));

    delete ui;
}


int DialogDevice::exec()
{
    auto ret = QDialog::exec();

    hub->stopScan();

    if (ret == QDialog::Accepted && getSelectedDevice() != "")
    {
        qDebug() << "dlgDevice.getSelectedDevice(): " << getSelectedDevice();

        DialogDataSetting dlgDataSetting;

        if (dlgDataSetting.exec() == QDialog::Accepted)
        {
            listener->connectDevice(getSelectedDevice(), dlgDataSetting.getDataBits(), dlgDataSetting.getDataRate());
        }
    }

    return ret;
}


QString DialogDevice::getSelectedDevice()
{
    QString item;

    if (ui->listWidgetDevices->currentItem())
        item  = ui->listWidgetDevices->currentItem()->text().split(",").at(0);

    return item;
}


void DialogDevice::handleDeviceFound(QString devName, unsigned int rssi)
{
    qDebug() << __FUNCTION__ << "called.";
    qDebug() << devName << rssi;

    ui->listWidgetDevices->addItem(devName + ", rssi=" + QString::number(rssi));
}


void DialogDevice::handleScanFinished()
{
    qDebug() << __FUNCTION__ << "called.";

    ui->pushButtonScan->setEnabled(true);
}

void DialogDevice::on_pushButtonScan_clicked()
{
    qDebug() << __FUNCTION__ << "called.";

    ui->pushButtonScan->setEnabled(false);
    ui->listWidgetDevices->clear();
    hub->startScan(0);
}
