
#include "gfhublistener.h"
#include <QtDebug>

gfHubListener::gfHubListener(Widget &w, gfsPtr<Hub>& pHub) :
    mHub(pHub),
    m(w)   // 初始化一个引用的值；
{

}

gfHubListener::~gfHubListener()
{

}

/// This callback is called when the Hub finishes scanning devices.
void gfHubListener::onScanFinished()
{
    cout << __FUNCTION__ << " has been called." << endl;
    if (nullptr == mDevice)
    {
        // if no device found, we do scan again
        mHub->startScan();
    }
    else
    {
        // or if there already is a device found and it's not
        //     in connecting or connected state, try to connect it.
        DeviceConnectionStatus status = mDevice->getConnectionStatus();
        if (DeviceConnectionStatus::Connected != status &&
                DeviceConnectionStatus::Connecting != status)
        {
            mDevice->connect();
        }
    }
}

/// This callback is called when the state of the hub changed
void gfHubListener::onStateChanged(HubState state)
{
    cout << __FUNCTION__ << " has been called. New state is " << static_cast<GF_UINT32>(state) << endl;
    // if the hub is disconnected (such as unplugged), then set the flag of exiting the app.
    if (HubState::Disconnected == state)
    {
        mLoop = false;
    }
}

/// This callback is called when the hub finds a device.
void gfHubListener::onDeviceFound(SPDEVICE device)
{
    // In the sample app, we only connect to one device, so once we got one, we stop scanning.
    cout << __FUNCTION__ << " has been called." << endl;
    if (nullptr != device)
    {
        // only search the first connected device if we connected it before
        if (nullptr == mDevice || device == mDevice)
        {
            mDevice = device;
            mHub->stopScan();
        }
    }
}

/// This callback is called a device has been connected successfully
void gfHubListener::onDeviceConnected(SPDEVICE device)
{
    cout << __FUNCTION__ << " has been called." << endl;
    DeviceSetting::DataNotifFlags flags;
    flags = (DeviceSetting::DataNotifFlags)
            (DeviceSetting::DNF_OFF
//             | DeviceSetting::DNF_ACCELERATE
             //| DeviceSetting::DNF_GYROSCOPE
             //| DeviceSetting::DNF_MAGNETOMETER
             //| DeviceSetting::DNF_EULERANGLE
             //| DeviceSetting::DNF_QUATERNION
             //| DeviceSetting::DNF_ROTATIONMATRIX
//             | DeviceSetting::DNF_EMG_GESTURE
             | DeviceSetting::DNF_EMG_RAW
//             | DeviceSetting::DNF_HID_MOUSE
//             | DeviceSetting::DNF_HID_JOYSTICK
//             | DeviceSetting::DNF_DEVICE_STATUS
             );
    if (device)
    {
        auto setting = device->getDeviceSetting();
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
            });
        }
    }
}

/// This callback is called when a device has been disconnected from
///                                 connection state or failed to connect to
void gfHubListener::onDeviceDisconnected(SPDEVICE device, GF_UINT8 reason)
{
    // if connection lost, we will try to reconnect again.
    cout << __FUNCTION__ << " has been called. reason: " << static_cast<GF_UINT32>(reason) << endl;
    if (nullptr != device && device == mDevice)
    {
        mDevice->connect();
    }
}

/// This callback is called when the quaternion data is received
void gfHubListener::onOrientationData(SPDEVICE device, const Quaternion& rotation)
{
    // print the quaternion data
    cout << __FUNCTION__ << " has been called. " << rotation.toString() << endl;
}

/// This callback is called when the gesture data is recevied
void gfHubListener::onGestureData(SPDEVICE device, Gesture gest)
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
void gfHubListener::onDeviceStatusChanged(SPDEVICE device, DeviceStatus status)
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

void gfHubListener::onExtendedDeviceData(SPDEVICE device, DeviceDataType dataType, gfsPtr<const std::vector<GF_UINT8>> data)
{
    m.pushRawData(data);
}

// Indicates if we want to exit app
bool gfHubListener::getLoop()
{
    return mLoop;
}

void gfHubListener::connectDevice() {
    // If there already is a device found and it's not
    //     in connecting or connected state, try to connect it.
    DeviceConnectionStatus status = mDevice->getConnectionStatus();
    if (DeviceConnectionStatus::Connected != status &&
            DeviceConnectionStatus::Connecting != status)
    {
        mDevice->connect();
    }
}

void gfHubListener::disconnectDevice() {
    // If there already is a device connected, disconnect it.
    DeviceConnectionStatus status = mDevice->getConnectionStatus();
    if (DeviceConnectionStatus::Connected == status)
    {
        mDevice->disconnect();
    }
}








