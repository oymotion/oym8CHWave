
#include "gflistener.h"

#include <QApplication>
#include <QJSEngine>
#include <QDebug>
#include "mainwindow.h"



GFListener::GFListener(std::shared_ptr<Hub> hub, QObject *parent) :QObject(parent),
    isRecordingEMGRawData(false),
    isRecordingCombinedData(false),
    mDevice(nullptr)
{

}


GFListener::~GFListener()
{
    qDebug("~gfListener() called.");
    finishSaveEMGRawData();
    finishSaveCombinedData();
}


QString GFListener::fullDevName(SPDEVICE device)
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
void GFListener::onScanFinished()
{
    cout << __FUNCTION__ << " has been called." << endl;
    emit scanFinished();
}

/// This callback is called when the state of the hub changed
void GFListener::onStateChanged(HubState state)
{
    cout << __FUNCTION__ << " has been called. New state is " << static_cast<GF_UINT32>(state) << endl;

    /// if the hub is disconnected (such as unplugged), then set the flag of exiting the app.
    if (HubState::Disconnected == state)
    {
        emit dongleUnplugged();
    }
}

/// This callback is called when the hub finds a device.
void GFListener::onDeviceFound(SPDEVICE device)
{
    cout << __FUNCTION__ << " has been called." << endl;

    mFoundDevices.push_back(device);

    // send deviceFound signal
    emit deviceFound(fullDevName(device), device->getRssi());
}

/// This callback is called a device has been connected successfully
void GFListener::onDeviceConnected(SPDEVICE device)
{
    cout << __FUNCTION__ << " has been called." << endl;

    mDevice = device;

    if (mDevice)
    {
        auto setting = mDevice->getDeviceSetting();
        auto listener = this;

        if (nullptr != setting)
        {
            // DeviceSetting::DataNotifFlags flags;
            // flags = (DeviceSetting::DataNotifFlags)
            //         (DeviceSetting::DNF_OFF
            //          //| DeviceSetting::DNF_ACCELERATE
            //          //| DeviceSetting::DNF_GYROSCOPE
            //          //| DeviceSetting::DNF_MAGNETOMETER
            //          //| DeviceSetting::DNF_EULERANGLE
            //          //| DeviceSetting::DNF_QUATERNION
            //          //| DeviceSetting::DNF_ROTATIONMATRIX
            //          //| DeviceSetting::DNF_EMG_GESTURE
            //          //| DeviceSetting::DNF_EMG_RAW
            //          //| DeviceSetting::DNF_HID_MOUSE
            //          //| DeviceSetting::DNF_HID_JOYSTICK
            //          //| DeviceSetting::DNF_DEVICE_STATUS
            //          );

            unsigned int flags = DeviceSetting::DNF_OFF;

            if (listener->mEMGDataRate != EMG_DATA_RATE::EMG_DATA_RATE_DISABLED)
                flags |= DeviceSetting::DNF_EMG_RAW;

            if (listener->mAccelDataRate != ACC_DATA_RATE::ACC_DATA_RATE_DISABLED)
                flags |= DeviceSetting::DNF_ACCELERATE;

            if (listener->mGyroDataRate != GYRO_DATA_RATE::GYRO_DATA_RATE_DISABLED)
                flags |= DeviceSetting::DNF_GYROSCOPE;

            if (listener->mMagDataRate != MAG_DATA_RATE::MAG_DATA_RATE_DISABLED)
                flags |= DeviceSetting::DNF_MAGNETOMETER;

            if (listener->mQuatDataRate != QUAT_DATA_RATE::QUAT_DATA_RATE_DISABLED)
                flags |= DeviceSetting::DNF_QUATERNION;


            setting->enableDataNotification(0);

            setting->setDataNotifSwitch((DeviceSetting::DataNotifFlags)flags, [listener, setting](ResponseResult result) {
                qDebug() << "setDataNotifSwitch() returned:" << static_cast<GF_UINT32>(result);

                if (result == ResponseResult::RREST_SUCCESS)
                {
                    if (listener->mEMGDataRate != EMG_DATA_RATE::EMG_DATA_RATE_DISABLED)
                    {
                        setting->setEMGRawDataConfig(listener->mEMGDataRate,         //sample rate
                                                     (DeviceSetting::EMGRowDataChannels)(0x00FF),  //channel 0~7
                                                     128,               //data length
                                                     listener->mEMGDataBits,         // adc resolution
                                                     [listener, setting](ResponseResult result) {
                            string ret = (result == ResponseResult::RREST_SUCCESS) ? ("sucess") : ("failed");
                            qDebug() <<"setEMGRawDataConfig() returned:" << ret.c_str();

                            if (result == ResponseResult::RREST_SUCCESS)
                            {
                                setting->enableDataNotification(1);
                            }
                            else
                            {
                                emit listener->emgSettingFailed();
                            }
                        });
                    }
                    else
                    {
                        setting->enableDataNotification(1);
                    }
                }
                else
                {
                    // TODO: emit error signal
                }
            });
        }
    }

    mDConnectStatus = true;

    // send deviceConnected signal
    emit deviceConnected();
}

