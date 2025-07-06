#include "wfc.h"
#include "wfc_sdl_utils.h"
#include "wfc_parser.h"

void wfc::check_config_file(const std::filesystem::path& p_config_path) {
  if (!std::filesystem::exists(p_config_path)) {
    char msg[500];
    sprintf(msg, "Could not open config file at path %s (current directory: %s)", p_config_path.c_str(), std::filesystem::current_path().c_str());
    throw std::runtime_error(msg);
  }

  if (!(std::filesystem::is_regular_file(p_config_path) &&
           p_config_path.has_extension() &&
           p_config_path.extension() == ".json")) {
    char msg[100];
    sprintf(msg, "Config file at %s is not a json file", p_config_path.c_str());
    throw std::runtime_error(msg);
  }
}

wfc::Canvas* wfc::init(const std::string& p_config_path) {
  wfc::init_sdl();
  wfc::Parser parser(p_config_path);

  wfc::CanvasInfo canvas_info;
  parser.parse_canvas(canvas_info);
  wfc::Canvas* canvas = new wfc::Canvas(
    canvas_info.width,
    canvas_info.height,
    canvas_info.rows,
    canvas_info.columns
  );
  canvas->set_direction_type(canvas_info.direction_type);

  std::vector<wfc::TileInfo> tiles;
  parser.parse_tiles(tiles);

  for (wfc::TileInfo tile: tiles) {
    canvas->add_tile(tile.name, tile.path);
  }

  for (wfc::TileInfo tile: tiles) {
    for (auto& item: tile.rules) {
      for (auto& t: item.second) {
        canvas->add_rule(tile.name, item.first, t);
      }
    }
  }

  canvas->reset();
  return canvas;
}

void wfc::poll_events(bool& p_running, wfc::Canvas* canvas) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      p_running = false;
    }
    else if (e.type == SDL_KEYDOWN) {
      if (e.key.keysym.sym == SDLK_r) {
        canvas->reset();
      }
    }
  }
}

void wfc::free(wfc::Canvas* canvas) {
  delete canvas;
  wfc::free_sdl();
}
