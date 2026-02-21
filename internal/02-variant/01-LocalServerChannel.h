/**
 * @file LocalServerChannel.h
 * @brief Local server channel: receives requests and sends responses. Uses WiFi status
 * to decide when to restart the server; no explicit Start/Stop.
 */

#ifndef LOCAL_SERVER_LOCAL_SERVER_CHANNEL_H
#define LOCAL_SERVER_LOCAL_SERVER_CHANNEL_H

#include <IServer.h>
#include <ServerProvider.h>
#include <ILogger.h>
#include <IWiFiConnectionStatusProvider.h>
#include <IHttpRequestQueue.h>
#include <IHttpResponseQueue.h>
#include <IHttpResponse.h>
#include <string>

#include "../01-interface/01-ILocalServerChannel.h"

/* @Component */
class LocalServerChannel final : public ILocalServerChannel {

    /* @Autowired */
    Private IWiFiConnectionStatusProviderPtr wiFiStatusProvider;
    /* @Autowired */
    Private ILoggerPtr logger;
    /* @Autowired */
    Private IHttpRequestQueuePtr requestQueue;
    /* @Autowired */
    Private IHttpResponseQueuePtr responseQueue;

    Private IServerPtr server_;

    /** Last known network connection id; when it changes we restart the server. */
    Private ULong lastNetworkConnectionId_{0};

    /**
     * Returns true if we may send/receive: network connected (id != 0), server restarted if id changed, server valid.
     */
    Private Bool PreCheck() {
        if (server_ == nullptr) {
            if (logger) logger->Warning(Tag::Untagged, StdString("[LocalServerChannel] PreCheck skip: server is null"));
            return false;
        }
        if (wiFiStatusProvider == nullptr) {
            if (logger) logger->Warning(Tag::Untagged, StdString("[LocalServerChannel] PreCheck skip: wiFiStatusProvider is null"));
            return false;
        }
        ULong networkConnectionId = wiFiStatusProvider->GetNetworkConnectionId();
        if (networkConnectionId == 0) {
            if (logger) logger->Info(Tag::Untagged, StdString("[LocalServerChannel] PreCheck skip: no network (connection id 0)"));
            return false;
        }
        if (networkConnectionId != lastNetworkConnectionId_) {
            if (logger) logger->Info(Tag::Untagged, StdString("[LocalServerChannel] Restarting server: connection id changed ") + std::to_string(lastNetworkConnectionId_) + " -> " + std::to_string(networkConnectionId));
            server_->Stop();
            server_->Start(DEFAULT_SERVER_PORT);
            lastNetworkConnectionId_ = networkConnectionId;
        }
        if (!server_->IsRunning()) {
            if (logger) logger->Warning(Tag::Untagged, StdString("[LocalServerChannel] PreCheck skip: server not running"));
            return false;
        }
        return true;
    }

    Public LocalServerChannel()
        : server_(ServerProvider::GetSecondServer()) {
        if (logger) {
            if (server_)
                logger->Info(Tag::Untagged, StdString("[LocalServerChannel] Created with server from ServerProvider"));
            else
                logger->Error(Tag::Untagged, StdString("[LocalServerChannel] Created but server is null (no server registered?)"));
        }
    }

    Public Bool ProcessRequest() override {
        if (!PreCheck()) return false;
        Val request = server_->ReceiveMessage();
        if (request == nullptr) return false;
        requestQueue->EnqueueRequest(request);
        if (logger) logger->Info(Tag::Untagged, StdString("[LocalServerChannel] Request received and enqueued"));
        return true;
    }

     Public Bool ProcessResponse() override {
        if (!PreCheck()) return false;
        IHttpResponsePtr response = responseQueue->DequeueLocalResponse();
        if (response == nullptr) return false;
        StdString requestId = response->GetRequestId();
        if (requestId.empty()) {
            if (logger) logger->Warning(Tag::Untagged, StdString("[LocalServerChannel] ProcessResponse skip: response has empty request id"));
            return false;
        }
        StdString message = response->ToHttpString();
        Bool sent = server_->SendMessage(requestId, message);
        if (logger && sent) logger->Info(Tag::Untagged, StdString("[LocalServerChannel] Response sent for request id ") + requestId);
        return sent;
    }

    Public Bool ProcessRequestAndResponse() override {
        Bool req = ProcessRequest();
        Bool rsp = ProcessResponse();
        if (logger && (!req || !rsp))
            logger->Info(Tag::Untagged, StdString("[LocalServerChannel] Cycle: request=") + (req ? "ok" : "skip") + " response=" + (rsp ? "ok" : "skip"));
        return req && rsp;
    }

};

#endif  // LOCAL_SERVER_LOCAL_SERVER_CHANNEL_H
