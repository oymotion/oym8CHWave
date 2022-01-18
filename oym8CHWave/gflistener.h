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
    void emgSettingFailed();

public:
    enum EMG_DATA_BITS {
        EMG_DATA_BITS_8 = 8,
        EMG_DATA_BITS_12 = 12
    };

    enum EMG_DATA_RATE {
        EMG_DATA_RATE_DISABLED = 0,
        EMG_DATA_RATE_100 = 100,
        EMG_DATA_RATE_200 = 200,
        EMG_DATA_RATE_250 = 250,
        EMG_DATA_RATE_400 = 400,
        EMG_DATA_RATE_500 = 500,
        EMG_DATA_RATE_650 = 650,
        EMG_DATA_RATE_1000=1000
    };

    enum ACC_DATA_RATE {
        ACC_DATA_RATE_DISABLED = 0,
        ACC_DATA_RATE_50 = 50,
        ACC_DATA_RATE_100 = 100,
        ACC_DATA_RATE_200 = 200
    };

    enum GYRO_DATA_RATE {
        GYRO_DATA_RATE_DISABLED = 0,
        GYRO_DATA_RATE_50 = 50,
        GYRO_DATA_RATE_100 = 100,
        GYRO_DATA_RATE_200 = 200
    };

    enum MAG_DATA_RATE {
        MAG_DATA_RATE_DISABLED = 0,
        MAG_DATA_RATE_5 = 5,
        MAG_DATA_RATE_10 = 10,
        MAG_DATA_RATE_25 = 25,
        MAG_DATA_RATE_50 = 50
    };

    enum QUAT_DATA_RATE {
        QUAT_DATA_RATE_DISABLED = 0,
        QUAT_DATA_RATE_50 = 50,
        QUAT_DATA_RATE_100 = 100,
        QUAT_DATA_RATE_200 = 200
    };


    // Save raw EMG data
    Q_INVOKABLE void saveEMGRawData(QString fileName);
    Q_INVOKABLE void finishSaveEMGRawData();

    // Save combined data
    Q_INVOKABLE void saveCombinedData(QString fileName);
    Q_INVOKABLE void finishSaveCombinedData();

    Q_INVOKABLE bool getDeviceStatus();
    //
    Q_INVOKABLE void connectDevice(const QString &devName, const EMG_DATA_BITS emgDataBits, const EMG_DATA_RATE emgDataRate, const ACC_DATA_RATE accelDataRate, const GYRO_DATA_RATE gyroDataRate, const MAG_DATA_RATE magDataRate, const QUAT_DATA_RATE quatDataRate);
    Q_INVOKABLE void disconnectDevice();

    EMG_DATA_BITS getEMGDataBits() {return mEMGDataBits;}
    EMG_DATA_RATE getEMGDataRate() {return mEMGDataRate;}

    ACC_DATA_RATE getAccDataRate() {return mAccelDataRate;}
    GYRO_DATA_RATE getGyroDataRate() {return mGyroDataRate;}
    MAG_DATA_RATE getMagDataRate() {return mMagDataRate;}
    QUAT_DATA_RATE getQuatDataRate() {return mQuatDataRate;}

private:
    QString fullDevName(SPDEVICE device);

    std::shared_ptr<QThread> gfThread;

    // create a device to operate
    std::shared_ptr<Device> mDevice;

    QVector<std::shared_ptr<Device>> mFoundDevices;

    bool mDConnectStatus = false;

    EMG_DATA_BITS mEMGDataBits;
    EMG_DATA_RATE mEMGDataRate;

    ACC_DATA_RATE mAccelDataRate;
    GYRO_DATA_RATE mGyroDataRate;
    MAG_DATA_RATE mMagDataRate;
    QUAT_DATA_RATE mQuatDataRate;

    bool isRecordingEMGRawData;
    bool isRecordingCombinedData;

    // file, object
    ofstream g_EMGfile;
    ofstream g_combinedFile;

    uint32_t timestampOffset;
};

#endif // GFLISTENER_H

