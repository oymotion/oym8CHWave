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

    GFListener::EMG_DATA_BITS getEMGDataBits();
    GFListener::EMG_DATA_RATE getEMGDataRate();

    GFListener::ACC_DATA_RATE getAccelDataRate();
    GFListener::GYRO_DATA_RATE getGyroDataRate();
    GFListener::MAG_DATA_RATE getMagneticDataRate();

    GFListener::QUAT_DATA_RATE getQuatDataRate();

private slots:
    void on_radioButton_12bit_clicked();

    void on_radioButton_650Hz_clicked();

    void on_radioButton_1000Hz_clicked();

    void on_groupBox_Quaternion_toggled(bool arg1);

    void on_groupBox_Accel_toggled(bool arg1);

    void on_groupBox_Gyro_toggled(bool arg1);

    void on_groupBox_Magnetic_toggled(bool arg1);

private:
    Ui::DialogDataSetting *ui;
};

#endif // DIALOGDATASETTING_H
