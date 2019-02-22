#ifndef GFTHREAD_H
#define GFTHREAD_H

#include <QThread>
#include "gfsdkqml.h"

class GfThread : public QThread
{
public:
    explicit GfThread(std::shared_ptr<gfSdkQml> listener,std::shared_ptr<gf::Hub> pHub, QObject *parent = nullptr);
    ~GfThread();

public:
    void stop();

protected:
    void run();

private:
    std::shared_ptr<gfSdkQml> m_listenerPtr;
    std::shared_ptr<gf::Hub> m_hub;

};

#endif // GFTHREAD_H
