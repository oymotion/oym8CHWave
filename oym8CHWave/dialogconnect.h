#ifndef DIALOGCONNECT_H
#define DIALOGCONNECT_H

#include <QDialog>

namespace Ui {
  class DialogConnect;
}

class DialogConnect : public QDialog
{
  Q_OBJECT

public:
  explicit DialogConnect(QWidget *parent = nullptr);
  ~DialogConnect();

  void show();

private slots:
  void on_buttonBox_accepted();

private:
  Ui::DialogConnect *ui;
};

#endif // DIALOGCONNECT_H
