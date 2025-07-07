#include "wfc_sdl_utils.h"

#include <SDL2/SDL_image.h>
#include <stdexcept>

#include <SDL2/SDL.h>

void wfc::init_sdl() {

  /// Initialize SDL Video

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw std::runtime_error("SDL Init failed");
  }

  /// Initialize SDL Image

  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
    throw std::runtime_error("SDL Img Init failed");
  }
}

void wfc::free_sdl() {

  /// Deinitialize SDL Image and SDL Video

  IMG_Quit();
  SDL_Quit();
}
