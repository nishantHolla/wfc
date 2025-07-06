#include "wfc.h"
#include "wfc_sdl_utils.h"
#include "wfc_parser.h"

void wfc::check_config_file(const std::filesystem::path& p_config_path) {
  if (!std::filesystem::exists(p_config_path)) {
    char msg[100];
    sprintf(msg, "Could not open config file at path %s", p_config_path.c_str());
    throw std::runtime_error(msg);
  }

  if (!(std::filesystem::is_regular_file(p_config_path) &&
           p_config_path.has_extension() &&
           p_config_path.extension() == ".json")) {
    char msg[100];
    sprintf(msg, "Config file at %s is not a json file", p_config_path.c_str());
    throw std::runtime_error(msg);
  }

  std::filesystem::current_path(p_config_path.parent_path());
}

wfc::Canvas* wfc::init(const std::string& p_config_path) {
  wfc::init_sdl();
  wfc::Parser parser(p_config_path);

  size_t width, height, rows, cols;
  parser.parse_canvas(width, height, rows, cols);
  wfc::Canvas* canvas = new wfc::Canvas(width, height, rows, cols);

  wfc::DirectionType direction_type;
  std::string base_path;
  parser.parse_settings(direction_type, base_path);
  canvas->set_direction_type(direction_type);
  canvas->set_base_path(base_path);

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
