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
                              SDL_WINDOW_SHOWN);

  if (!window__) {
    throw std::runtime_error("SDL Window creation failed.\n");
  }

  renderer__ = SDL_CreateRenderer(window__, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer__) {
    throw std::runtime_error("SDL Renderer creation failed.\n");
  }

  create_null_texture__();
  buffer__.resize(rows__ * columns__, nullptr);
}

wfc::Canvas::~Canvas() {
  for (auto& item : tiles__) {
    delete item.second;
  }

  buffer__.clear();
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

void wfc::Canvas::render() {
  SDL_RenderClear(renderer__);

  for (size_t row = 0; row < rows__; ++row) {
    for (size_t col = 0; col < columns__; ++col) {
      size_t idx = (row * columns__) + col;
      SDL_Rect pos_rect = {
        static_cast<int>((col * tile_width__)),
        static_cast<int>((row * tile_height__)),
        static_cast<int>(tile_width__),
        static_cast<int>(tile_height__)
      };

      wfc::Tile* tile = buffer__[idx];
      SDL_Texture* texture = tile ? tile->get_texture() : null_texture__;
      SDL_RenderCopy(renderer__, texture, nullptr, &pos_rect);
    }
  }

  SDL_RenderPresent(renderer__);
}

void wfc::Canvas::create_null_texture__() {
  null_texture__ = SDL_CreateTexture(renderer__, SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET, tile_width__, tile_height__);

  if (!null_texture__) {
    throw std::runtime_error("Failed to create null texture");
  }

  SDL_Texture* previous_target = SDL_GetRenderTarget(renderer__);

  SDL_SetRenderTarget(renderer__, null_texture__);
  SDL_SetRenderDrawColor(renderer__, 0, 0, 0, 0);
  SDL_RenderClear(renderer__);

  SDL_SetRenderDrawColor(renderer__, 255, 255, 255, 255);
  SDL_Rect full_rect = {0, 0, static_cast<int>(tile_width__), static_cast<int>(tile_height__)};
  SDL_RenderFillRect(renderer__, &full_rect);

  const int border_thickness = 2;
  SDL_SetRenderDrawColor(renderer__, 0, 0, 0, 255);
  SDL_Rect innerRect = {
    border_thickness,
    border_thickness,
    static_cast<int>(tile_width__ - 2 * border_thickness),
    static_cast<int>(tile_height__ - 2 * border_thickness)
  };
  SDL_RenderFillRect(renderer__, &innerRect);

  SDL_SetRenderTarget(renderer__, previous_target);
}
