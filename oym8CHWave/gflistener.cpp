
#include "gflistener.h"

#include <QApplication>
#include <QJSEngine>
#include <QDebug>
#include "mainwindow.h"


class GfThread : public QThread
{
public:
    explicit GfThread(std::shared_ptr<gf::Hub> hub, QObject *parent = nullptr) :
        QThread(parent),
        m_hub(hub)
    {

    }

    ~GfThread()
    {
        // request thread stop
        requestInterruption();
        quit();
        wait();
    }


protected:
    void run()
    {
        GF_RET_CODE retCode = GF_RET_CODE::GF_SUCCESS;

        /**
         * Initialize hub. Could be failed in below cases:
         * 1.The hub is not plugged in the USB port.
         * 2.Other apps are connected to the hub already.
        **/
        while ((retCode = m_hub->init()) != GF_RET_CODE::GF_SUCCESS)
        {
            cout << "hub init failed: " << static_cast<GF_UINT32>(retCode) << endl;

            if (isInterruptionRequested())
                return;

            QThread::sleep(1);
        }

        cout << "hub init succeed: " << static_cast<GF_UINT32>(retCode) << endl;

        while (!isInterruptionRequested()){
            /// set up 50ms timeout so we can handle console commands; 50ms
            GF_UINT32 period = 50;

            /**
             * Hub::run could be failed in the below cases:
             * 1.other threads have already been launching it.
             * 2.WorkMode is not set to WorkMode::Polling.
            **/
            /// A return of GF_RET_CODE::GF_ERROR_TIMEOUT means no error but period expired.
            retCode = m_hub->run(period);

            if (GF_RET_CODE::GF_SUCCESS != retCode &&
                    GF_RET_CODE::GF_ERROR_TIMEOUT != retCode)
            {
                cout << "Method run() failed: " << static_cast<GF_UINT32>(retCode) << endl;
                break;
            }
        }
    }


private:
   // std::shared_ptr<gfListener> m_listenerPtr;
    std::shared_ptr<gf::Hub> m_hub;
};



gfListener::gfListener(std::shared_ptr<Hub> hub, QObject *parent) :QObject(parent),
    isSaveRawData(false),
    isSaveQuaternionData(false),
    mDevice(nullptr)
{
    gfThread = std::make_shared<GfThread>(hub);
    gfThread->start();
}


gfListener::~gfListener()
{
    qDebug("~gfListener()");
    finishSaveData();

    gfThread->requestInterruption();
    gfThread->quit();
    gfThread->wait();
}


QString gfListener::fullDevName(SPDEVICE device)
{
    QString fullName = QString::fromStdWString(device->getName());

    if (fullName.at(fullName.length() - 1) != ')')
    {
        auto address = QString::fromStdWString(device->getAddress());

        fullName += "(";
        fullName += address.at(3);
        fullName += address.at(4);
        fullName += address.at(0);
        fullName += address.at(1);
        fullName += ")";
    }

    return fullName;
}


/// This callback is called when the Hub finishes scanning devices.
void gfListener::onScanFinished()
{
    cout << __FUNCTION__ << " has been called." << endl;
    emit scanFinished();
}

/// This callback is called when the state of the hub changed
void gfListener::onStateChanged(HubState state)
{
    cout << __FUNCTION__ << " has been called. New state is " << static_cast<GF_UINT32>(state) << endl;

    /// if the hub is disconnected (such as unplugged), then set the flag of exiting the app.
    if (HubState::Disconnected == state)
    {
        emit dongleUnplugged();
    }
}

/// This callback is called when the hub finds a device.
void gfListener::onDeviceFound(SPDEVICE device)
{
    cout << __FUNCTION__ << " has been called." << endl;

    mFoundDevices.push_back(device);

    // send deviceFound signal
    emit deviceFound(fullDevName(device), device->getRssi());
}

