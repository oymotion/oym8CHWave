#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <functional>
#include <thread>
#include <mutex>
#include <QtSerialPort/QSerialPort>
#include "CircleBuff.h"

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

  void on_actionStop_triggered();

  void on_actionConnect_triggered();

  void on_drawLine(QVector<double> rawData);

protected:
  friend class DialogConnect;
  QSerialPort serialPort;

private:
  Ui::MainWindow *ui;

  std::map<QWidget*, QString> commands;
  std::thread queryThread;
  std::mutex commandsMutex;

  QVector<double> x;
  QVector<double> ch_data[8];

  char  sendData = 0x0;
  uint32_t count;

  const int THRESHOLD = 120;
  const int X_RANGE = 100;
  const int Y_RANGE = 250;
  const int SHOW_MESSAGE_TIME = 5000;

  CIRCLE_BUFFER<int> chBufRaw[8];
  CIRCLE_BUFFER<int> chBufRawLast[8];

  void queryParams(); // Thread function
};

#endif // MAINWINDOW_H
