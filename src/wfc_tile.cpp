#include "wfc_tile.h"

wfc::Tile::Tile(const std::string& p_path):
  path__(p_path) {
}

void wfc::Tile::add_rule(wfc::Directions p_direction, Tile* p_tile) {
  rules__[p_direction].insert(p_tile);
}

void wfc::Tile::add_rule(wfc::Directions p_direction, const std::initializer_list<Tile*>& p_list) {
  for (const auto& tile: p_list) {
    rules__[p_direction].insert(tile);
  }
}

bool wfc::Tile::check_rule(wfc::Directions p_direction, Tile* p_tile) {
  return rules__[p_direction].count(p_tile);
}