/// This callback is called when a device has been disconnected from connection state or failed to connect to
void GFListener::onDeviceDisconnected(SPDEVICE device, GF_UINT8 reason)
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
void GFListener::onOrientationData(SPDEVICE device, const Quaternion& rotation)
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


     if (isRecordingCombinedData)
     {
         if (g_combinedFile.is_open())
         {
             // Save quaternion data
             g_combinedFile << QString::asprintf("    {\"timestamp\" : %ul, \"QUAT\" : [%f, %f, %f, %f]}\n",
                                                 QDateTime::currentMSecsSinceEpoch() - timestampOffset,
                                                 rotation.w(), rotation.x(), rotation.y(), rotation.z()).toStdString();
         }
     }
}

/// This callback is called when the gesture data is recevied
void GFListener::onGestureData(SPDEVICE device, Gesture gest)
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
void GFListener::onDeviceStatusChanged(SPDEVICE device, DeviceStatus status)
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

void GFListener::onExtendedDeviceData(SPDEVICE device, DeviceDataType dataType, gfsPtr<const std::vector<GF_UINT8>> data)
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

                if (mEMGDataBits == EMG_DATA_BITS_12)
                {
                    for (size_t i = 0; i < data->size(); i += 2)
                    {
                        // 12bit -> 8bit
                        tempRawData.push_back(static_cast<uint8_t>((data->at(i) + (data->at(i + 1) << 8)) >> 4));
                    }
                }
                else if (mEMGDataBits == EMG_DATA_BITS_8)
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

            if (isRecordingEMGRawData)
            {
                if (g_EMGfile.is_open())
                {
                    // Save raw data
                    g_EMGfile.write((const char*)data->data(), data->size());
                }
            }
            else if (isRecordingCombinedData)
            {
                if (g_combinedFile.is_open())
                {
                    // Save EMG data

                    QString arrayString("[");

                    if (mEMGDataBits == EMG_DATA_BITS_12)
                    {
                        for (size_t i = 0; i < data->size(); i += 2)
                        {
                            // Pack 12bit
                            arrayString.append(QString::asprintf("%d, ", data->at(i) + (data->at(i + 1) << 8)));
                        }
                    }
                    else if (mEMGDataBits == EMG_DATA_BITS_8)
                    {
                        for (size_t i = 0; i < data->size(); ++i)
                        {
                            arrayString.append(QString::asprintf("%d, ", data->at(i)));
                        }
                    }

                    arrayString.chop(2);
                    arrayString.append("]");

                    g_combinedFile << "\n    {\"timestamp\" : " << QDateTime::currentMSecsSinceEpoch() - timestampOffset
                                   << ", \"EMG\" : " << arrayString.toStdString()
                                   << "},";
                }
            }
        }
        break;

    case DeviceDataType::DDT_ACCELERATE:
        {
            if (isRecordingCombinedData)
            {
                if (g_combinedFile.is_open())
                {
                    // Save accel data
                    QString arrayString("[");

                    for (size_t i = 0; i < data->size(); i += 4)
                    {
                        // Pack data
                        int32_t q15 = data->at(i) + (data->at(i + 1) << 8) + (data->at(i + 2) << 16) + (data->at(i + 3) << 24);
                        arrayString.append(QString::asprintf("%f, ", q15 / 65536.0f));
                    }

                    arrayString.chop(2);
                    arrayString.append("]");

                    // qDebug() << QDateTime::currentMSecsSinceEpoch() << timestampOffset << QDateTime::currentMSecsSinceEpoch() - timestampOffset;

                    g_combinedFile << "\n    {\"timestamp\" : " << QDateTime::currentMSecsSinceEpoch() - timestampOffset
                                   << ", \"ACC\" : " << arrayString.toStdString()
                                   << "},";
                }
            }
        }
        break;

    case DeviceDataType::DDT_GYROSCOPE:
        {
            if (isRecordingCombinedData)
            {
                if (g_combinedFile.is_open())
                {
                    // Save gyro data
                    QString arrayString("[");

                    for (size_t i = 0; i < data->size(); i += 4)
                    {
                        // Pack data
                        int32_t q15 = data->at(i) + (data->at(i + 1) << 8) + (data->at(i + 2) << 16) + (data->at(i + 3) << 24);
                        arrayString.append(QString::asprintf("%f, ", q15 / 65536.0f));
                    }

                    arrayString.chop(2);
                    arrayString.append("]");

                    g_combinedFile << "\n    {\"timestamp\" : " << QDateTime::currentMSecsSinceEpoch() - timestampOffset
                                   << ", \"GYRO\" : " << arrayString.toStdString()
                                   << "},";
                }
            }
        }
        break;

    case DeviceDataType::DDT_MAGNETOMETER:
        {
            if (isRecordingCombinedData)
            {
                if (g_combinedFile.is_open())
                {
                    // Save mag data
                    QString arrayString("[");

                    for (size_t i = 0; i < data->size(); i += 4)
                    {
                        // Pack data
                        int32_t q15 = data->at(i) + (data->at(i + 1) << 8) + (data->at(i + 2) << 16) + (data->at(i + 3) << 24);
                        arrayString.append(QString::asprintf("%f, ", q15 / 65536.0f));
                    }

                    arrayString.chop(2);
                    arrayString.append("]");

                    g_combinedFile << "\n    {\"timestamp\" : " << QDateTime::currentMSecsSinceEpoch() - timestampOffset
                                   << ", \"MAG\" : " << arrayString.toStdString()
                                   << "},";
                }
            }
        }
        break;

      // Quaternion data is NOT here
