/*
   Copyright 2017, OYMotion Inc.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
   THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
   DAMAGE.

*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QTime>
#include <QtGlobal>
#include <qdebug.h>
#include "gfsdkqml.h"
#include "dialogconnect.h"
#include "qcustomplot/qcustomplot.h"


QT_USE_NAMESPACE

#define ARR_SIZE(a) (sizeof(a) / sizeof(a[0]))

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  deviceConnected(false)
{
  ui->setupUi(this);

  QFile f(":qdarkstyle/style.qss");  //显示图片

  if (!f.exists())
  {
      printf("Unable to set stylesheet, file not found\n");
  }
  else
  {
      f.open(QFile::ReadOnly | QFile::Text);
      QTextStream ts(&f);
      qApp->setStyleSheet(ts.readAll());
  }

  ch_WindowInit();   //波形显示界面初始化

  for (int i = 0; i < CHNUM; i++)  //初始化数据滤波存储数组
  {
    chBufRaw[i].begin(1024);
    chBufRawShort[i].begin(32);
  }

  ui->statusBar->showMessage(tr("Waiting for gForce connection..."));
}

MainWindow::~MainWindow()
{
  if (serialPort.isOpen())
  {
    serialPort.close();  //退出时关闭串口
  }

  delete ui;
}

void MainWindow::ch_WindowInit()
{
  //初始化控件数组
  plots[0] = ui->customPlot_CH1;
  plots[1] = ui->customPlot_CH2;
  plots[2] = ui->customPlot_CH3;
  plots[3] = ui->customPlot_CH4;
  plots[4] = ui->customPlot_CH5;
  plots[5] = ui->customPlot_CH6;
  plots[6] = ui->customPlot_CH7;
  plots[7] = ui->customPlot_CH8;


  for (int i = 0; i < ARR_SIZE(plots); i++)
  {
    plots[i]->addGraph();

    plots[i]->xAxis->setRange(0, X_RANGE);
    plots[i]->yAxis->setRange(0, Y_RANGE);

    plots[i]->setBackground(QBrush(QColor(64, 64, 64, 100)));

    plots[i]->xAxis->setBasePen(QPen(Qt::gray));
    plots[i]->xAxis->setTickPen(QPen(Qt::gray));
    plots[i]->xAxis->setSubTickPen(QPen(Qt::gray));
    plots[i]->xAxis->setTickLabelColor(Qt::gray);

    plots[i]->yAxis->setBasePen(QPen(Qt::gray));
    plots[i]->yAxis->setTickPen(QPen(Qt::gray));
    plots[i]->yAxis->setSubTickPen(QPen(Qt::gray));
    plots[i]->yAxis->setTickLabelColor(Qt::gray);

    plots[i]->graph(0)->setPen(QPen(Qt::green));
  }

  x.clear();
  count = 0;
}

void MainWindow::on_drawLine(QVector<uint8_t> rawData)
{
  if (rawData.size() == 128)
  {
    for (int i = 0; i < 16; i += 2)
    {
      x.push_back(count++);
      uint8_t sendData = 0;

      for (int n = 0; n < CHNUM; ++n)
      {
        //int raw = (rawData[i*CHNUM+n] + rawData[(i+1)*CHNUM+n]) / 2;
        int raw = rawData[i * CHNUM + n];
        ch_data[n].push_back(raw);

        uint8_t dataBack = filterProces(raw, n);
        sendData = static_cast<uint8_t>(sendData | dataBack << n);
      }  

      if (serialPort.isOpen())
      {
        serialPort.clear();   // Discard data in buffer
        serialPort.write((char*)&sendData, 1);
      }
    }

    while (x.length() > X_RANGE)
    {
      x.pop_front();

      for (int n = 0; n < CHNUM; ++n)
      {
        ch_data[n].pop_front();
      }
    }

    for (int i = 0; i < ARR_SIZE(plots); i++)
    {
      plots[i]->graph(0)->setData(x, ch_data[i]);

      auto range = plots[i]->yAxis->range();

      plots[i]->rescaleAxes();

      plots[i]->yAxis->setRange(range);

      plots[i]->replot();
    }
  }
}

void MainWindow::handleDeviceDisconnected()
{
  if (deviceConnected)
  {
    deviceConnected = false;
    QMessageBox::information(this, tr("Info"), tr("gForce disconnected."));
    ui->statusBar->showMessage(tr("gForce disconnected."));
  }
}

void MainWindow::handleDeviceConnected()
{
  deviceConnected = true;
  ui->statusBar->showMessage(tr("gForce connected."), SHOW_MESSAGE_TIME);
}

void MainWindow::handleReadyRead()
{
  //qDebug() << "serialPort.read: " << serialPort.read(1);
}

void MainWindow::on_actionExit_triggered()
{
  if (serialPort.isOpen())
  {
    serialPort.close();
  }

  this->close();
}

void MainWindow::on_actionConnect_triggered()
{
  if (serialPort.isOpen())
  {
    serialPort.close();
  }
  else
  {
    DialogConnect dialogConnect(this);
    dialogConnect.show();
  }

  if (serialPort.isOpen())
  {
    ui->actionConnect->setText(tr("Disconnect from speakers"));

    qDebug() <<"Connect Success";
  }
  else
  {
    ui->actionConnect->setText(tr("Connect to speakers"));
  }
}


uint8_t MainWindow::filterProces(int channelData, int CHNum)
{
  chBufRaw[CHNum].write(channelData);

  chBufRawShort[CHNum].write(abs(channelData - chBufRaw[CHNum].readMean()));

//  qDebug() <<"CHNum" << CHNum << chBufRaw[CHNum].readMean() << "chBufRawShort[CHNum].readMean()" << chBufRawShort[CHNum].readMean();

  if ((chBufRawShort[CHNum].readMean() > THRESHOLD_MIN) && (chBufRawShort[CHNum].readMean() < THRESHOLD_MAX))
  {
    return 0x01;
  }
  else
  {
    return 0x00;
  }
}


