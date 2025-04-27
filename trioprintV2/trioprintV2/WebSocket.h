#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "MemoryManager.h"

struct WebSocketManager {

  AsyncWebServer server;
  AsyncWebSocket ws;

  WebSocketManager()
    : server(flashMemory::get<FLASH_MEMORY::WEB_SOCKET_PORT>()), ws("/ws") {
  }

  void onWebSocketEvent(AsyncWebSocket *server,
                        AsyncWebSocketClient *client,
                        AwsEventType type,
                        void *arg,
                        uint8_t *data,
                        size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
#if defined(DEBUG_SERIAL)
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
#endif
        break;
      case WS_EVT_DISCONNECT:
#if defined(DEBUG_SERIAL)
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
#endif
        break;
      case WS_EVT_DATA:
        {
          AwsFrameInfo *info = (AwsFrameInfo *)arg;
          if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            String msg = "";
            for (size_t i = 0; i < len; i++) {
              msg += (char)data[i];
            }
#if defined(DEBUG_SERIAL)
            Serial.printf("WebSocket data from client #%u: %s\n", client->id(), msg.c_str());
#endif
          }
          break;
        }
      default:
        break;
    }
  }

  void begin() {
    ws.onEvent([this](AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len) {
      this->onWebSocketEvent(server, client, type, arg, data, len);
    });

    Serial.printf("starting socket server, port: %d\n", flashMemory::get<FLASH_MEMORY::WEB_SOCKET_PORT>());
    server.addHandler(&ws);
    server.begin();
    Serial.println("socket server started");
  }

  bool broadcastAllTXT(String &data) {
    return ws.textAll(data);
  }

  bool broadcastAllTXT(const char *data) {
    return ws.textAll(data);
  }
};

WebSocketManager WSM;
