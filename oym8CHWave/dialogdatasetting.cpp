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

GFListener::EMG_DATA_BITS DialogDataSetting::getEMGDataBits()
{
    if (ui->radioButton_12bit->isChecked())
        return GFListener::EMG_DATA_BITS_12;
    else /*if (ui->radioButton_8bit->isChecked())*/
        return GFListener::EMG_DATA_BITS_8;
}

GFListener::EMG_DATA_RATE DialogDataSetting::getEMGDataRate()
{
    if (!ui->groupBox_EMG->isChecked())
        return GFListener::EMG_DATA_RATE_DISABLED;

    if (ui->radioButton_1000Hz->isChecked())
        return GFListener::EMG_DATA_RATE_1000;
    else if (ui->radioButton_650Hz->isChecked())
        return GFListener::EMG_DATA_RATE_650;
    else if (ui->radioButton_500Hz->isChecked())
        return GFListener::EMG_DATA_RATE_500;
    else if (ui->radioButton_400Hz->isChecked())
        return GFListener::EMG_DATA_RATE_400;
    else if (ui->radioButton_250Hz->isChecked())
        return GFListener::EMG_DATA_RATE_250;
    else if (ui->radioButton_200Hz->isChecked())
        return GFListener::EMG_DATA_RATE_200;
    else /*if (ui->radioButton_100Hz->isChecked())*/
        return GFListener::EMG_DATA_RATE_100;
}


GFListener::ACC_DATA_RATE DialogDataSetting::getAccelDataRate()
{
    if (!ui->groupBox_Accel->isChecked())
        return GFListener::ACC_DATA_RATE_DISABLED;

    if (ui->radioButton_Accel50HZ->isChecked())
        return GFListener::ACC_DATA_RATE_50;
    else if (ui->radioButton_Accel100HZ->isChecked())
        return GFListener::ACC_DATA_RATE_100;
    else
        return GFListener::ACC_DATA_RATE_DISABLED;
}


GFListener::GYRO_DATA_RATE DialogDataSetting::getGyroDataRate()
{
    if (!ui->groupBox_Gyro->isChecked())
        return GFListener::GYRO_DATA_RATE_DISABLED;

    if (ui->radioButton_Gyro50HZ->isChecked())
        return GFListener::GYRO_DATA_RATE_50;
    else if (ui->radioButton_Gyro100HZ->isChecked())
        return GFListener::GYRO_DATA_RATE_100;
    else
        return GFListener::GYRO_DATA_RATE_DISABLED;
}


GFListener::MAG_DATA_RATE DialogDataSetting::getMagneticDataRate()
{
    if (!ui->groupBox_Magnetic->isChecked())
        return GFListener::MAG_DATA_RATE_DISABLED;

    if (ui->radioButton_Magnetic10HZ->isChecked())
        return GFListener::MAG_DATA_RATE_10;
    else
        return GFListener::MAG_DATA_RATE_DISABLED;
}


GFListener::QUAT_DATA_RATE DialogDataSetting::getQuatDataRate()
{
    if (!ui->groupBox_Quaternion->isChecked())
        return GFListener::QUAT_DATA_RATE_DISABLED;

    if (ui->radioButton_Quaternion50HZ->isChecked())
        return GFListener::QUAT_DATA_RATE_50;
    else if (ui->radioButton_Quaternion100HZ->isChecked())
        return GFListener::QUAT_DATA_RATE_100;
    else
        return GFListener::QUAT_DATA_RATE_DISABLED;
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


void DialogDataSetting::on_groupBox_Quaternion_toggled(bool checked)
{
    if (checked)
    {
        ui->groupBox_Accel->setChecked(false);
        ui->groupBox_Gyro->setChecked(false);
        ui->groupBox_Magnetic->setChecked(false);
    }
}


void DialogDataSetting::on_groupBox_Accel_toggled(bool checked)
{
    if (checked)
    {
        ui->groupBox_Quaternion->setChecked(false);
    }
}


void DialogDataSetting::on_groupBox_Gyro_toggled(bool checked)
{
    if (checked)
    {
        ui->groupBox_Quaternion->setChecked(false);
    }
}


void DialogDataSetting::on_groupBox_Magnetic_toggled(bool checked)
{
    if (checked)
    {
        ui->groupBox_Quaternion->setChecked(false);
    }
}


