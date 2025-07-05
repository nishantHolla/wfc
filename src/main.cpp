#include "wfc.h"
#include "wfc_canvas.h"
#include "wfc_log.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    wfc::Log::error("Usage: ./wfc </path/to/config.json>");
    exit(1);
  }

  bool running = true;
  wfc::Canvas* canvas = wfc::init(argv[1]);

  while (running) {
    wfc::poll_events(running);
    canvas->render();
  }

  wfc::free(canvas);
  return 0;
}
