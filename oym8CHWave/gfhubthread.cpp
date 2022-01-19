#include "gfhubthread.h"


// need to use namespace;
/// \namespace gf The namespace in which all of the gForceSDK project definitions are contained.
using namespace gf;
using namespace std;


GFHubThread::GFHubThread(std::shared_ptr<gf::Hub> hub, QObject *parent) :
    QThread(parent),
    m_hub(hub)
{

}

GFHubThread::~GFHubThread()
{
    qDebug("~GFHubThread() called.");

    // request thread stop
    requestInterruption();
    quit();
    wait();

    m_hub->deinit();
}


void GFHubThread::run()
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

    m_hub->setWorkMode(WorkMode::Polling);

    emit hubReady();

    while (!isInterruptionRequested()){
        /// set up 50ms timeout so we can handle console commands; 50ms
        GF_UINT32 period = 10;

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

