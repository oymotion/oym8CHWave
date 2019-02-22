#include "mainwindow.h"
#include <QApplication>

#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <qdebug.h>

#include "gfsdkqml.h"
#include "GfThread/gfthread.h"

std::promise<bool> isPHubOK;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);


  MainWindow mainwindow;
  mainwindow.show();

  tstring identifer(_T("FetchGForceHandle sample app"));
  auto fut = isPHubOK.get_future();
  auto pHub = HubManager::getHubInstance(identifer);
  auto gflistener = make_shared<gfSdkQml>(pHub, std::ref(mainwindow));

  QObject::connect(gflistener.get(), SIGNAL(sendDeviceData(QVector<double>)),
                   &mainwindow, SLOT(on_drawLine(QVector<double>)));

  GfThread gfThread(gflistener, pHub);
  gfThread.start();



  fut.wait();

  return a.exec();
}