/// This callback is called a device has been connected successfully
void gfListener::onDeviceConnected(SPDEVICE device)
{
    cout << __FUNCTION__ << " has been called." << endl;

    mDevice = device;

    if (mDevice)
    {
        auto setting = mDevice->getDeviceSetting();
        auto listener = this;

        if (nullptr != setting)
        {
            setting->enableDataNotification(0);

            setting->setEMGRawDataConfig(mDataRate,         //sample rate
                                         (DeviceSetting::EMGRowDataChannels)(0x00FF),  //channel 0~7
                                         128,               //data length
                                         mDataBits,         // adc resolution
                                         [listener, setting](ResponseResult result) {
                string ret = (result == ResponseResult::RREST_SUCCESS) ? ("sucess") : ("failed");
                qDebug() <<"setEMGRawDataConfig() returned:" << ret.c_str();

                if (result == ResponseResult::RREST_SUCCESS)
                {
                    DeviceSetting::DataNotifFlags flags;
                    flags = (DeviceSetting::DataNotifFlags)
                            (DeviceSetting::DNF_OFF
                             //| DeviceSetting::DNF_ACCELERATE
                             //| DeviceSetting::DNF_GYROSCOPE
                             //| DeviceSetting::DNF_MAGNETOMETER
                             //| DeviceSetting::DNF_EULERANGLE
                             //| DeviceSetting::DNF_QUATERNION
                             //| DeviceSetting::DNF_ROTATIONMATRIX
                             //| DeviceSetting::DNF_EMG_GESTURE
                             | DeviceSetting::DNF_EMG_RAW
                             //| DeviceSetting::DNF_HID_MOUSE
                             //| DeviceSetting::DNF_HID_JOYSTICK
                             //| DeviceSetting::DNF_DEVICE_STATUS
                             );

                    setting->setDataNotifSwitch(flags, [setting](ResponseResult result) {
                        qDebug() << "setDataNotifSwitch() returned:" << static_cast<GF_UINT32>(result);
                        setting->enableDataNotification(1);
                    });
                }
                else
                {
                    emit listener->emgSettingFailed();
                }
            });

        }
    }

    mDConnectStatus = true;

    // send deviceConnected signal
    emit deviceConnected();
}

/// This callback is called when a device has been disconnected from connection state or failed to connect to
void gfListener::onDeviceDisconnected(SPDEVICE device, GF_UINT8 reason)
{
    // if connection lost, we will try to reconnect again.
    cout << __FUNCTION__ << " has been called. reason: " << static_cast<GF_UINT32>(reason) << endl;

    if (mDevice == device)
    {
        mDConnectStatus = false;
        mDevice = nullptr;

        // send deviceDisConnected signal
        emit deviceDisConnected();
    }
}

/// This callback is called when the quaternion data is received
void gfListener::onOrientationData(SPDEVICE device, const Quaternion& rotation)
{
    Q_UNUSED(device)

    // print the quaternion data
//    cout << __FUNCTION__ << " has been called. " << rotation.toString() << endl;
//    QVector<float> dataTemp;
//    dataTemp.append(rotation.w());
//    dataTemp.append(rotation.x());
//    dataTemp.append(rotation.y());
//    dataTemp.append(rotation.z());
//    qDebug("dataTemp.append(rotation.x(0)); = %f",dataTemp[0]);
//    qDebug("dataTemp.append(rotation.x(1)); = %f",dataTemp[1]);
//    qDebug("dataTemp.append(rotation.x(2)); = %f",dataTemp[2]);
//    qDebug("dataTemp.append(rotation.x(3)); = %f",dataTemp[3]);
    //emit sendQuaternion(dataTemp);
     QJSEngine JS;
     QJSValue array = JS.newArray();

     array.setProperty(0, rotation.w());
     array.setProperty(1, rotation.x());
     array.setProperty(2, rotation.y());
     array.setProperty(3, rotation.z());
     emit sendQuaternion(array.toVariant());

     // save quaternion
     if (isSaveQuaternionData) {
         //save quaternion data
         if (g_quaternionFile.is_open()) {
             g_quaternionFile << rotation.w();
             g_quaternionFile << "  ";
             g_quaternionFile << rotation.x();
             g_quaternionFile << "  ";
             g_quaternionFile << rotation.y();
             g_quaternionFile << "  ";
             g_quaternionFile << rotation.z();
             g_quaternionFile << "\n";
//             qDebug("rotation.w() = %f", rotation.w());
//             qDebug("rotation.x() = %f", rotation.x());
//             qDebug("rotation.y() = %f", rotation.y());
//             qDebug("rotation.z() = %f", rotation.z());
         }
     }
}