//    case DeviceDataType::DDT_QUATERNION:
//        break;

    default:
        break;
    }
}


void GFListener::connectDevice(const QString &devName, const EMG_DATA_BITS emgDataBits, const EMG_DATA_RATE emgDataRate, const ACC_DATA_RATE accelDataRate, const GYRO_DATA_RATE gyroDataRate, const MAG_DATA_RATE magDataRate, const QUAT_DATA_RATE quatDataRate) {
    SPDEVICE device = nullptr;

    qDebug() << "devName:" << devName << ", emgDataBits:" << emgDataBits << ", emgDataRate:" << emgDataRate;
    mEMGDataBits = emgDataBits;   // Save for later use
    mEMGDataRate = emgDataRate;
    mAccelDataRate = accelDataRate;
    mGyroDataRate = gyroDataRate;
    mMagDataRate = magDataRate;
    mQuatDataRate = quatDataRate;

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
void GFListener::disconnectDevice() {
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

void GFListener::saveEMGRawData(QString fileName)
{
    qDebug("saveEMGRawData: '%s'", fileName.toStdString().c_str());

    if (!fileName.isEmpty())
    {
        isRecordingEMGRawData = true;
        g_EMGfile.open(fileName.toStdString(), ios::binary | ios::app);

        if (g_EMGfile.is_open())
        {
            qDebug("'%s' is opened.", fileName.toStdString().c_str());
        }
    }
}

void GFListener::finishSaveEMGRawData()
{
    if (isRecordingEMGRawData)
    {
        if (g_EMGfile.is_open())
        {
            g_EMGfile.close();
        }

        isRecordingEMGRawData = false;
    }
}

void GFListener::saveCombinedData(QString fileName)
{
    qDebug("saveCombinedData: '%s'", fileName.toStdString().c_str());

    if (!fileName.isEmpty())
    {
        isRecordingCombinedData = true;
        timestampOffset = QDateTime::currentMSecsSinceEpoch();
        g_combinedFile.open(fileName.toStdString());

        if (g_combinedFile.is_open())
        {
            qDebug("'%s' is opened.", fileName.toStdString().c_str());

            g_combinedFile << "{\n  \"data\": [";
        }
    }
}

void GFListener::finishSaveCombinedData()
{
    if (isRecordingCombinedData)
    {
        if (g_combinedFile.is_open())
        {
            unsigned long pos = g_combinedFile.tellp();
            g_combinedFile.seekp(pos - 1); // Remove trailing ","
            qDebug() << "prev pos:" << pos << ", new pos:" << g_combinedFile.tellp();
            g_combinedFile << "\n  ]\n}\n";
            g_combinedFile.close();
        }

        isRecordingCombinedData = false;
    }
}

bool GFListener::getDeviceStatus()
{
    return mDConnectStatus;
}





