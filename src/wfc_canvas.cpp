#include "wfc_canvas.h"
#include "wfc_utils.h"
#include "wfc_random.h"

#include <stdexcept>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

wfc::Canvas::Canvas(size_t p_width, size_t p_height, size_t p_rows, size_t p_columns) :
  width__(p_width),
  height__(p_height),
  rows__(p_rows),
  columns__(p_columns),
  tile_width__(width__ / rows__),
  tile_height__(height__ / columns__),
  direction_type__(DirectionType::QUAD_DIRECTIONS) {

  /// Create SDL window and renderer for the canvas

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

  /// Initialize canvas values

  create_null_texture__();
  buffer__.resize(rows__ * columns__);
}

wfc::Canvas::~Canvas() {

  /// Free all tiles in the canvas

  for (auto& item : tiles__) {
    delete item.second;
  }

  /// Deinitialize SDL and canvas values

  buffer__.clear();
  SDL_DestroyRenderer(renderer__);
  SDL_DestroyWindow(window__);
}

void wfc::Canvas::set_direction_type(const wfc::DirectionType p_dir_type) {
  direction_type__ = p_dir_type;
}

void wfc::Canvas::add_tile(const std::string& p_name, const std::string& p_path) {

  /// Check if tile is already known to the canvas

  if (tiles__.find(p_name) != tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s already exists", p_name.c_str());
    throw std::runtime_error(msg);
  }

  /// Add tile to the canvas

  fs::path abs_path = fs::absolute(p_path);
  tiles__[p_name] = new wfc::Tile(abs_path, renderer__);
}

void wfc::Canvas::add_rule(const std::string& p_for, wfc::Directions p_dir, const std::string& p_to) {

  /// Check if the source tile is not known to the canvas

  if (tiles__.find(p_for) == tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s does not exist", p_for.c_str());
    throw std::runtime_error(msg);
  }

  /// Check if the destination tile is not known to the canvas

  if (tiles__.find(p_to) == tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s does not exist", p_to.c_str());
  }

  /// Add rule to the tile

  tiles__[p_for]->add_rule(p_dir, tiles__[p_to]);
}

void wfc::Canvas::add_constraint(wfc::Constraints p_cons, const std::string& p_tile) {

  /// Check if the source tile is not known to the canvas

  if (tiles__.find(p_tile) == tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s does not exist", p_tile.c_str());
    throw std::runtime_error(msg);
  }

  /// Add the constraint to the canvas

  constraints__.others[p_cons].insert(tiles__[p_tile]);
}

void wfc::Canvas::add_constraint(size_t x, size_t y, const std::string& p_tile) {

  /// Check if the source tile is not known to the canvas

  if (tiles__.find(p_tile) == tiles__.end()) {
    char msg[100];
    sprintf(msg, "Tile with name %s does not exist", p_tile.c_str());
    throw std::runtime_error(msg);
  }

  /// Check if the coordinate are out of bound

  size_t idx = y * columns__ + x;
  if (idx >= buffer__.size()) {
    char msg[100];
    sprintf(msg, "Coordninate %zu, %zu is out of bound", x, y);
    throw std::runtime_error(msg);
  }

  /// Add the constraint to the canvas

  constraints__.fixed[idx].insert(tiles__[p_tile]);
}

void wfc::Canvas::add_rule(const std::string& p_for, wfc::Directions p_dir, const std::vector<std::string>& p_to) {

  /// Call add_rule for each tile in the list

  for (const std::string& tile : p_to) {
    add_rule(p_for, p_dir, tile);
  }
}

void wfc::Canvas::reset() {

  /// Get list of all possible_tiles

  std::unordered_set<Tile*> possible_tiles;
  for (const auto& item : tiles__) {
    possible_tiles.insert(item.second);
  }

  /// Reset the buffer__

  for (size_t i = 0, e = buffer__.size(); i < e; ++i) {
    buffer__[i].tile = nullptr;
    buffer__[i].possible_tiles = possible_tiles;
  }

  apply_constraints__();

  /// Reset number of tiles collapsed

  collapsed_count__ = 0;
}

