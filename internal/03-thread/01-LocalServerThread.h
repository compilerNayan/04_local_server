/**
 * @file LocalServerThread.h
 * @brief IRunnable that runs ProcessRequestAndResponse on ILocalServerChannel in an infinite loop.
 */

#ifndef LOCAL_SERVER_LOCAL_SERVER_THREAD_H
#define LOCAL_SERVER_LOCAL_SERVER_THREAD_H

#include <IRunnable.h>
#include <Thread.h>
#include "../01-interface/01-ILocalServerChannel.h"

class LocalServerThread final : public IRunnable {

    /* @Autowired */
    Private ILocalServerChannelPtr localServerChannel;

    Public Void Run() override {
        Thread::Sleep(5000);
        while (true) {
            localServerChannel->ProcessRequestAndResponse();
            Thread::Sleep(100);
        }
    }
};

#endif  // LOCAL_SERVER_LOCAL_SERVER_THREAD_H
