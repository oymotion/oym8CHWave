#include "mainwindow.h"
#include <QApplication>
#include <QtCore>


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  qRegisterMetaType<QVector<uint8_t>>("QVector<uint8_t>");

  // Create transaltor
  QTranslator trans;

  // Load language pack
  if (trans.load(QLocale::system().name() + ".qm"))
  {
    // install translator
    a.installTranslator(&trans);
  }

  MainWindow mainwindow;
  mainwindow.show();

  return a.exec();
}
