#pragma once

#include <ESPmDNS.h>
#include "config.h"
#include "MemoryManager.h"

namespace mDNS {
void begin() {
  if (!MDNS.begin(flashMemory::get<FLASH_MEMORY::WEB_NAME>())) {
    Serial.println("Error starting mDNS");
    return;
  }
  Serial.println("mDNS started: " + String(flashMemory::get<FLASH_MEMORY::M_DNS_NAME>()));
}
}
