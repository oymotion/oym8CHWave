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

gfListener::DATA_BITS DialogDataSetting::getDataBits()
{
    if (ui->radioButton_12bit->isChecked())
        return gfListener::DATA_BITS_12;
    else /*if (ui->radioButton_8bit->isChecked())*/
        return gfListener::DATA_BITS_8;
}

gfListener::DATA_RATE DialogDataSetting::getDataRate()
{
    if (ui->radioButton_1000Hz->isChecked())
        return gfListener::DATA_RATE_1000;
    else if (ui->radioButton_650Hz->isChecked())
        return gfListener::DATA_RATE_650;
    else if (ui->radioButton_500Hz->isChecked())
        return gfListener::DATA_RATE_500;
    else if (ui->radioButton_400Hz->isChecked())
        return gfListener::DATA_RATE_400;
    else if (ui->radioButton_250Hz->isChecked())
        return gfListener::DATA_RATE_250;
    else if (ui->radioButton_200Hz->isChecked())
        return gfListener::DATA_RATE_200;
    else /*if (ui->radioButton_100Hz->isChecked())*/
        return gfListener::DATA_RATE_100;
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
    ui->radioButton_8bit->setChecked(true);
}

void DialogDataSetting::on_radioButton_1000Hz_clicked()
{
    ui->radioButton_8bit->setChecked(true);
}
