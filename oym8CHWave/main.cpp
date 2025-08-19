#include "mainwindow.h"
#include <QApplication>
#include <QtCore>


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  qRegisterMetaType<QVector<uint8_t>>("QVector<uint8_t>");

  QVector<QTranslator *> translators;
  QString language = QLocale::system().name();
  QString transDir = QCoreApplication::applicationDirPath() + "/translations/";
  QDir directory(transDir);

  QStringList filters;
  filters << "*" + language + ".qm";

  auto files = directory.entryList(filters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

  for (const QString &transFile : files) {
      qDebug() << "language:" << language << ", translation:" << transDir + transFile;

      QTranslator *translator = new QTranslator(&a);

      if (translator->load(transDir + transFile)) {
          a.installTranslator(translator);
          translators.append(translator);
      } else {
          qWarning() << "Failed to load translation:" << transFile;
          delete translator;
      }
  }

  MainWindow mainwindow;
  mainwindow.show();

  // a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

  return a.exec();
}
