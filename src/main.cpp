#include "wfc_canvas.h"
#include "wfc_log.h"
#include "wfc_sdl_utils.h"
#include "wfc_parser.h"

void app(const wfc::Parser& parser) {
  size_t width, height, rows, cols;
  parser.parse_canvas(width, height, rows, cols);
  wfc::Canvas canvas(width, height, rows, cols);

  bool quit = false;
  SDL_Event e;

  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) quit = true;
    }
    canvas.render();
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    wfc::Log::error("Usage: ./wfc </path/to/config.json>");
    exit(1);
  }

  wfc::init_sdl();
  wfc::Parser parser(argv[1]);
  app(parser);
  wfc::free_sdl();
  return 0;
}
