
/*

 Copyright (c) <2025> <Vili Kervinen>
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once

#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

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

  bool broadcastAllTXT(const String &data) {
    return (*ws).textAll(data);
  }

  bool broadcastAllTXT(const char *data) {
    return (*ws).textAll(data);
  }
};

WebSocketManager WSM;