/// This callback is called when the gesture data is recevied
void gfListener::onGestureData(SPDEVICE device, Gesture gest)
{
    Q_UNUSED(device)

    // a gesture event coming.
    string gesture;
    switch (gest)
    {
    case Gesture::Relax:
        gesture = "Relax";
        break;
    case Gesture::Fist:
        gesture = "Fist";
        break;
    case Gesture::SpreadFingers:
        gesture = "SpreadFingers";
        break;
    case Gesture::WaveIn:
        gesture = "WaveIn";
        break;
    case Gesture::WaveOut:
        gesture = "WaveOut";
        break;
    case Gesture::Pinch:
        gesture = "Pinch";
        break;
    case Gesture::Shoot:
        gesture = "Shoot";
        break;
    case Gesture::Undefined:
    default:
    {
        gesture = "Undefined: ";
        string s;
        stringstream ss(s);
        ss << static_cast<int>(gest);
        gesture += ss.str();
    }
    }
    cout << __FUNCTION__ << " has been called. " << gesture << endl;
}

/// This callback is called when the button on gForce is pressed by user
void gfListener::onDeviceStatusChanged(SPDEVICE device, DeviceStatus status)
{
    Q_UNUSED(device)

    string devicestatus;

    switch (status)
    {
    case DeviceStatus::ReCenter:
        devicestatus = "ReCenter";
        break;
    case DeviceStatus::UsbPlugged:
        devicestatus = "UsbPlugged";
        break;
    case DeviceStatus::UsbPulled:
        devicestatus = "UsbPulled";
        break;
    case DeviceStatus::Motionless:
        devicestatus = "Motionless";
        break;
    default:
        {
            devicestatus = "Undefined: ";
            string s;
            stringstream ss(s);
            ss << static_cast<int>(status);
            devicestatus += ss.str();
        }
    }
    cout << __FUNCTION__ << " has been called. " << devicestatus << endl;
}

void gfListener::onExtendedDeviceData(SPDEVICE device, DeviceDataType dataType, gfsPtr<const std::vector<GF_UINT8>> data)
{
    Q_UNUSED(device)

    // qDebug() << "dataType:" << static_cast<GF_UINT32>(dataType) << ", dataLen:" << data->size();

    switch (dataType) {
    case DeviceDataType::DDT_EMGRAW:
        {
            // qvector, must be RawData;
            if (128 == data->size())
            {
                QVector<uint8_t> tempRawData;

                if (mDataBits == DATA_BITS_12)
                {
                    for (size_t i = 0; i < data->size(); i += 2)
                    {
                        // 12bit -> 8bit
                        tempRawData.push_back(static_cast<uint8_t>((data->at(i) + (data->at(i + 1) << 8)) >> 4));
                    }
                }
                else if (mDataBits == DATA_BITS_8)
                {
                    for (size_t i = 0; i < data->size(); ++i)
                    {
                        tempRawData.push_back(data->at(i));
                    }
                }
                else
                {
                    break;
                }

                emit sendDeviceData(tempRawData);
            }

            // save raw data
            if (isSaveRawData)
            {
                //save raw data with format
                if (g_file.is_open())
                {
                    g_file.write((const char*)data->data(), data->size());
                }
            }
        }
        break;

    case DeviceDataType::DDT_QUATERNION:
        {
        }
        break;

    default:
        break;
    }
}


void gfListener::connectDevice(const QString &devName, const DATA_BITS dataBits, const DATA_RATE dataRate) {
    SPDEVICE device = nullptr;

    qDebug() << "devName:" << devName << ", dataBits:" << dataBits << ", dataRate:" << dataRate;
    mDataBits = dataBits;   // Save for later use
    mDataRate = dataRate;

    for (auto dev : mFoundDevices)
    {
        if (fullDevName(dev) == devName)
        {
            qDebug() << "Connect to device:" << devName;
            device = dev;
            break;
        }
    }

    if (device == nullptr)
        return;

    /// If there already is a device found and it's not
    /// in connecting or connected state, try to connect it.
    DeviceConnectionStatus status = device->getConnectionStatus();

    if (DeviceConnectionStatus::Connected != status && DeviceConnectionStatus::Connecting != status)
    {
        device->connect();
    }
}

