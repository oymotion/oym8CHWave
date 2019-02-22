#include "dialogconnect.h"
#include "ui_dialogconnect.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "mainwindow.h"


extern QSerialPort serialPort;


DialogConnect::DialogConnect(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogConnect)
{
  ui->setupUi(this);
}

DialogConnect::~DialogConnect()
{
  delete ui;
}

void DialogConnect::show()
{
//  if (serialPort.isOpen())
//    serialPort.close();

  auto ports = QSerialPortInfo::availablePorts();

  for (auto it=ports.begin(); it!=ports.end(); it++)
    ui->comboBox->addItem(it->portName());

  this->exec();
}

void DialogConnect::on_buttonBox_accepted()
{
  MainWindow* mainWindow = (MainWindow*)parent();

  mainWindow->serialPort.setPortName(ui->comboBox->currentText());
  mainWindow->serialPort.setBaudRate(QSerialPort::Baud115200);

  if (mainWindow->serialPort.open(QIODevice::ReadWrite))
  {
  }
}
