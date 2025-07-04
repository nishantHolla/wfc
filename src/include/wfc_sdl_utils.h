#ifndef  WFC_SDL_UTILS_H_
#define WFC_SDL_UTILS_H_

namespace wfc {

/*
 * Initialize video and image for SDL
 *
 * Throws:
 *       If SDL_Init fails
 *       If Img_Init fails
 */
void init_sdl();

/*
 * Deinitialize video for SDL
 */
void free_sdl();

}

#endif // !WFC_SDL_UTILS_H_
