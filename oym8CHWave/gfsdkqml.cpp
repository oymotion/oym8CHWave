
#include "gfsdkqml.h"
#include <QApplication>
#include <QJSEngine>
#include <QDebug>
#include "GfThread/gfthread.h"
#include "mainwindow.h"

gfSdkQml::gfSdkQml(gfsPtr<Hub>& pHub, MainWindow &w, QObject *parent) :QObject(parent),
    mHub(pHub),
    m(w),
    isSaveRawData(false),
    isSaveQuaternionData(false),
    isManualLock(false),
    mDevice(nullptr)
{

}

gfSdkQml::~gfSdkQml()
{
    qDebug("~gfSdkQml()");
}

/// This callback is called when the Hub finishes scanning devices.
void gfSdkQml::onScanFinished()
{
    cout << __FUNCTION__ << " has been called." << endl;
    if (nullptr == mDevice)
    {
        /// if no device found, we do scan again
        mHub->startScan();
    }
    else
    {
        /// or if there already is a device found and it's not
        /// in connecting or connected state, try to connect it.
        DeviceConnectionStatus status = mDevice->getConnectionStatus();
        if (DeviceConnectionStatus::Connected != status &&
                DeviceConnectionStatus::Connecting != status)
        {
            mDevice->connect();
        }
    }
}

/// This callback is called when the state of the hub changed
void gfSdkQml::onStateChanged(HubState state)
{
    cout << __FUNCTION__ << " has been called. New state is " << static_cast<GF_UINT32>(state) << endl;
    /// if the hub is disconnected (such as unplugged), then set the flag of exiting the app.
    if (HubState::Disconnected == state)
    {
        emit dongleUnplugged();
    }
}

/// This callback is called when the hub finds a device.
void gfSdkQml::onDeviceFound(SPDEVICE device)
{
    /// In the sample app, we only connect to one device, so once we got one, we stop scanning.
    cout << __FUNCTION__ << " has been called." << endl;
    if (nullptr != device)
    {
        /// only search the first connected device if we connected it before
        if (nullptr == mDevice || device == mDevice)
        {
            /// get the pointer of sdk device`s pointer, and mDevice = device;
            /// finally, qt code get the device`s ownership;
            mDevice = device;
            mHub->stopScan();
        }
    }
}

/// This callback is called a device has been connected successfully
void gfSdkQml::onDeviceConnected(SPDEVICE device)
{
    cout << __FUNCTION__ << " has been called." << endl;
    DeviceSetting::DataNotifFlags flags;
    flags = (DeviceSetting::DataNotifFlags)
            (DeviceSetting::DNF_OFF
//             | DeviceSetting::DNF_ACCELERATE
//             | DeviceSetting::DNF_GYROSCOPE
//             | DeviceSetting::DNF_MAGNETOMETER
//             | DeviceSetting::DNF_EULERANGLE
               | DeviceSetting::DNF_QUATERNION
//             | DeviceSetting::DNF_ROTATIONMATRIX
//             | DeviceSetting::DNF_EMG_GESTURE
               | DeviceSetting::DNF_EMG_RAW
//             | DeviceSetting::DNF_HID_MOUSE
//             | DeviceSetting::DNF_HID_JOYSTICK
//             | DeviceSetting::DNF_DEVICE_STATUS
             );
    if (mDevice)
    {
        auto setting = mDevice->getDeviceSetting();
        if (nullptr != setting)
        {
            setting->setDataNotifSwitch(flags, [](ResponseResult result) {
                cout << "setDataNotifSwitch: " << static_cast<GF_UINT32>(result) << endl;
            });

            setting->setEMGRawDataConfig(200,         //sample rate
                                         (DeviceSetting::EMGRowDataChannels)(0x00FF),  //channel 0~7
                                         128,    //data length
                                         8,       // adc resolution
                                         [](ResponseResult result) {
                string ret = (result == ResponseResult::RREST_SUCCESS) ? ("sucess") : ("failed");
                cout << "[INFO]: Set Emg Config "<< ret << endl;
                qDebug() <<"Set Emg Config ";
            });
        }
    }

    mDConnectStatus = true;

    // send deviceConnected signal
    emit deviceConnected();
}

/// This callback is called when a device has been disconnected from connection state or failed to connect to
void gfSdkQml::onDeviceDisconnected(SPDEVICE device, GF_UINT8 reason)
{
    // if connection lost, we will try to reconnect again.
    cout << __FUNCTION__ << " has been called. reason: " << static_cast<GF_UINT32>(reason) << endl;

    mDConnectStatus = false;

    // send deviceDisConnected signal
    emit deviceDisConnected();

    // disconnected by power button
    if (!isManualLock) {
        mDevice = nullptr;
        // if no device found, we do scan again
        mHub->startScan();
    }
    else { // close qt program
    }
}

