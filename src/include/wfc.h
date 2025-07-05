#ifndef WFC_H_
#define WFC_H_

#include "wfc_canvas.h"

#include <string>

namespace wfc {

/*
 * Creates a wfc::Canvas object by parsing the config at the given path
 * Also initializes SDL
 *
 * Params:
 *       String p_config_path: Path to the config file
 *
 * Returns:
 *        Pointer to the allocated canvas object
 */
wfc::Canvas* init(const std::string& p_config_path);

/*
 * Poll SDL events
 *
 * Params:
 *       bool p_running: Reference to the bool that controls the main loop
 */
void poll_events(bool& p_running);


/*
 * Destroys the given canvas objec
 * Also deinitializes SDL
 *
 * Params:
 *       wfc::Canvas* canvas: Pointer to the canvas object to destroy
 */
void free(wfc::Canvas* canvas);

};

#endif // !WFC_H_
