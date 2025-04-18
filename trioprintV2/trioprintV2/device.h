#include "esp32-hal.h"
#pragma once

#include "config.h"
#include "MemoryManager.h"
#include "HardwareSerial.h"
#include "Buffer.h"

namespace DevM {

struct DeviceManager {
  enum PrinterStatus {
    Offline,
    Idle,
    Busy,
    Printing,
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
    commandSizebuffer = new RuntimeBuffer<size_t>(flashMemory::get<FLASH_MEMORY::PRINTER_BUFFER_SIZE>());
    if (serialConfig.serial == 1)
      serial = &Serial;
    else if (serialConfig.serial == 2)
      serial = &Serial1;
    else if (serialConfig.serial == 3)
      serial = &Serial2;

    if (serialConfig.custom) {
      serial->begin(serialConfig.baud_rate, serialConfig.config, serialConfig.rx, serialConfig.tx);
    } else {
      serial->begin(serialConfig.baud_rate);
    }
  }

  size_t print(const String& data) {
    commandSizebuffer->finalize();
    if (spacesLeftInBuffer() >= 1 && availableInBuffer() >= data.length()) {
      commandSizebuffer->write(data.length());
      return serial->print(data);
    }
    return 0;
  }

  size_t write(const uint8_t* c, const size_t size) {
    commandSizebuffer->finalize();
    if (spacesLeftInBuffer() >= 1 && availableInBuffer() >= size) {
      commandSizebuffer->write(size);
      return serial->write(c, size);
    }
    return 0;
  }

  PrinterStatus read() {
    String data;

    if (!serial->available()) {
      if (millis() - lastResponse >= flashMemory::get<FLASH_MEMORY::PRINTER_TIMEOUT>()) {
        return Offline;
      }
      return Idle;
    }

    while (serial->available()) {
      data = serial->readStringUntil('\n');
      if (data.isEmpty()) continue;
      if (data.startsWith("ok")) {
        commandSizebuffer->read();
        commandSizebuffer->finalize();
        return Printing;
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
    PRINTING,
    PAUSE,
    END,
    STOP,
    EMS,
  };

  PrintState printState = NOT_PRINTING;

  inline static bool isCommand(const String& data) {
    return data.startsWith("M") || data.startsWith("G");
  }

  bool trimLine(String* line) {
    if (line->startsWith(COMMENTCHAR)) return false;
    int delimiterIndex = line->indexOf(COMMENTCHAR);
    if (delimiterIndex != -1) {
      *line = line->substring(0, delimiterIndex);
    }
    return !(line->isEmpty() || !isCommand(*line));
  }

  bool readLineFromSdCard(String* output) {
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
    }
  }

  bool readLine(String* line) {
    return readLineFromSdCard(line);
  }

  void startPrint(DeviceManager* dm, String fn, bool sP = false, uint64_t cS = 0) {
    deviceManager = dm;
    filename = fn;
    shutdownProtection = sP;
    currentStep = cS;

    file = SDM::openFile(filename);

    char c;
    String ln;
    steps = 0;

    while (file.read(&c, 1) == 1) {
      ln += c;
      if (c == '\n')
        if (isCommand(ln))
          steps++;
    }
    file.seek(0);
    printState = PRINTING;
  }

  void stop() {
    steps = sizeof(END_COMMANDS) / sizeof(END_COMMANDS[0]);
    currentStep = 0;
    printState = STOP;
  }

  void ems() {
    steps = sizeof(EMS_COMMANDS) / sizeof(EMS_COMMANDS[0]);
    currentStep = 0;
    printState = EMS;
  }

  void printFinished() {
    printState = NOT_PRINTING;
    currentStep = 0;
    steps = 0;
  }

  void Handle() {
    if (printState == NOT_PRINTING) return;

    if (deviceManager->spacesLeftInBuffer() < 1) return;

    if (deviceManager->availableInBuffer() == 0) return;


    while (deviceManager->spacesLeftInBuffer() > 0) {
      String line;
      bool readState = false;

      readState = readLine(&line);

      if (!readState) {
        printFinished();
        return;
      }

      if (!trimLine(&line)) continue;

      if (!isCommand(line)) continue;

      line += '\n';

      if (!(deviceManager->availableInBuffer() >= line.length())) return;

      deviceManager->print(line);
      currentStep++;

      if (currentStep >= steps) {
        printFinished();
        return;
      }
    }
  }
};

}  // namespace DevM
