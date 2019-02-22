#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QTime>
#include <QtGlobal>
#include <qdebug.h>
#include "gfsdkqml.h"
#include "dialogconnect.h"
#include "qcustomplot/qcustomplot.h"


QT_USE_NAMESPACE


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ch_WindowInit();

  for (int i = 0; i < 8; i++)
  {
    chBufRaw[i].begin(32);
    chBufRawLast[i].begin(32);
  }
}

MainWindow::~MainWindow()
{
  if (serialPort.isOpen())
  {
    serialPort.close();
  }
  delete ui;
}

void MainWindow::ch_WindowInit()
{
  ui->customPlot_CH1->addGraph(); // CH1
  ui->customPlot_CH2->addGraph(); // CH2
  ui->customPlot_CH3->addGraph(); // CH3
  ui->customPlot_CH4->addGraph(); // CH4
  ui->customPlot_CH5->addGraph(); // CH5
  ui->customPlot_CH6->addGraph(); // CH6
  ui->customPlot_CH7->addGraph(); // CH7
  ui->customPlot_CH8->addGraph(); // CH8

  ui->customPlot_CH1->xAxis->setRange(0, X_RANGE);
  ui->customPlot_CH1->yAxis->setRange(0, Y_RANGE);
  ui->customPlot_CH2->xAxis->setRange(0, X_RANGE);
  ui->customPlot_CH2->yAxis->setRange(0, Y_RANGE);
  ui->customPlot_CH3->xAxis->setRange(0, X_RANGE);
  ui->customPlot_CH3->yAxis->setRange(0, Y_RANGE);
  ui->customPlot_CH4->xAxis->setRange(0, X_RANGE);
  ui->customPlot_CH4->yAxis->setRange(0, Y_RANGE);
  ui->customPlot_CH5->xAxis->setRange(0, X_RANGE);
  ui->customPlot_CH5->yAxis->setRange(0, Y_RANGE);
  ui->customPlot_CH6->xAxis->setRange(0, X_RANGE);
  ui->customPlot_CH6->yAxis->setRange(0, Y_RANGE);
  ui->customPlot_CH7->xAxis->setRange(0, X_RANGE);
  ui->customPlot_CH7->yAxis->setRange(0, Y_RANGE);
  ui->customPlot_CH8->xAxis->setRange(0, X_RANGE);
  ui->customPlot_CH8->yAxis->setRange(0, Y_RANGE);

  x.clear();
  count = 0;
}

void MainWindow::on_drawLine(QVector<double> rawData)
{
  uint8_t dataBack;

  if (rawData.size() == 128)
  {


    for (int i = 0; i < 16; i += 2)
    {
      x.push_back(count++);
      sendData = 0;

      for (int n = 0; n < 8; ++n)
      {
        int raw = (rawData[i*8+n] + rawData[(i+1)*8+n]) / 2;
        ch_data[n].push_back(raw);
        dataBack = filterProces(raw, n);
        sendData = (sendData | dataBack << n);
      }  

      if (serialPort.isOpen())
      {
        serialPort.write(&sendData, 1);
      }

    }



    while (x.length() > X_RANGE)
    {
      x.pop_front();

      for (int n = 0; n < 8; ++n)
      {
        ch_data[n].pop_front();
      }
    }

    ui->customPlot_CH1->graph(0)->setData(x, ch_data[0]);
    ui->customPlot_CH2->graph(0)->setData(x, ch_data[1]);
    ui->customPlot_CH3->graph(0)->setData(x, ch_data[2]);
    ui->customPlot_CH4->graph(0)->setData(x, ch_data[3]);
    ui->customPlot_CH5->graph(0)->setData(x, ch_data[4]);
    ui->customPlot_CH6->graph(0)->setData(x, ch_data[5]);
    ui->customPlot_CH7->graph(0)->setData(x, ch_data[6]);
    ui->customPlot_CH8->graph(0)->setData(x, ch_data[7]);

    auto range_1 = ui->customPlot_CH1->yAxis->range();
    auto range_2 = ui->customPlot_CH2->yAxis->range();
    auto range_3 = ui->customPlot_CH3->yAxis->range();
    auto range_4 = ui->customPlot_CH4->yAxis->range();
    auto range_5 = ui->customPlot_CH5->yAxis->range();
    auto range_6 = ui->customPlot_CH6->yAxis->range();
    auto range_7 = ui->customPlot_CH7->yAxis->range();
    auto range_8 = ui->customPlot_CH8->yAxis->range();

    ui->customPlot_CH1->rescaleAxes();
    ui->customPlot_CH2->rescaleAxes();
    ui->customPlot_CH3->rescaleAxes();
    ui->customPlot_CH4->rescaleAxes();
    ui->customPlot_CH5->rescaleAxes();
    ui->customPlot_CH6->rescaleAxes();
    ui->customPlot_CH7->rescaleAxes();
    ui->customPlot_CH8->rescaleAxes();

    ui->customPlot_CH1->yAxis->setRange(range_1);
    ui->customPlot_CH2->yAxis->setRange(range_2);
    ui->customPlot_CH3->yAxis->setRange(range_3);
    ui->customPlot_CH4->yAxis->setRange(range_4);
    ui->customPlot_CH5->yAxis->setRange(range_5);
    ui->customPlot_CH6->yAxis->setRange(range_6);
    ui->customPlot_CH7->yAxis->setRange(range_7);
    ui->customPlot_CH8->yAxis->setRange(range_8);

    ui->customPlot_CH1->replot();
    ui->customPlot_CH2->replot();
    ui->customPlot_CH3->replot();
    ui->customPlot_CH4->replot();
    ui->customPlot_CH5->replot();
    ui->customPlot_CH6->replot();
    ui->customPlot_CH7->replot();
    ui->customPlot_CH8->replot();
  }
}

void MainWindow::handleReadyRead()
{
  //qDebug() << "serialPort.read: " << serialPort.read(1);
}

void MainWindow::on_actionStop_triggered()
{
  this->close();
}

void MainWindow::on_actionConnect_triggered()
{
  if (serialPort.isOpen())
    {
      //disconnect(&serialPort, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
      serialPort.close();
    }
  else
  {
    DialogConnect dialogConnect(this);
    dialogConnect.show();
  }

  if (serialPort.isOpen())
    {
      ui->actionConnect->setText(tr("Disconnect"));

      //connect(&serialPort, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
      //serialPort.write("Connect Success\n");
      qDebug() <<"Connect Success";

      sendData = 0;
    }
  else
    {
      ui->actionConnect->setText(tr("Connect"));
    }
}


uint8_t MainWindow::filterProces(int channelData, int CHNum)
{
//  auto *chData = channelData.data();


    chBufRaw[CHNum].write(channelData);

    chBufRawLast[CHNum].write(abs(channelData - chBufRaw[CHNum].readMean()));


  if (abs(chBufRawLast[CHNum].readMean()) > 15)
  {
    return 0x01;
  }
  else
  {
    return 0x00;
  }
}
