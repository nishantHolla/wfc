#include "wfc_sdl_utils.h"

#include <SDL2/SDL_image.h>
#include <stdexcept>

#include <SDL2/SDL.h>

void wfc::init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw std::runtime_error("SDL Init failed");
  }

  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
    throw std::runtime_error("SDL Img Init failed");
  }
}

void wfc::free_sdl() {
  IMG_Quit();
  SDL_Quit();
}
