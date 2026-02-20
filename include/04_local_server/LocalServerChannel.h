/**
 * @file LocalServerChannel.h
 * @brief Local server channel: receives requests and sends responses. Uses WiFi status
 * to decide when to restart the server; no explicit Start/Stop.
 */

#ifndef LOCAL_SERVER_LOCAL_SERVER_CHANNEL_H
#define LOCAL_SERVER_LOCAL_SERVER_CHANNEL_H

#include "ILocalServerChannel.h"
#include <IServer.h>
#include <ServerProvider.h>
#include <ILogger.h>
#include <IWiFiConnectionStatusProvider.h>
#include <IHttpResponseQueue.h>
#include <IHttpResponse.h>

/* @Component */
class LocalServerChannel final : public ILocalServerChannel {

    /* @Autowired */
    Private IWiFiConnectionStatusProviderPtr wiFiStatusProvider;
    /* @Autowired */
    Private ILoggerPtr logger;
    /* @Autowired */
    Private IHttpResponseQueuePtr responseQueue;
    /* @Autowired */
    Private IServerPtr server_;

    /** Last known network connection id; when it changes we restart the server. */
    Private ULong lastNetworkConnectionId_{0};

    /**
     * Returns true if we may send/receive: network connected (id != 0), server restarted if id changed, server valid.
     */
    Private Bool PreCheck() {
        if (server_ == nullptr || wiFiStatusProvider == nullptr) return false;
        ULong networkConnectionId = wiFiStatusProvider->GetNetworkConnectionId();
        if (networkConnectionId == 0) return false;
        if (networkConnectionId != lastNetworkConnectionId_) {
            server_->Stop();
            server_->Start(DEFAULT_SERVER_PORT);
            lastNetworkConnectionId_ = networkConnectionId;
        }
        if (!server_->IsRunning()) return false;
        return true;
    }

    Public LocalServerChannel()
        : server_(ServerProvider::GetSecondServer()) {}

    Public IHttpRequestPtr ProcessRequest() override {
        if (!PreCheck()) return nullptr;
        return server_->ReceiveMessage();
    }

     Public Bool ProcessResponse() override {
        if (!PreCheck()) return false;
        IHttpResponsePtr response = responseQueue->DequeueLocalResponse();
        if (response == nullptr) return false;
        StdString requestId = response->GetRequestId();
        if (requestId.empty()) return false;
        StdString message = response->ToHttpString();
        return server_->SendMessage(requestId, message);
    }

};

#endif  // LOCAL_SERVER_LOCAL_SERVER_CHANNEL_H
