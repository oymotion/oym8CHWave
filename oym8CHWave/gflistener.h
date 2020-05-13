#ifndef GFLISTENER_H
#define GFLISTENER_H

// include qt library
#include <QObject>
#include <QVariant>
#include <QVector>
#include <QThread>

// C++11
#include <fstream>
#include <iostream>

// include gforce sdk
#include "third_party/gforce/inc/gforce.h"


// need to use namespace;
/// \namespace gf The namespace in which all of the gForceSDK project definitions are contained.
using namespace gf;
using namespace std;


/**
*1.Create a class for inheriting HubListener class; then, can use virtual function;
*2.The gfSdk implements HubListener,operates gForce device and receives data
**/
class gfListener : public QObject, public HubListener
{
    Q_OBJECT

public:
    gfListener(std::shared_ptr<Hub> hub, QObject *parent = nullptr);
    ~gfListener();

    // below is gforce sdk callback function
    virtual void onScanFinished();
    virtual void onStateChanged(HubState state);
    virtual void onDeviceFound(SPDEVICE device);
    virtual void onDeviceConnected(SPDEVICE device);
    virtual void onDeviceDisconnected(SPDEVICE device, GF_UINT8 reason);
    virtual void onOrientationData(SPDEVICE device, const Quaternion& rotation);
    virtual void onGestureData(SPDEVICE device, Gesture gest);
    virtual void onDeviceStatusChanged(SPDEVICE device, DeviceStatus status);
    virtual void onExtendedDeviceData(SPDEVICE device, DeviceDataType dataType, gfsPtr<const std::vector<GF_UINT8>> data);

signals:
    QVariant sendQuaternion(QVariant quaternion);
    void sendDeviceData(QVector<uint8_t> data);
    // void sendQuaternion(QVector<float> quaternion);
    // dongle unplugged
    void dongleUnplugged();
    void scanFinished();
    void deviceFound(QString devName, unsigned int rssi);
    void deviceConnected();
    void deviceDisConnected();

public:
    enum DATA_BITS {DATA_BITS_8 = 8, DATA_BITS_12 = 12};
    enum DATA_RATE {DATA_RATE_100 = 100, DATA_RATE_200 = 200, DATA_RATE_250 = 250, DATA_RATE_400 = 400, DATA_RATE_500 = 500, DATA_RATE_650 = 650};

    Q_INVOKABLE void saveRawData(QString fileName);
    Q_INVOKABLE void finishSaveData();
    // Q_INVOKABLE void setDataType(int dataType);
    // save quaternion
    Q_INVOKABLE void saveQuaternionData(QString fileName);
    Q_INVOKABLE void finishSaveQuaternionData();
    Q_INVOKABLE bool getDeviceStatus();
    //
    Q_INVOKABLE void connectDevice(const QString &devName, const DATA_BITS dataBits=DATA_BITS_8, const DATA_RATE dataRate=DATA_RATE_500);
    Q_INVOKABLE void disconnectDevice();

private:
    std::shared_ptr<QThread> gfThread;

    // create a device to operate
    std::shared_ptr<Device> mDevice;

    QVector<std::shared_ptr<Device>> mFoundDevices;

    bool mDConnectStatus = false;

    DATA_BITS mDataBits;
    DATA_RATE mDataRate;

    bool isSaveRawData;
    bool isSaveQuaternionData;
    // file, object
    ofstream g_file;
    ofstream g_quaternionFile;
};

#endif // GFLISTENER_H

