#include "wfc.h"
#include "wfc_canvas.h"
#include "wfc_random.h"
#include "wfc_log.h"

#include <unistd.h>
#include <string>
#include <filesystem>

#define USAGE "Usage: ./wfc [-s seed] [-t delay_time_in_ms] [-o /path/to/output_image.png] </path/to/config.json>"

namespace fs = std::filesystem;

const uint32_t TARGET_FPS = 60;                  /// FPS to run the SDL window in
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS;  /// Frame Delay in ms for the set FSP
uint32_t COLLAPSE_INTERVAL = 10;                 /// Time between each collapse in ms
std::string OUTPUT_IMAGE_PATH = "";              /// Path to store the file image
int32_t SEED = -1;                               /// Seed to use for random number generation, random seed if -1

int main(int argc, char* argv[]) {
  /// Check if sufficient arguments

  if (argc < 2) {
    wfc::Log::info(USAGE);
    exit(0);
  }

  wfc::Log::info("Parsing arguments and flags...");

  /// Parse flags

  int opt;
  while ((opt = getopt(argc, argv, "s:t:o:")) != -1) {
    switch (opt) {
      case 's':
        SEED = std::atoi(optarg);
        break;

      case 't':
        COLLAPSE_INTERVAL = std::atoi(optarg);
        break;

      case 'o':
        OUTPUT_IMAGE_PATH = optarg;
        break;

      default:
        wfc::Log::error(USAGE);
        exit(1);
    }
  }

  /// Parse arguments

  if (OUTPUT_IMAGE_PATH != "" && !fs::exists(fs::absolute(OUTPUT_IMAGE_PATH).parent_path())) {
    std::stringstream msg("Path to image does not exist");
    wfc::Log::error(msg.str());
    exit(1);
  }

  OUTPUT_IMAGE_PATH = fs::absolute(OUTPUT_IMAGE_PATH);

  if (OUTPUT_IMAGE_PATH != "" && fs::exists(OUTPUT_IMAGE_PATH)) {
    std::stringstream msg("Image file ");
    msg << OUTPUT_IMAGE_PATH << " already exists";
    wfc::Log::error(msg.str());
    exit(1);
  }

  if (OUTPUT_IMAGE_PATH != "" && fs::path(OUTPUT_IMAGE_PATH).extension() != ".png") {
    wfc::Log::error("Output file must be a png");
    exit(1);
  }

  try {
    wfc::check_config_file(argv[optind]);
  }
  catch (const std::runtime_error& err) {
    wfc::Log::error(err.what());
    exit(1);
  }

  fs::path config_path(argv[optind]);
  fs::current_path(config_path.parent_path());
  fs::path config_file = config_path.filename();

  /// Set seed for random number generator

  if (SEED > 0) {
    wfc::Random::seed(SEED);
  }

  /// Initialize SDL and create canvas

  wfc::Canvas* canvas;
  try {
    canvas = wfc::init(config_file);
  }
  catch (const std::runtime_error& err) {
    wfc::Log::error(err.what());
    exit(1);
  }

  /// Start app loop

  Uint32 last_collapse_time = SDL_GetTicks();
  bool running = true;
  wfc::Log::info("Starging app loop...");

  while (running) {
    /// Pre-frame actions

    Uint32 frame_start = SDL_GetTicks();
    wfc::poll_events(running, canvas);

    /// Check if it is time to collapse tile

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_collapse_time >= COLLAPSE_INTERVAL) {
      if (!canvas->collapse_next()) {
        canvas->reset();
      }
      last_collapse_time = current_time;
    }

    /// Render frame

    canvas->render();

    /// Post-frame actions

    Uint32 frame_time = SDL_GetTicks() - frame_start;
    if (frame_time < FRAME_DELAY) {
      SDL_Delay(FRAME_DELAY - frame_time);
    }
  }

  if (OUTPUT_IMAGE_PATH != "") {
    canvas->save_image(OUTPUT_IMAGE_PATH);
  }

  wfc::Log::info("Shutting down app...");

  /// Free canvas and deinitialize SDL

  wfc::free(canvas);

  return 0;
}
