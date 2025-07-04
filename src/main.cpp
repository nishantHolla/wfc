#include "wfc_canvas.h"
#include "wfc_sdl_utils.h"

void app() {
  wfc::Canvas canvas(640, 640, 4, 4);
  canvas.set_base_path("tiles");
  canvas.set_direction_type(wfc::DirectionType::QUAD_DIRECTIONS);

  canvas.add_tile("up", "up.png");
  canvas.add_tile("right", "right.png");
  canvas.add_tile("down", "down.png");
  canvas.add_tile("left", "left.png");
  canvas.add_tile("blank", "blank.png");

  SDL_Event e;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    canvas.render();
  }
}

int main(void) {
  wfc::init_sdl();
  app();
  wfc::free_sdl();
  return 0;
}
