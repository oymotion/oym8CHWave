#ifndef DIALOGDATASETTING_H
#define DIALOGDATASETTING_H

#include <QDialog>

#include "gflistener.h"


namespace Ui {
class DialogDataSetting;
}

class DialogDataSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDataSetting(QWidget *parent = nullptr);
    ~DialogDataSetting();

    gfListener::EMG_DATA_BITS getEMGDataBits();
    gfListener::EMG_DATA_RATE getEMGDataRate();

    gfListener::ACC_DATA_RATE getAccelDataRate();
    gfListener::GYRO_DATA_RATE getGyroDataRate();
    gfListener::MAG_DATA_RATE getMagneticDataRate();

    gfListener::QUAT_DATA_RATE getQuatDataRate();

private slots:
    void on_radioButton_12bit_clicked();

    void on_radioButton_650Hz_clicked();

    void on_radioButton_1000Hz_clicked();

private:
    Ui::DialogDataSetting *ui;
};

#endif // DIALOGDATASETTING_H
