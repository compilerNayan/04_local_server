/**
 * @file ILocalServerChannel.h
 * @brief Interface for the local server channel: receive requests and send responses.
 */

#ifndef LOCAL_SERVER_ILOCAL_SERVER_CHANNEL_H
#define LOCAL_SERVER_ILOCAL_SERVER_CHANNEL_H

#include <StandardDefines.h>
#include <IHttpRequest.h>

DefineStandardPointers(ILocalServerChannel)
class ILocalServerChannel {
    Public virtual ~ILocalServerChannel() = default;

    /**
     * Receive one request from the local server, if available.
     * @return IHttpRequestPtr if a request was received, nullptr otherwise
     */
    Public virtual IHttpRequestPtr ProcessRequest() = 0;

    /**
     * Send a response to the client identified by requestId.
     * @param requestId Unique request ID (e.g. from the received IHttpRequest)
     * @param message Response body to send
     * @return true if sent successfully, false otherwise
     */
    Public virtual Bool ProcessResponse() = 0;
};

#endif  // LOCAL_SERVER_ILOCAL_SERVER_CHANNEL_H
