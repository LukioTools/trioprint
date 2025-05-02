#pragma once
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "MemoryManager.h"

struct WebSocketManager {

  AsyncWebServer *server;
  AsyncWebSocket *ws;

  WebSocketManager() {}

  void onWebSocketEvent(AsyncWebSocket *server,
                        AsyncWebSocketClient *client,
                        AwsEventType type,
                        void *arg,
                        uint8_t *data,
                        size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        {
          AwsFrameInfo *info = (AwsFrameInfo *)arg;
          if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            String msg = "";
            for (size_t i = 0; i < len; i++) {
              msg += (char)data[i];
            }
            Serial.printf("WebSocket data from client #%u: %s\n", client->id(), msg.c_str());
           }
          break;
        }
      default:
        break;
    }
  }

  void begin() {

    server = new AsyncWebServer(flashMemory::get<FLASH_MEMORY::WEB_SOCKET_PORT>());
    ws = new AsyncWebSocket("/");

    (*ws).onEvent([this](AsyncWebSocket *server,
                         AsyncWebSocketClient *client,
                         AwsEventType type,
                         void *arg,
                         uint8_t *data,
                         size_t len) {
      this->onWebSocketEvent(server, client, type, arg, data, len);
    });

    Serial.printf("starting socket server, port: %d\n", flashMemory::get<FLASH_MEMORY::WEB_SOCKET_PORT>());
    (*server).addHandler(ws);
    (*server).begin();
    Serial.println("socket server started");
  }

  bool broadcastAllTXT(String &data) {
    return (*ws).textAll(data);
  }

  bool broadcastAllTXT(const char *data) {
    return (*ws).textAll(data);
  }
};

WebSocketManager WSM;
