
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

#if defined(ESP32)
#include "esp32-hal.h"
#endif

#include "config.h"
#include "MemoryManager.h"
#include "HardwareSerial.h"
#include "Buffer.h"
#include "stdExtension.h"
#include "WebSocket.h"

namespace DevM {

struct DeviceManager {
  enum PrinterStatus {
    Offline,
    Idle,
    Busy,
    CommandSuccess,
    ColdExtrusion,
    Error,
  };

  bool WriteAllowed = true;
  uint32_t lastResponse = 0;
  size_t failedCommands = 0;

  HardwareSerial* serial = nullptr;

  RuntimeBuffer<size_t>* commandSizebuffer = nullptr;

  DeviceManager() {}

  size_t availableInBuffer() {
    size_t amount = 0;
    commandSizebuffer->finalize();
    for (int i = 0; i < commandSizebuffer->used(); i++) {
      amount += (*commandSizebuffer)[i];
    }
    return flashMemory::get<FLASH_MEMORY::PRINTER_BUFFER_SIZE>() - amount;
  }

  size_t spacesLeftInBuffer() {
    return commandSizebuffer->available();
  }

  void begin() {
    FLASH_MEMORY::DevSerialConfig serialConfig = flashMemory::get<FLASH_MEMORY::DEVSERIAL>();
    commandSizebuffer = new RuntimeBuffer<size_t>(flashMemory::get<FLASH_MEMORY::PRINTER_COMMAND_SIZE>());

    if (serialConfig.serial == 1)
      serial = &Serial;
      
    #ifdef ESP32
    else if (serialConfig.serial == 2)
      serial = &Serial2;
    #endif

    if (serialConfig.custom) {
      Serial.println("custom");

      //serial->begin(serialConfig.baudRate, serialConfig.config, serialConfig.rx, serialConfig.tx);
    } else {
      //serial->begin(serialConfig.baudRate);
    }
  }

  size_t print(const String& data) {
    commandSizebuffer->finalize();
    if (spacesLeftInBuffer() >= 1 && availableInBuffer() >= data.length()) {
      auto len = serial->print(data);
      commandSizebuffer->write(len);
      return len;
    }
    return 0;
  }

  size_t write(const uint8_t* c, const size_t size) {
    commandSizebuffer->finalize();
    if (spacesLeftInBuffer() >= 1 && availableInBuffer() >= size) {
      auto len = serial->write(c, size);
      commandSizebuffer->write(len);
      return len;
    }
    return 0;
  }

  PrinterStatus read() {

    if (!serial->available()) {
      if (millis() - lastResponse >= flashMemory::get<FLASH_MEMORY::PRINTER_TIMEOUT>()) {
        return Offline;
      }
      return Idle;
    }

    while (serial->available()) {
      String data;
      data = serial->readStringUntil('\n');
      WSM.broadcastAllTXT(data);
      if (data.isEmpty()) continue;
      if (data.startsWith("ok")) {
        commandSizebuffer->read();
        commandSizebuffer->finalize();
        return CommandSuccess;
      } else if (data.startsWith("echo: cold extrusion prevented")) {
        return ColdExtrusion;
      } else if (data.startsWith("echo:Unknown command:")) {
        commandSizebuffer->read();
        commandSizebuffer->finalize();
        return Error;
      } else if (data.startsWith("echo:busy: processing")) {
        return Busy;
      }
    }

    return Idle;
  }
};

struct GCodeManager {
  String filename;
  bool shutdownProtection = false;

  FsFile file;
  FsFile recoveryFile;

  uint64_t currentStep = 0;
  uint64_t steps = 0;

  DeviceManager* deviceManager = nullptr;


  enum PrintState {
    NOT_PRINTING,
    INITIALIZING,
    PRINTING,
    END,
    PAUSE,
    STOP,
    EMS,
  };

  PrintState printState = NOT_PRINTING;

  inline static bool isCommand(const String& data) {
    return data.startsWith("M") || data.startsWith("G");
  }

  void attachDeviceManager(DeviceManager* dm) {
    deviceManager = dm;
  }

  bool trimLine(String* line) {
    if (line->startsWith(COMMENTCHAR)) return false;
    int delimiterIndex = line->indexOf(COMMENTCHAR);
    if (delimiterIndex != -1) {
      *line = line->substring(0, delimiterIndex);
    }
    return !(line->isEmpty() || !isCommand(*line));
  }

