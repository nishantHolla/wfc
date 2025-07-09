#include "wfc.h"
#include "wfc_sdl_utils.h"
#include "wfc_parser.h"
#include "wfc_log.h"

namespace fs = std::filesystem;

void wfc::check_config_file(const fs::path& p_config_path) {

  /// Check if the path exists

  if (!fs::exists(p_config_path)) {
    char msg[500];
    sprintf(msg, "Could not open config file at path %s (current directory: %s)", p_config_path.c_str(), fs::current_path().c_str());
    throw std::runtime_error(msg);
  }

  /// Check if the path is a json file

  if (!(fs::is_regular_file(p_config_path) &&
           p_config_path.has_extension() &&
           p_config_path.extension() == ".json")) {
    char msg[100];
    sprintf(msg, "Config file at %s is not a json file", p_config_path.c_str());
    throw std::runtime_error(msg);
  }
}

wfc::Canvas* wfc::init(const std::string& p_config_path) {
  wfc::Log::info("Initializing SDL...");
  wfc::init_sdl();

  wfc::Log::info("Initializing parser... ");
  wfc::Parser parser(p_config_path);

  wfc::Log::info("Parsing canvas config at " + p_config_path + "...");
  wfc::CanvasInfo canvas_info;
  parser.parse_canvas(canvas_info);
  wfc::Canvas* canvas = new wfc::Canvas(
    canvas_info.width,
    canvas_info.height,
    canvas_info.rows,
    canvas_info.columns
  );
  canvas->set_direction_type(canvas_info.direction_type);

  wfc::Log::info("Parsing groups config at " + p_config_path + "...");
  wfc::GroupInfo group_info;
  parser.parse_groups(group_info);

  wfc::Log::info("Parsing tiles config at " + p_config_path + "...");
  std::vector<wfc::TileInfo> tiles;
  parser.parse_tiles(tiles, group_info);

  wfc::Log::info("Adding parsed tiles to canvas...");
  for (wfc::TileInfo tile: tiles) {
    canvas->add_tile(tile.name, tile.path);
  }

  wfc::Log::info("Adding parsed rules to tiles...");
  for (wfc::TileInfo tile: tiles) {
    for (auto& item: tile.rules) {
      for (auto& t: item.second) {
        canvas->add_rule(tile.name, item.first, t);
      }
    }
  }

  wfc::Log::info("Parsing constraints at " + p_config_path + "...");
  wfc::ConstraintInfo constraints;
  parser.parse_constraints(constraints);

  wfc::Log::info("Adding constraints to canvas...");
  auto add_constraint = [canvas](wfc::Constraints dir, const std::unordered_set<std::string>& set){
    for (auto& tile : set) {
      canvas->add_constraint(dir, tile);
    }
  };

  add_constraint(wfc::Constraints::TOP, constraints.top);
  add_constraint(wfc::Constraints::TOP_RIGHT, constraints.top_right);
  add_constraint(wfc::Constraints::RIGHT, constraints.right);
  add_constraint(wfc::Constraints::BOTTOM_RIGHT, constraints.bottom_right);
  add_constraint(wfc::Constraints::BOTTOM, constraints.bottom);
  add_constraint(wfc::Constraints::BOTTOM_LEFT, constraints.bottom_left);
  add_constraint(wfc::Constraints::LEFT, constraints.left);
  add_constraint(wfc::Constraints::TOP_LEFT, constraints.top_left);

  for (auto& fixed : constraints.fixed) {
    for (const std::string& tile : fixed.tiles) {
      canvas->add_constraint(fixed.row, fixed.column, tile);
    }
  }

  wfc::Log::info("Clearing canvas buffer...");
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
