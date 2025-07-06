#include "wfc.h"
#include "wfc_canvas.h"
#include "wfc_log.h"

#include <random>
#include <ctime>

const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;
const Uint32 COLLAPSE_INTERVAL = 10;

int main(int argc, char* argv[]) {
  std::srand(std::time(NULL));

  if (argc != 2) {
    wfc::Log::error("Usage: ./wfc </path/to/config.json>");
    exit(1);
  }

  bool running = true;
  wfc::Canvas* canvas = wfc::init(argv[1]);

  Uint32 last_collapse_time = SDL_GetTicks();

  while (running) {
    Uint32 frame_start = SDL_GetTicks();

    wfc::poll_events(running, canvas);

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_collapse_time >= COLLAPSE_INTERVAL) {
      if (!canvas->collapse_next()) {
        canvas->reset();
      }
      last_collapse_time = current_time;
    }

    canvas->render();

    Uint32 frame_time = SDL_GetTicks() - frame_start;
    if (frame_time < FRAME_DELAY) {
      SDL_Delay(FRAME_DELAY - frame_time);
    }
  }

  wfc::free(canvas);
  return 0;
}
