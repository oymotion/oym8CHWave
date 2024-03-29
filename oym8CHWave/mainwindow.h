#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <functional>
#include <thread>
#include <mutex>
#include <QtSerialPort/QSerialPort>

#include "CircleBuff.h"
#include "qcustomplot/qcustomplot.h"

#include "gflistener.h"
#include "gfhubthread.h"


namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
//  virtual void closeEvent(QCloseEvent * event);

private:
  void sendString(const QString& str);  // Use this to avoid race condition of serial port.

  uint8_t doFilter(int channeldata, int CHNum); // Check if need to play sound for channel, returns 0 or 1

signals:

  void sig_sendString(const QString &str);

private slots:
  void ch_WindowInit();

  void handleReadyRead();

  void handleHubReady();
  void handleDeviceConnected();
  void handleDeviceDisconnected();
  void handleEmgSettingFailed();

  void on_actionExit_triggered();
  void on_actionConnectTogForce_triggered();
  void on_actionConnectToSounPlayer_triggered();

  void on_drawLine(QVector<uint8_t> rawData);

  void on_pushButtonRecord_clicked();

protected:
  friend class DialogConnect;
  QSerialPort serialPort;

private:
  Ui::MainWindow *ui;

  bool hubReady;
  std::shared_ptr<Hub> mHub;                // Create a instance of hub.
  std::shared_ptr<GFHubThread> gfHubThread; // To tick Hub

  std::shared_ptr<GFListener> gflistener;

  static const int CHNUM = 8;
  const int THRESHOLD_MIN = 4;
  const int THRESHOLD_MAX = 80;
  const int X_RANGE = 100;
  const int Y_RANGE = 250;
  const int SHOW_MESSAGE_TIME = 5000;

  bool deviceConnected;
  QString recordingEMGFileName;
  QString recordingCombinedFileName;

  QCustomPlot *plots[CHNUM];

  QVector<double> x;
  QVector<double> ch_data[CHNUM];

  uint32_t count;

  CIRCLE_BUFFER<int> chBufRaw[CHNUM];
  CIRCLE_BUFFER<int> chBufRawShort[CHNUM];
};

#endif // MAINWINDOW_H
