#ifndef GFHUBTHREAD_H
#define GFHUBTHREAD_H

#include <QThread>

// include gforce sdk
#include "third_party/gforce/inc/gforce.h"


// need to use namespace;
/// \namespace gf The namespace in which all of the gForceSDK project definitions are contained.
using namespace gf;
using namespace std;


class GFHubThread : public QThread
{
    Q_OBJECT

public:
    explicit GFHubThread(std::shared_ptr<gf::Hub> hub, QObject *parent = nullptr);

    virtual ~GFHubThread();


signals:
    void hubReady();


protected:
    void run();


private:
   // std::shared_ptr<gfListener> m_listenerPtr;
    std::shared_ptr<gf::Hub> m_hub;
};


#endif // GFHUBTHREAD_H
