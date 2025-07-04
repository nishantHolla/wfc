#ifndef WFC_TILE_H_
#define WFC_TILE_H_

#include "wfc_directions.h"

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>

namespace wfc {

class Tile {
public:

  /*
   * Construct the tile object with the path to the tile image
   *
   * Params:
   *       String p_path: Path to the image file of the tile
   */
  Tile(const std::string& p_path);

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

private:
  const std::string path__;
  std::unordered_map<wfc::Directions, std::unordered_set<Tile*>> rules;
};

}

#endif // !WFC_TILE_H_
