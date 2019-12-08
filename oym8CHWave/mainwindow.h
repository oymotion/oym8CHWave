#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <functional>
#include <thread>
#include <mutex>
#include <QtSerialPort/QSerialPort>
#include "CircleBuff.h"
#include "qcustomplot/qcustomplot.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  void sendString(const QString& str);  // Use this to avoid race condition of serial port.

  uint8_t filterProces(int channeldata, int CHNum);

signals:

  void sig_sendString(const QString &str);

private slots:
  void ch_WindowInit();

  void handleReadyRead();

  void handleDeviceDisconnected();
  void handleDeviceConnected();

  void on_actionExit_triggered();

  void on_actionConnect_triggered();

  void on_drawLine(QVector<uint8_t> rawData);


protected:
  friend class DialogConnect;
  QSerialPort serialPort;

private:
  Ui::MainWindow *ui;

  static const int CHNUM = 8;
  const int THRESHOLD_MIN = 4;
  const int THRESHOLD_MAX = 80;
  const int X_RANGE = 100;
  const int Y_RANGE = 250;
  const int SHOW_MESSAGE_TIME = 5000;

  bool deviceConnected;

  QCustomPlot *plots[CHNUM];

  QVector<double> x;
  QVector<double> ch_data[CHNUM];

  uint32_t count;

  CIRCLE_BUFFER<int> chBufRaw[CHNUM];
  CIRCLE_BUFFER<int> chBufRawShort[CHNUM];

};

#endif // MAINWINDOW_H
