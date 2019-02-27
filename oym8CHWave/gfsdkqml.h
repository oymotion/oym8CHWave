#ifndef GFSDKQML_H
#define GFSDKQML_H

// include qt library
#include <QObject>
#include <QVariant>
#include <QVector>

// include gforce sdk
#include "third_party/gforce/inc/gforce.h"

// C++11
#include <fstream>
#include <iostream>

#include "mainwindow.h"

// need to use namespace;
/// \namespace gf The namespace in which all of the gForceSDK project definitions are contained.
using namespace gf;
using namespace std;

class MainWindow;

/**
*1.Create a class for inheriting HubListener class; then, can use virtual function;
*2.The gfSdkQml implements HubListener,operates gForce device and receives data
**/
class gfSdkQml : public QObject, public HubListener
{
    Q_OBJECT

public:
    gfSdkQml(gfsPtr<Hub>& pHub, MainWindow &w, QObject *parent = 0);
    ~gfSdkQml();

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
//  void sendQuaternion(QVector<float> quaternion);
    // dongle unplugged
    void dongleUnplugged();
    void deviceConnected();
    void deviceDisConnected();

public:
    Q_INVOKABLE void saveRawData(QString fileName);
    Q_INVOKABLE void finishSaveData();
    Q_INVOKABLE void setDataType(int dataType);
    // save quaternion
    Q_INVOKABLE void saveQuaternionData(QString fileName);
    Q_INVOKABLE void finishSaveQuaternionData();
    Q_INVOKABLE bool getDeviceStatus();
    //
    Q_INVOKABLE void connectDevice();
    Q_INVOKABLE void disconnectDevice();

private:
    bool mDConnectStatus = false;
    // create a instance of hub.
    gfsPtr<Hub> mHub;
    // create a device to operate
    gfsPtr<Device> mDevice;
    bool isSaveRawData;
    bool isSaveQuaternionData;
    // file, object
    ofstream g_file;
    ofstream g_quaternionFile;
    // manual-lock Device
    bool isManualLock;


    MainWindow &m;
};

#endif // GFSDKQML_H
