#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  qRegisterMetaType<QVector<uint8_t>>("QVector<uint8_t>");

  MainWindow mainwindow;
  mainwindow.show();

  return a.exec();
}