// disconnect action: for qml btn;
void gfListener::disconnectDevice() {
    // If there already is a device connected, disconnect it.
    if (nullptr != mDevice) {  // mDevice must not be nullptr
        DeviceConnectionStatus status = mDevice->getConnectionStatus();

        if (DeviceConnectionStatus::Connected == status)
        {
            mDevice->disconnect();
        }
    }
}

//void gfSdk::setDataType(int dataType)
//{
//    // 0: off; 1: rawdata; 2: quaternion
//    DeviceSetting::DataNotifFlags flags;
//    switch (dataType) {

//    case 0:
//        flags = (DeviceSetting::DataNotifFlags)
//                (DeviceSetting::DNF_OFF);

//        if (mDevice)
//        {
//            auto setting = mDevice->getDeviceSetting();
//            if (nullptr != setting)
//            {
//                setting->setDataNotifSwitch(flags, [](ResponseResult result) {
//                    cout << "setDataNotifSwitch: " << static_cast<GF_UINT32>(result) << endl;
//                });
//            }
//        }
//        break;

//    case 1:
//        flags = (DeviceSetting::DataNotifFlags)
//                (DeviceSetting::DNF_EMG_RAW);

//        if (mDevice)
//        {
//            auto setting = mDevice->getDeviceSetting();
//            if (nullptr != setting)
//            {
//                setting->setDataNotifSwitch(flags, [](ResponseResult result) {
//                    cout << "setDataNotifSwitch: " << static_cast<GF_UINT32>(result) << endl;
//                });
//                // configure "EMG raw data" parameters
//                setting->setEMGRawDataConfig(650,         //sample rate
//                                             (DeviceSetting::EMGRowDataChannels)(0x00FF),  //channel 0~7
//                                             128,    //data length
//                                             8,       // adc resolution
//                                             [](ResponseResult result) {
//                    string ret = (result == ResponseResult::RREST_SUCCESS) ? ("sucess") : ("failed");
//                    cout << "[INFO]: Set Emg Config "<< ret << endl;
//                    qDebug() <<"Set Emg Config ";
//                });
//            }
//        }
//        break;

//    case 2:
//        flags = (DeviceSetting::DataNotifFlags)
//                (DeviceSetting::DNF_QUATERNION);

//        if (mDevice)
//        {
//            auto setting = mDevice->getDeviceSetting();

//            if (nullptr != setting)
//            {
//                setting->setDataNotifSwitch(flags, [](ResponseResult result) {
//                    cout << "setDataNotifSwitch: " << static_cast<GF_UINT32>(result) << endl;
//                });
//            }
//        }
//        break;

//    default:
//        break;
//    }
//}

void gfListener::saveRawData(QString fileName)
{
    qDebug("get folder name by qml signal: %s", fileName.toStdString().c_str());

    if (!fileName.isEmpty()) {
        isSaveRawData = true;
        QString fileNameBin = fileName + ".bin";
        g_file.open(fileNameBin.toStdString(), ios::binary | ios::app);

        if (g_file.is_open()) {
            qDebug(" %s is opened ",fileNameBin.toStdString().c_str());
        }
    }
}

void gfListener::finishSaveData()
{
    isSaveRawData = false;
    if (g_file.is_open()) {
        g_file.close();
    }
}

void gfListener::saveQuaternionData(QString fileName)
{
    qDebug("get folder name by qml signal: %s", fileName.toStdString().c_str());

    if (!fileName.isEmpty()) {
        isSaveQuaternionData = true;
        QString fileNameTxt = fileName + ".txt";
        g_quaternionFile.open(fileNameTxt.toStdString(), ios::app);

        if (g_quaternionFile.is_open()) {
            qDebug(" %s is opened ",fileNameTxt.toStdString().c_str());
        }
    }
}

void gfListener::finishSaveQuaternionData()
{
    isSaveQuaternionData = false;

    if (g_quaternionFile.is_open()) {
        g_quaternionFile.close();
    }
}

bool gfListener::getDeviceStatus()
{
    return mDConnectStatus;
}





