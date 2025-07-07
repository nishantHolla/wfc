#include "wfc_tile.h"
#include <SDL2/SDL_image.h>

wfc::Tile::Tile(const std::string& p_path):
  path__(p_path) {
}

wfc::Tile::Tile(const std::string& p_path, SDL_Renderer* p_renderer): wfc::Tile::Tile(p_path) {
  load(p_renderer);
}

void wfc::Tile::load(SDL_Renderer* p_renderer) {
  SDL_Surface* surface = IMG_Load(path__.c_str());
  if (!surface) {
    char msg[500];
    sprintf(msg, "Failed to load texture at %s\n", path__.c_str());
    throw std::runtime_error(msg);
  }

  texture__ = SDL_CreateTextureFromSurface(p_renderer, surface);
  SDL_FreeSurface(surface);

  if (!texture__) {
    char msg[500];
    sprintf(msg, "Failed to convert surface to texture for %s\n", path__.c_str());
    throw std::runtime_error(msg);
  }
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

std::unordered_set<wfc::Tile*> wfc::Tile::get_rules(wfc::Directions p_dir) {
  return rules__[p_dir];
}

SDL_Texture* wfc::Tile::get_texture() const {
  return texture__;
}
