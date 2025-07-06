#ifndef WFC_H_
#define WFC_H_

#include "wfc_canvas.h"

#include <string>

namespace wfc {

/*
 * Check the health of the config file and performs necessary actions to begin processing the file
 *
 * Params:
 *       Path p_config_path: Path to the config file
 *
 *
 * Throws:
 *       If config file does not exists or not a json file
 */
void check_config_file(const std::filesystem::path& p_config_path);

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
 *       bool        p_running: Reference to the bool that controls the main loop
 *       wfc::Canvas p_canvas : Current canvas
 */
void poll_events(bool& p_running, wfc::Canvas* p_canvas);

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
