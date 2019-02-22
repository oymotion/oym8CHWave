#ifndef GFHUBLISTENER_H
#define GFHUBLISTENER_H

// include gforce sdk
#include "gforce.h"
#include <QVector>

// need to use namespace;
/// \namespace gf The namespace in which all of the gForceSDK project definitions are contained.
using namespace gf;
using namespace std;

// include widgets for push data;
#include "widget.h"
class Widget;

/**
*1.Create a class for inheriting HubListener class; then, can use virtual function;
*2.The gfHubListener implements HubListener,operates gForce device and receives data
**/
class gfHubListener : public HubListener
{

public:
    gfHubListener(Widget &w,gfsPtr<Hub>& pHub);
    ~gfHubListener();

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
    bool getLoop();
    //
    void connectDevice();
    void disconnectDevice();

private:
    // Indicates if we will keep message polling
    bool mLoop = true;
    // create a instance of hub.
    gfsPtr<Hub> mHub;
    // create a device to operate
    gfsPtr<Device> mDevice;
    Widget &m;

};

#endif // GFHUBLISTENER_H