bool wfc::Canvas::collapse_next() {
  /// Check if uncollapsed tile exists

  if (collapsed_count__ == rows__ * columns__) {
    return true;
  }

  /// Collapse the tile with the lowest entropy

  size_t spot_idx = get_lowest_entropy_spot_idx__();
  wfc::Spot* spot = &buffer__[spot_idx];
  if (spot->possible_tiles.size() == 0) {
    return false;
  }

  /// Select a tile to collapse into

  size_t tile_selection_idx = wfc::Random::int_from_range(0, spot->possible_tiles.size());
  wfc::Tile* tile = *wfc::select_from(spot->possible_tiles, tile_selection_idx);

  // Collapse the tile

  spot->tile = tile;
  spot->possible_tiles.clear();
  reduce_entropy_arround__(spot_idx);
  ++collapsed_count__;
  return true;
}

void wfc::Canvas::render() {

  /// Clear the window

  SDL_RenderClear(renderer__);

  /// Loop through buffer__ and add tiles to the window

  for (size_t row = 0; row < rows__; ++row) {
    for (size_t col = 0; col < columns__; ++col) {
      size_t idx = (row * columns__) + col;

      /// Calculate the position of the tile in the window

      SDL_Rect pos_rect = {
        static_cast<int>((col * tile_width__)),
        static_cast<int>((row * tile_height__)),
        static_cast<int>(tile_width__),
        static_cast<int>(tile_height__)
      };


      /// Check if the tile is collapsed or not

      wfc::Tile* tile = buffer__[idx].tile;
      SDL_Texture* texture = tile ? tile->get_texture() : null_texture__;
      SDL_RenderCopy(renderer__, texture, nullptr, &pos_rect);
    }
  }

  /// Present the window

  SDL_RenderPresent(renderer__);
}

void wfc::Canvas::create_null_texture__() {
  /// Create a texture to represent uncollapsed tiles

  null_texture__ = SDL_CreateTexture(renderer__, SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_TARGET, tile_width__, tile_height__);

  if (!null_texture__) {
    throw std::runtime_error("Failed to create null texture");
  }

  /// Store the current render target

  SDL_Texture* previous_target = SDL_GetRenderTarget(renderer__);

  /// Fill the texture with color black

  SDL_SetRenderTarget(renderer__, null_texture__);
  SDL_SetRenderDrawColor(renderer__, 0, 0, 0, 0);
  SDL_RenderClear(renderer__);

  /// Draw white border around the texture

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

  /// Restore the render target

  SDL_SetRenderTarget(renderer__, previous_target);
}

size_t wfc::Canvas::get_lowest_entropy_spot_idx__() {
  size_t min_entropy = tiles__.size();

  /// Calculate the minimum entropy in the canvas

  for (const auto& spot : buffer__) {
    if (spot.tile != nullptr) {
      continue;
    }
    min_entropy = std::min(min_entropy, spot.possible_tiles.size());
  }

  /// Collect all tiles that have the minimum entropy

  std::vector<size_t> min_entropy_spots;
  for (size_t i = 0, e = buffer__.size(); i < e; ++i) {
    if (buffer__[i].possible_tiles.size() == min_entropy) {
      min_entropy_spots.push_back(i);
    }
  }

  /// Return a random spot with minimum entropy

  return min_entropy_spots[wfc::Random::int_from_range(0, min_entropy_spots.size())];
}

