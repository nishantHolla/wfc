#ifndef WFC_TILE_H_
#define WFC_TILE_H_

#include "wfc_directions.h"

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>
#include <filesystem>
#include <SDL2/SDL.h>

namespace wfc {

class Tile {
public:

  /*
   * Construct the tile object with the path to the tile image
   *
   * Params:
   *       String p_path    : Path to the image file of the tile
   */
  Tile(const std::string& p_path);

  /*
   * Construct the tile object with the path to the tile image and use the given renderer
   * to load the image
   *
   * Params:
   *       String        p_path     : Path to the image file of the tile
   *       SDL_Renderer* p_renderer : Renderer to use to load the image
   */
  Tile(const std::string& p_path, SDL_Renderer* renderer);

  /*
   * Load the image from its path
   *
   * Params:
   *       SDL_Renderer* p_renderer: Renderer to use to load the texture for the tile
   *
   * Throws:
   *       If Img_Load fails to load image at p_path
   *       If conversion from SDL Surface to SDL Texture fails
   */
  void load(SDL_Renderer* p_renderer);

  /*
   * Add a rule of placement for this tile in the given direction
   *
   * Params:
   *       Directions p_dir : Direction of the rule
   *       Tile*      p_tile: Pointer to the tile that can be placed in the direction of this tile
   */
  void add_rule(wfc::Directions p_direction, Tile* p_tile);

  /*
   * Add more than one rule of placement for this tile in the given direction
   *
   * Params:
   *       Directions              p_dir : Direction of the rule
   *       initializer_list<Tile*> p_tile: List of pointers to the tile that can be placed in the
   *                                       direction of this tile
   */
  void add_rule(wfc::Directions p_direction, const std::initializer_list<Tile*>& p_list);

  /*
   * Check if the given tile can be placed in the given direction
   *
   * Params:
   *       Directions p_dir : Direction to check in
   *       Tile*      p_tile: Tile to check
   *
   * Return:
   *       true if tile can be placed else false
   */
  bool check_rule(wfc::Directions p_direction, Tile* p_tile);

  /*
   * Get the texture of the tile
   */
  SDL_Texture* get_texture() const;

private:
  const std::filesystem::path path__;
  std::unordered_map<wfc::Directions, std::unordered_set<Tile*>> rules__;
  SDL_Texture* texture__;
};

}

#endif // !WFC_TILE_H_