/// This callback is called when the quaternion data is received
void gfSdkQml::onOrientationData(SPDEVICE device, const Quaternion& rotation)
{
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
void gfSdkQml::onGestureData(SPDEVICE device, Gesture gest)
{
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
void gfSdkQml::onDeviceStatusChanged(SPDEVICE device, DeviceStatus status)
{
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

void gfSdkQml::onExtendedDeviceData(SPDEVICE device, DeviceDataType dataType, gfsPtr<const std::vector<GF_UINT8>> data)
{
    switch (dataType) {
    case DeviceDataType::DDT_EMGRAW:
    {

        // qvector, must be RawData;
        if (128 == data->size()) {
            QVector<uint8_t> tempRawData;
            for (int i = 0; i < data->size(); ++i) {
                tempRawData.push_back((*data)[i]);
            }
            emit sendDeviceData(tempRawData);
        }

        // save raw data
        if (isSaveRawData) {
            //save raw data with format
            if (g_file.is_open()) {
                g_file.write((const char *)&(*data)[0], data->size());
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

void gfSdkQml::connectDevice() {
    /// If there already is a device found and it's not
    /// in connecting or connected state, try to connect it.
    DeviceConnectionStatus status = mDevice->getConnectionStatus();
    if (DeviceConnectionStatus::Connected != status &&
            DeviceConnectionStatus::Connecting != status)
    {
        mDevice->connect();
    }
}

// disconnect action: for qml btn;
void gfSdkQml::disconnectDevice() {
    // If there already is a device connected, disconnect it.
    if (nullptr != mDevice) {  // mDevice must not be nullptr
        DeviceConnectionStatus status = mDevice->getConnectionStatus();
        if (DeviceConnectionStatus::Connected == status)
        {
            mDevice->disconnect();
            mDevice = nullptr;
            isManualLock = true;
        }
    }
}

void gfSdkQml::setDataType(int dataType)
{
    // 0: off; 1: rawdata; 2: quaternion
    DeviceSetting::DataNotifFlags flags;
    switch (dataType) {
    case 0:
        flags = (DeviceSetting::DataNotifFlags)
                (DeviceSetting::DNF_OFF);
        if (mDevice)
        {
            auto setting = mDevice->getDeviceSetting();
            if (nullptr != setting)
            {
                setting->setDataNotifSwitch(flags, [](ResponseResult result) {
                    cout << "setDataNotifSwitch: " << static_cast<GF_UINT32>(result) << endl;
                });
            }
        }
        break;
    case 1:
        flags = (DeviceSetting::DataNotifFlags)
                (DeviceSetting::DNF_EMG_RAW);
        if (mDevice)
        {
            auto setting = mDevice->getDeviceSetting();
            if (nullptr != setting)
            {
                setting->setDataNotifSwitch(flags, [](ResponseResult result) {
                    cout << "setDataNotifSwitch: " << static_cast<GF_UINT32>(result) << endl;
                });
                // configure "EMG raw data" parameters
                setting->setEMGRawDataConfig(650,         //sample rate
                                             (DeviceSetting::EMGRowDataChannels)(0x00FF),  //channel 0~7
                                             128,    //data length
                                             8,       // adc resolution
                                             [](ResponseResult result) {
                    string ret = (result == ResponseResult::RREST_SUCCESS) ? ("sucess") : ("failed");
                    cout << "[INFO]: Set Emg Config "<< ret << endl;
                    qDebug() <<"Set Emg Config ";
                });
            }
        }
        break;

    case 2:
        flags = (DeviceSetting::DataNotifFlags)
                (DeviceSetting::DNF_QUATERNION);
        if (mDevice)
        {
            auto setting = mDevice->getDeviceSetting();
            if (nullptr != setting)
            {
                setting->setDataNotifSwitch(flags, [](ResponseResult result) {
                    cout << "setDataNotifSwitch: " << static_cast<GF_UINT32>(result) << endl;
                });
            }
        }
        break;
    default:
        break;
    }
}

void gfSdkQml::saveRawData(QString fileName)
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

void gfSdkQml::finishSaveData()
{
    isSaveRawData = false;
    if (g_file.is_open()) {
        g_file.close();
    }
}

void gfSdkQml::saveQuaternionData(QString fileName)
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

void gfSdkQml::finishSaveQuaternionData()
{
    isSaveQuaternionData = false;
    if (g_quaternionFile.is_open()) {
        g_quaternionFile.close();
    }
}

bool gfSdkQml::getDeviceStatus()
{
    return mDConnectStatus;
}





