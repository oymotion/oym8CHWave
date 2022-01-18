#include "dialogdatasetting.h"
#include "ui_dialogdatasetting.h"

DialogDataSetting::DialogDataSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDataSetting)
{
    ui->setupUi(this);
}

DialogDataSetting::~DialogDataSetting()
{
    delete ui;
}

gfListener::EMG_DATA_BITS DialogDataSetting::getEMGDataBits()
{
    if (ui->radioButton_12bit->isChecked())
        return gfListener::EMG_DATA_BITS_12;
    else /*if (ui->radioButton_8bit->isChecked())*/
        return gfListener::EMG_DATA_BITS_8;
}

gfListener::EMG_DATA_RATE DialogDataSetting::getEMGDataRate()
{
    if (!ui->groupBox_EMG->isChecked())
        return gfListener::EMG_DATA_RATE_DISABLED;

    if (ui->radioButton_1000Hz->isChecked())
        return gfListener::EMG_DATA_RATE_1000;
    else if (ui->radioButton_650Hz->isChecked())
        return gfListener::EMG_DATA_RATE_650;
    else if (ui->radioButton_500Hz->isChecked())
        return gfListener::EMG_DATA_RATE_500;
    else if (ui->radioButton_400Hz->isChecked())
        return gfListener::EMG_DATA_RATE_400;
    else if (ui->radioButton_250Hz->isChecked())
        return gfListener::EMG_DATA_RATE_250;
    else if (ui->radioButton_200Hz->isChecked())
        return gfListener::EMG_DATA_RATE_200;
    else /*if (ui->radioButton_100Hz->isChecked())*/
        return gfListener::EMG_DATA_RATE_100;
}


gfListener::ACC_DATA_RATE DialogDataSetting::getAccelDataRate()
{
    if (!ui->groupBox_Accel->isChecked())
        return gfListener::ACC_DATA_RATE_DISABLED;

    if (ui->radioButton_Accel100HZ->isChecked())
        return gfListener::ACC_DATA_RATE_100;
    else
        return gfListener::ACC_DATA_RATE_DISABLED;
}


gfListener::GYRO_DATA_RATE DialogDataSetting::getGyroDataRate()
{
    if (!ui->groupBox_Gyro->isChecked())
        return gfListener::GYRO_DATA_RATE_DISABLED;

    if (ui->radioButton_Gyro100HZ->isChecked())
        return gfListener::GYRO_DATA_RATE_100;
    else
        return gfListener::GYRO_DATA_RATE_DISABLED;
}


gfListener::MAG_DATA_RATE DialogDataSetting::getMagneticDataRate()
{
    if (!ui->groupBox_Magnetic->isChecked())
        return gfListener::MAG_DATA_RATE_DISABLED;

    if (ui->radioButton_Magnetic10HZ->isChecked())
        return gfListener::MAG_DATA_RATE_10;
    else
        return gfListener::MAG_DATA_RATE_DISABLED;
}


gfListener::QUAT_DATA_RATE DialogDataSetting::getQuatDataRate()
{
    if (!ui->groupBox_Quaternion->isChecked())
        return gfListener::QUAT_DATA_RATE_DISABLED;

    if (ui->radioButton_Quaternion100HZ->isChecked())
        return gfListener::QUAT_DATA_RATE_100;
    else
        return gfListener::QUAT_DATA_RATE_DISABLED;
}


void DialogDataSetting::on_radioButton_12bit_clicked()
{
    if (ui->radioButton_1000Hz->isChecked()|| ui->radioButton_650Hz->isChecked())
    {
        ui->radioButton_500Hz->setChecked(true);
    }
}

void DialogDataSetting::on_radioButton_650Hz_clicked()
{
    // Only 8 bit mode allowed for 650HZ+
    ui->radioButton_8bit->setChecked(true);
}

void DialogDataSetting::on_radioButton_1000Hz_clicked()
{
    // Only 8 bit mode allowed for 650HZ+
    ui->radioButton_8bit->setChecked(true);
}

