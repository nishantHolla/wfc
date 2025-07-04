#include "wfc_log.h"

#include <iostream>

int main(void) {
  wfc::Log::error("Hello, world.");
  wfc::Log::info("Hello, world.");
  wfc::Log::warn("Hello, world.");
  return 0;
}
