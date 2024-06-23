#pragma once
#include <WebSocketsServer.h>
#include "config.h"
#ifndef WEB_SOCKET_PORT
#define WEB_SOCKET_PORT 81
#endif
namespace WebSocketW {
    WebSocketsServer webSocket = WebSocketsServer(WEB_SOCKET_PORT);

    void begin(){
        webSocket.begin();
        webSocket.onEvent(webSocketEvent)
    }

    void Handle(){
        webSocket.loop();
    }

    bool brodcastAllTXT(String data){
        return webSocket.broadcastTXT(data)
    }

    bool brodcastAllTXT(const char*& data){
        return webSocket.broadcastTXT(data)
    }


    void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
        if (type == WStype_DISCONNECTED) {
            Serial.printf("[%u] Disconnected!\n", num);
        } else if (type == WStype_CONNECTED) {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %s\n", num, ip.toString().c_str());
        } else if (type == WStype_TEXT) {
            Serial.printf("[%u] Text: %s\n", num, payload);
        }
    }

}