#include "gfthread.h"
#include "mainwindow.h"


extern std::promise<bool> isPHubOK;

GfThread::GfThread(std::shared_ptr<gfSdkQml> listener, std::shared_ptr<gf::Hub> pHub, QObject *parent) :
    QThread(parent),
    m_listenerPtr(listener),
    m_hub(pHub)
{

}

GfThread::~GfThread()
{
    // request thread stop
    requestInterruption();
    quit();
    wait();
}

void GfThread::stop()
{

}

void GfThread::run()
{
    GF_RET_CODE retCode = GF_RET_CODE::GF_SUCCESS;
    cout << __FUNCTION__ << " has been called." << endl;
    m_hub->setWorkMode(WorkMode::Polling);
    gfsPtr<HubListener> listener = static_pointer_cast<HubListener>(m_listenerPtr);
    retCode = m_hub->registerListener(listener);
    cout << "registerListener " << ((retCode == GF_RET_CODE::GF_SUCCESS) ? "SUCCESS" : "FAIL") << endl;

    /**
     * Initialize hub. Could be failed in below cases:
     * 1.The hub is not plugged in the USB port.
     * 2.Other apps are connected to the hub already.
    **/
    retCode = m_hub->init();

    if (GF_RET_CODE::GF_SUCCESS == retCode) {
        isPHubOK.set_value(true);
        cout << "hub init succeed: " << static_cast<GF_UINT32>(retCode) << endl;
        retCode = m_hub->startScan();
        cout << "start Scan succeed: " << static_cast<GF_UINT32>(retCode) << endl;
    }
    else {
        isPHubOK.set_value(false);
        cout << "hub init failed: " << static_cast<GF_UINT32>(retCode) << endl;
    }

    while(!isInterruptionRequested()){
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
    // release phub resource
    m_hub->unRegisterListener(m_listenerPtr);
    m_hub->deinit();
}
