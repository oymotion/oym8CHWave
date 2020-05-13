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

    gfListener::DATA_BITS getDataBits();
    gfListener::DATA_RATE getDataRate();

private:
    Ui::DialogDataSetting *ui;
};

#endif // DIALOGDATASETTING_H
