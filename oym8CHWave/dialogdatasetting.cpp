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
    if (ui->radioButton_650Hz->isChecked())
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
