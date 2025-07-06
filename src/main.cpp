#include "wfc.h"
#include "wfc_canvas.h"
#include "wfc_log.h"
#include "wfc_random.h"

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <filesystem>

const uint32_t TARGET_FPS = 60;
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS;
uint32_t COLLAPSE_INTERVAL = 10;
int32_t SEED = -1;
const std::string USAGE = "Usage: ./wfc [-s seed] [-t delay_time_in_ms] </path/to/config.json>";

int main(int argc, char* argv[]) {
  if (argc < 2) {
    wfc::Log::info(USAGE);
    exit(0);
  }

  int opt;
  while ((opt = getopt(argc, argv, "s:t:")) != -1) {
    switch (opt) {
      case 's':
        SEED = std::atoi(optarg);
        break;

      case 't':
        COLLAPSE_INTERVAL = std::atoi(optarg);
        break;

      default:
        wfc::Log::error(USAGE);
        exit(1);
    }
  }

  wfc::check_config_file(argv[optind]);
  std::filesystem::path config_path = std::filesystem::path(argv[optind]).filename();

  if (SEED > 0) {
    wfc::Random::seed(SEED);
  }

  wfc::Canvas* canvas = wfc::init(config_path);
  Uint32 last_collapse_time = SDL_GetTicks();

  bool running = true;
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
