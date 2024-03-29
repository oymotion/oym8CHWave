﻿/*
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
#include <iostream>
#include <thread>
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <qdebug.h>

#include "qcustomplot/qcustomplot.h"
#include "dialogconnect.h"
#include "dialogdevice.h"


QT_USE_NAMESPACE

#define ARR_SIZE(a) (sizeof(a) / sizeof(a[0]))

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  hubReady(false),
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

  ui->pushButtonRecord->setText(tr("Start Recording"));

  ch_WindowInit();   //波形显示界面初始化

  for (int i = 0; i < CHNUM; i++)  //初始化数据滤波存储数组
  {
    chBufRaw[i].begin(1024);
    chBufRawShort[i].begin(32);
  }

  ui->statusBar->showMessage(tr("Waiting for gForce connection..."));


  mHub = HubManager::getHubInstance(_T("gForce"));

  gfHubThread = std::make_shared<GFHubThread>(mHub);
  QObject::connect(gfHubThread.get(), SIGNAL(hubReady()),
                   this, SLOT(handleHubReady()));
  gfHubThread->start();


  gflistener = std::make_shared<GFListener>(mHub, this);

  QObject::connect(gflistener.get(), SIGNAL(sendDeviceData(QVector<uint8_t>)),
                   this, SLOT(on_drawLine(QVector<uint8_t>)));

  QObject::connect(gflistener.get(), SIGNAL(deviceConnected()),
                   this, SLOT(handleDeviceConnected()));

  QObject::connect(gflistener.get(), SIGNAL(deviceDisConnected()),
                   this, SLOT(handleDeviceDisconnected()));

  QObject::connect(gflistener.get(), SIGNAL(emgSettingFailed()),
                   this, SLOT(handleEmgSettingFailed()));

  GF_RET_CODE retCode = GF_RET_CODE::GF_SUCCESS;
  cout << __FUNCTION__ << " has been called." << endl;

  retCode = mHub->registerListener(gflistener);
  cout << "registerListener " << ((retCode == GF_RET_CODE::GF_SUCCESS) ? "SUCCESS" : "FAIL") << endl;
}


MainWindow::~MainWindow()
{
  if (serialPort.isOpen())
  {
    serialPort.close();  //退出时关闭串口
  }

  // release phub resource
  mHub->unRegisterListener(gflistener);

  delete ui;
}


//void MainWindow::closeEvent(QCloseEvent * event)
//{
//    if (QMessageBox::information(this, tr("Info"), tr("Close?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
//        event->accept();
//    else
//        event->ignore();
//}


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
  if (rawData.size() == 128/*8bit ADC*/ || rawData.size() == 64/*12bit ADC*/)
  {
    for (int i = 0; i < rawData.size() / 8; i++)
    {
      x.push_back(count++);
      uint8_t sendData = 0; // Bit flags to indicate if play sound for channels

      for (int n = 0; n < CHNUM; ++n)
      {
        //int raw = (rawData[i*CHNUM+n] + rawData[(i+1)*CHNUM+n]) / 2;
        int raw = rawData[i * CHNUM + n];
        ch_data[n].push_back(raw);

        if (serialPort.isOpen())
        {
          uint8_t dataBack = doFilter(raw, n);
          sendData = static_cast<uint8_t>(sendData | dataBack << n);
        }
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


void MainWindow::handleHubReady()
{
  hubReady = true;
}


void MainWindow::handleDeviceConnected()
{
  deviceConnected = true;
  ui->statusBar->showMessage(tr("gForce connected."), SHOW_MESSAGE_TIME);

  ui->pushButtonRecord->setEnabled(true);
}


void MainWindow::handleDeviceDisconnected()
{
  if (deviceConnected)
  {
    deviceConnected = false;
    QMessageBox::information(this, tr("Info"), tr("gForce disconnected."));
    ui->statusBar->showMessage(tr("gForce disconnected."));

    if (recordingEMGFileName.isEmpty() && recordingCombinedFileName.isEmpty())
    {
        // Not recording, can disable it now
        ui->pushButtonRecord->setEnabled(false);
    }
    else
    {
        // Save file?
    }
  }
}


void MainWindow::handleEmgSettingFailed()
{
    QMessageBox::critical(this, tr("Error"), tr("EMG setting failed!\nPlease disconnect then connect again."));
    ui->statusBar->showMessage(tr("EMG setting failed!"));
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


void MainWindow::on_actionConnectTogForce_triggered()
{
    if (!hubReady)
    {
        QMessageBox::question(this, tr("gForce Hub not ready"), tr("Did you plug in gForce dongle?"), QMessageBox::Ok);
        return;
    }

    if (gflistener->getDeviceStatus())
    {
        if (QMessageBox::question(this, tr("Disconnect?"), tr("gForce is connected, disconnected first?")) != QMessageBox::Yes)
            return;

        gflistener->disconnectDevice();
    }

    DialogDevice dlgDevice(mHub, gflistener);
    dlgDevice.exec();
}


void MainWindow::on_actionConnectToSounPlayer_triggered()
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
    ui->actionConnectToSounPlayer->setText(tr("Disconnect from Sound Player"));

    qDebug() <<"Connect Success";
  }
  else
  {
    ui->actionConnectToSounPlayer->setText(tr("Connect to Sound Player"));
  }
}


uint8_t MainWindow::doFilter(int channelData, int CHNum)
{
  chBufRaw[CHNum].write(channelData);

  chBufRawShort[CHNum].write(abs(channelData - chBufRaw[CHNum].readMean()));

  // qDebug() <<"CHNum" << CHNum << chBufRaw[CHNum].readMean() << "chBufRawShort[CHNum].readMean()" << chBufRawShort[CHNum].readMean();

  if ((chBufRawShort[CHNum].readMean() > THRESHOLD_MIN) && (chBufRawShort[CHNum].readMean() < THRESHOLD_MAX))
  {
    return 0x01;
  }
  else
  {
    return 0x00;
  }
}


void MainWindow::on_pushButtonRecord_clicked()
{
    if (recordingEMGFileName.isEmpty() && recordingCombinedFileName.isEmpty())
    {
        // Not recording, start

        auto currentDir = QDir::current();

        if (!currentDir.exists("data"))
        {
            currentDir.mkdir("data");
        }

        if (gflistener->getAccDataRate() != GFListener::ACC_DATA_RATE::ACC_DATA_RATE_DISABLED ||
            gflistener->getGyroDataRate() != GFListener::GYRO_DATA_RATE::GYRO_DATA_RATE_DISABLED ||
            gflistener->getMagDataRate() != GFListener::MAG_DATA_RATE::MAG_DATA_RATE_DISABLED ||
            gflistener->getQuatDataRate() != GFListener::QUAT_DATA_RATE::QUAT_DATA_RATE_DISABLED)
        {
            // Save to combined file

            recordingCombinedFileName = (QString("data") + QDir::separator() + QString("combined_%1.json")).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
            gflistener->saveCombinedData(recordingCombinedFileName);
        }
        else
        {
            // Save to raw EMG file

            recordingEMGFileName = (QString("data") + QDir::separator() + QString("EMG_%1_%2bits_%3Hz.bin")).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"), gflistener->getEMGDataBits(), gflistener->getEMGDataRate());
            gflistener->saveEMGRawData(recordingEMGFileName);
        }

        ui->pushButtonRecord->setText(tr("Stop Recording"));
    }
    else
    {
        // Recording, stop

        if (!recordingEMGFileName.isEmpty())
        {
            gflistener->finishSaveEMGRawData();

            // qDebug() << recordingEMGFileName << "saved.";
            if (QMessageBox::question(this, tr("File Saved"), tr("File '%1' saved.\nSave to another place?").arg(recordingEMGFileName), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                QString newFilename = QFileDialog::getSaveFileName(this, tr("Save as"), ".", tr("Binary file (*.bin)"));

                if (newFilename.length() != 0)
                {
                    bool renamed = QFile::rename(recordingEMGFileName, newFilename);

                    if (renamed)
                    {
                        QMessageBox::information(this, tr("File renamed"), tr("File '%1' renamed to '%2'").arg(recordingEMGFileName, newFilename));
                    }
                }
            }

            recordingEMGFileName.clear();
        }


        if (!recordingCombinedFileName.isEmpty())
        {
            gflistener->finishSaveCombinedData();
            // qDebug() << finishSaveCombinedData << "saved.";

            if (QMessageBox::question(this, tr("File Saved"), tr("File '%1' saved.\nSave to another place?").arg(recordingCombinedFileName), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                QString newFilename = QFileDialog::getSaveFileName(this, tr("Save as"), ".", tr("JSON file (*.json)"));

                if (newFilename.length() != 0)
                {
                    bool renamed = QFile::rename(recordingCombinedFileName, newFilename);

                    if (renamed)
                    {
                        QMessageBox::information(this, tr("File renamed"), tr("File '%1' renamed to '%2'").arg(recordingCombinedFileName, newFilename));
                    }
                }
            }

            recordingCombinedFileName.clear();
        }


        ui->pushButtonRecord->setText(tr("Start Recording"));
        ui->pushButtonRecord->setEnabled(deviceConnected);
    }
}