void wfc::Canvas::reduce_entropy_arround__(size_t p_spot_idx) {
  /// Get the spot to reduce entropy around

  wfc::Spot* current_spot = &buffer__[p_spot_idx];

  if (current_spot->tile == nullptr) {
    return;
  }

  size_t row_number = p_spot_idx / columns__;
  size_t col_number = p_spot_idx % columns__;

  auto reduce_for = [](wfc::Spot* spot, wfc::Spot* current, wfc::Directions dir){
    /// Get the rules of the tile of current spot

    std::unordered_set<wfc::Tile*> rules = current->tile->get_rules(dir);
    if (rules.size() == 0) {
      return;
    }

    /// Get the current possibilities of reducing spot

    std::unordered_set<wfc::Tile*>& current_possibilities = spot->possible_tiles;

    /// Find intersection

    spot->possible_tiles = wfc::set_intersection(current_possibilities, rules);
  };

  if (row_number != 0) { /// Top spot exists
    wfc::Spot* top_spot = &buffer__[p_spot_idx - columns__];
    reduce_for(top_spot, current_spot, wfc::Directions::NORTH);
  }

  if (row_number != rows__ - 1) { /// Bottom spot exists
    wfc::Spot* bottom_spot = &buffer__[p_spot_idx + columns__];
    reduce_for(bottom_spot, current_spot, wfc::Directions::SOUTH);
  }

  if (col_number != 0) { /// Left spot exists
    wfc::Spot* left_spot = &buffer__[p_spot_idx - 1];
    reduce_for(left_spot, current_spot, wfc::Directions::WEST);
  }

  if (col_number != columns__ - 1) { /// Right spot exists
    wfc::Spot* right_spot = &buffer__[p_spot_idx + 1];
    reduce_for(right_spot, current_spot, wfc::Directions::EAST);
  }

  if (direction_type__ != wfc::DirectionType::OCT_DIRECTIONS) {
    return;
  }

  if (row_number != 0 && col_number != 0) { /// Top Left spot exists
    wfc::Spot* top_left_spot = &buffer__[p_spot_idx - columns__ - 1];
    reduce_for(top_left_spot, current_spot, wfc::Directions::NORTH_WEST);
  }

  if (row_number != 0 && col_number != columns__ - 1) { /// Top Right spot exists
    wfc::Spot* top_right_spot = &buffer__[p_spot_idx - columns__ + 1];
    reduce_for(top_right_spot, current_spot, wfc::Directions::NORTH_EAST);
  }

  if (row_number != rows__ - 1 && col_number != 0) { /// Bottom Left spot exists
    wfc::Spot* bottom_left_spot = &buffer__[p_spot_idx + columns__ - 1];
    reduce_for(bottom_left_spot, current_spot, wfc::Directions::SOUTH_WEST);
  }

  if (row_number != rows__ - 1 && col_number != columns__ - 1) { /// Bottom Right spot exists
    wfc::Spot* bottom_right_spot = &buffer__[p_spot_idx + columns__ + 1];
    reduce_for(bottom_right_spot, current_spot, wfc::Directions::SOUTH_EAST);
  }
}

void wfc::Canvas::apply_constraints__() {

  /// Top Row

  if (constraints__.others[wfc::Constraints::TOP].size() > 0) {
    for (size_t i = 0; i < columns__; ++i) {
      buffer__[i].possible_tiles = constraints__.others[wfc::Constraints::TOP];
    }
  }

  /// Bottom Row

  if (constraints__.others[wfc::Constraints::BOTTOM].size() > 0) {
    for (size_t i = buffer__.size() - columns__, e = buffer__.size(); i < e; ++i) {
      buffer__[i].possible_tiles = constraints__.others[wfc::Constraints::BOTTOM];
    }
  }

  /// Left Column

  if (constraints__.others[wfc::Constraints::LEFT].size() > 0) {
    for (size_t i = 0, e = buffer__.size(); i < e; i += columns__) {
      buffer__[i].possible_tiles = constraints__.others[wfc::Constraints::LEFT];
    }
  }

  /// Right Column

  if (constraints__.others[wfc::Constraints::RIGHT].size() > 0) {
    for (size_t i = columns__ - 1, e = buffer__.size(); i < e; i += columns__) {
      buffer__[i].possible_tiles = constraints__.others[wfc::Constraints::RIGHT];
    }
  }

  /// Top Right Spot

  size_t top_right_index = columns__ - 1;
  if (constraints__.others[wfc::Constraints::TOP_RIGHT].size() > 0) {
    buffer__[top_right_index].possible_tiles = constraints__.others[wfc::Constraints::TOP_RIGHT];
  }

  /// Bottom Right Spot

  size_t bottom_right_index = buffer__.size() - 1;
  if (constraints__.others[wfc::Constraints::BOTTOM_RIGHT].size() > 0) {
    buffer__[bottom_right_index].possible_tiles = constraints__.others[wfc::Constraints::BOTTOM_RIGHT];
  }

  /// Bottom Left Spot

  size_t bottom_left_index = buffer__.size() - columns__;
  if (constraints__.others[wfc::Constraints::BOTTOM_LEFT].size() > 0) {
    buffer__[bottom_left_index].possible_tiles = constraints__.others[wfc::Constraints::BOTTOM_LEFT];
  }

  /// Top Left Spot

  size_t top_left_index = 0;
  if (constraints__.others[wfc::Constraints::TOP_LEFT].size() > 0) {
    buffer__[top_left_index].possible_tiles = constraints__.others[wfc::Constraints::TOP_LEFT];
  }

  /// Fixed

  for (auto& [idx, value]: constraints__.fixed) {
    buffer__[idx].possible_tiles = value;
  }
}
