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
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  QFile f(":qdarkstyle/style.qss");

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

  ch_WindowInit();

  for (int i = 0; i < CHNUM; i++)
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

void MainWindow::on_drawLine(QVector<double> rawData)
{
  uint8_t dataBack;

  if (rawData.size() == 128)
  {
    for (int i = 0; i < 16; i += 2)
    {
      x.push_back(count++);
      sendData = 0;

      for (int n = 0; n < CHNUM; ++n)
      {
        int raw = (rawData[i*CHNUM+n] + rawData[(i+1)*CHNUM+n]) / 2;
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