  bool readLineFromSdCard(String* output, uint* size) {
    *output = "";
    while (true) {
      int d = file.read();
      if (d == -1) {
        return false;
      }
      if ((char)d == '\n') {
        return true;
      }
      *output += (char)d;
      *size = *size + 1;
    }
  }

  bool readLine(String* line, uint* size) {
    if (printState == PRINTING)
      return readLineFromSdCard(line, size);
    else if (printState == PAUSE)
      return false;
    else if (printState == STOP) {
      *line = String(END_COMMANDS[currentStep]);
      *size = (*line).length();
      return true;
    } else if (printState == EMS) {
      *line = String(EMS_COMMANDS[currentStep]);
      *size = (*line).length();
      return true;
    }
    return false;
  }

  void startPrint(String fn, bool sP = false, uint64_t cS = 0) {
    filename = fn;
    shutdownProtection = sP;
    currentStep = cS;

    file = SDM::openFile(filename);

    steps = 0;

    printState = INITIALIZING;
    Serial.println("print started");
  }

  String initLine;

  void initPrint() {
    const size_t BUFFER_SIZE = 512;
    static char buffer[BUFFER_SIZE];
    static size_t bufferPos = 0;
    static size_t bufferLength = 0;
    static char stage = 1;

    static size_t readAmount;

    if (bufferPos >= bufferLength) {
      if (stage == 1) {
        auto SDRequest = std::make_unique<SDM::HANDLER::GCodeInit>(stage, file, BUFFER_SIZE, bufferPos, bufferLength, buffer);
        SDM::HANDLER::SDHandlerManager.addHandler(std::move(SDRequest));
        stage = 2;
      } else if (stage == 2) {

      } else if (stage == 3) {
        if (bufferLength == 0) {
          file.seek(0);
          printState = PRINTING;
          Serial.println("File read completed. Total steps: " + String(steps));
          return;
        }
        stage == 1;
        bufferPos = 0;
      }
    }

    // Process current buffer until end
    while (bufferPos < bufferLength) {
      char c = buffer[bufferPos++];
      initLine += c;
      readAmount++;

      if (c == '\n') {
        if (isCommand(initLine)) {
          steps++;
        }
        initLine = "";
      }
    }
    stage = 1;
  }




  bool stop() {
    if (printState == PRINTING || printState == PAUSE) {
      steps = sizeof(END_COMMANDS) / sizeof(END_COMMANDS[0]);
      currentStep = 0;
      printState = STOP;
      return true;
    }
    return false;
  }

  void ems() {
    steps = sizeof(EMS_COMMANDS) / sizeof(EMS_COMMANDS[0]);
    currentStep = 0;
    printState = EMS;
  }

  PrintState pause() {
    if (printState == PRINTING) {
      printState = PAUSE;
      return printState;
    } else if (printState == PAUSE) {
      printState = PRINTING;
      return printState;
    }
    return PAUSE;
  }

  void printFinished() {
    printState = NOT_PRINTING;
    currentStep = 0;
    steps = 0;
  }

  uint64_t last = 324324;

  void Handle() {
    if (printState == INITIALIZING) {
      initPrint();
      return;
    }

    if (printState == NOT_PRINTING) {
      return;
    };

    if (deviceManager->spacesLeftInBuffer() < 1) {
      //Serial.printf("no spaces left in buffer: %d\n", deviceManager->spacesLeftInBuffer());
      return;
    }

    if (deviceManager->availableInBuffer() < 1) {
      //Serial.println("printers buffer is full: ");
      return;
    }

    while (deviceManager->spacesLeftInBuffer() > 0) {
      String line;
      bool readState = false;

      uint size = 0;

      readState = readLine(&line, &size);

      if (!readState) {
        printFinished();
        Serial.println("print finished");
        return;
      }

      if (!trimLine(&line)) continue;

      if (!isCommand(line)) continue;

      line += '\n';

      if (!(deviceManager->availableInBuffer() >= line.length())) {
        file.seek(file.curPosition() - size - 1);
        return;
      }

      deviceManager->print(line);

      currentStep++;
      WSM.broadcastAllTXT("commands completed: " + String(currentStep) + "/"+String(steps));
      //Serial.printf("spaces left: %d, available bytes: %d, steps done: %d\n", deviceManager->spacesLeftInBuffer(), deviceManager->availableInBuffer(), currentStep);

      if (currentStep >= steps) {
        printFinished();
        return;
      }
    }
  }
};

}
