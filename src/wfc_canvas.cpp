#include "wfc_canvas.h"

#include <stdexcept>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

wfc::Canvas::Canvas(size_t p_width, size_t p_height, size_t p_rows, size_t p_columns) :
  width__(p_width),
  height__(p_height),
  rows__(p_rows),
  columns__(p_columns),
  tile_width__(width__ / rows__),
  tile_height__(height__ / columns__),
  base_path__(""),
  direction_type__(DirectionType::QUAD_DIRECTIONS) {

  window__ = SDL_CreateWindow("WFC",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              p_width,
                              p_height,
                              SDL_WINDOW_HIDDEN);

  if (!window__) {
    throw std::runtime_error("SDL Window creation failed.\n");
  }

  renderer__ = SDL_CreateRenderer(window__, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer__) {
    throw std::runtime_error("SDL Renderer creation failed.\n");
  }
}

wfc::Canvas::~Canvas() {
  for (auto& item : tiles__) {
    delete item.second;
  }

  SDL_DestroyRenderer(renderer__);
  SDL_DestroyWindow(window__);
}

void wfc::Canvas::set_base_path(const std::string& p_base_path) {
  base_path__ = p_base_path;
}

void wfc::Canvas::set_direction_type(const wfc::DirectionType p_dir_type) {
  direction_type__ = p_dir_type;
}

void wfc::Canvas::add_tile(const std::string& p_name, const std::string& p_path) {
  if (tiles__.find(p_name) != tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s already exists", p_name.c_str());
    throw std::runtime_error(msg);
  }

  fs::path abs_path = fs::absolute(base_path__ / p_path);
  tiles__[p_name] = new wfc::Tile(abs_path, renderer__);
}

void wfc::Canvas::add_rule(const std::string& p_for, wfc::Directions p_dir, const std::string& p_to) {
  if (tiles__.find(p_for) == tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s does not exist", p_for.c_str());
    throw std::runtime_error(msg);
  }

  if (tiles__.find(p_to) == tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s does not exist", p_to.c_str());
  }

  tiles__[p_for]->add_rule(p_dir, tiles__[p_to]);
}

void wfc::Canvas::add_rule(const std::string& p_for, wfc::Directions p_dir,
                           const std::initializer_list<const std::string> p_to) {
  if (tiles__.find(p_for) == tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s does not exist", p_for.c_str());
    throw std::runtime_error(msg);
  }

  for (const auto& tile : p_to) {
    if (tiles__.find(tile) == tiles__.end()) {
      char msg[100];
      sprintf(msg, "Tile with name %s does not exist", tile.c_str());
      throw std::runtime_error(msg);
    }

    tiles__[p_for]->add_rule(p_dir, tiles__[tile]);
  }
}
