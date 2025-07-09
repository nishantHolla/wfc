#ifndef WFC_CANVAS_H_
#define WFC_CANVAS_H_

#include "wfc_directions.h"
#include "wfc_tile.h"
#include "wfc_utils.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <unordered_set>

#include <SDL2/SDL.h>

namespace wfc {

struct Spot {
  wfc::Tile* tile;
  std::unordered_set<wfc::Tile*> possible_tiles;
  Spot() : tile(nullptr), possible_tiles{} {
  }
};

class Canvas {
public:
  /*
   * Construct the canvas with the given specifications
   *
   * Params:
   *       size_t p_width : Width of the canavas
   *       size_t p_height: Height of the canvas
   *       size_t p_rows  : Number of rows in the canvas
   *       size_t columns : Number of columns in the canvas
   *
   * Throws:
   *       If SDL Window creation fails
   *       If SDL Renderer creation fails
   */
  Canvas(size_t p_width, size_t p_height, size_t p_rows, size_t p_columns);

  /*
   * Destroy the canvas and all the resources it holds
   */
  ~Canvas();

  /*
   * Set direction type
   *
   * Params:
   *       DirectionType p_dir_type: Direction type to set
   */
  void set_direction_type(const wfc::DirectionType p_dir_type);

  /*
   * Add possible tiles for the canvas
   *
   * Params:
   *       String p_name: Name of the tile
   *       String p_path: Path to the tile image
   *
   * Throws:
   *       If tile with name p_name already exists in tiles__
   */
  void add_tile(const std::string& p_name, const std::string& p_path);

  /*
   * Add rule to a tile in the canvas
   *
   * Params:
   *       String     p_for: Name of the tile for which to add the rule
   *       Directions p_dir: Direction to add the rule in
   *       String     p_to : Name of the tile in the rule
   *
   * Throws:
   *       If tile with name p_for or p_to does not exist in tiles__
   */
  void add_rule(const std::string& p_for, wfc::Directions p_dir, const std::string& p_to);

  /*
   * Add more than one rule of placement for tile in the canvas
   *
   * Params:
   *       String         p_for: Name of the tile for which to add the rule
   *       Directions     p_dir: Direction to add the rule in
   *       Vector<String> p_to : List of names of the tile in the rule
   *
   * Throws:
   *       If tile with name p_for or p_to does not exist in tiles__
   */
  void add_rule(const std::string& p_for, wfc::Directions p_dir, const std::vector<std::string>& p_to);

  /*
   * Add a constraint to the canvas for edges
   *
   * Params:
   *       Constraints p_cons: Constraint type to be added for
   *       String      p_tile: Name of the tile for the constraint
   *
   * Throws:
   *       If tile with name p_tile does not exist in tiles__
   */
  void add_constraint(wfc::Constraints p_cons, const std::string& p_tile);

  /*
   * Add a constraint to the canvas for coordinates
   *
   * Params:
   *       size_t  p_x   : x coordinate of the constraint
   *       size_t  p_y   : y coordinate of the constraint
   *       String  p_tile: Name of the tile for the constraint
   *
   * Throws:
   *       If tile with name p_tile does not exist in tiles__
   *       If the coordinates are out of bound
   */
  void add_constraint(size_t x, size_t y, const std::string& p_tile);

  /*
   * Reset the canvas
   */
  void reset();

  /*
   * Collapse the next tile in the canvas
   *
   * Returns:
   *        true if tile was collapsed successfully, else false
   */
  bool collapse_next();

  /*
   * Render the current state of buffer__ using SDL window
   */
  void render();

private:
  struct Constraints {
    std::unordered_map<wfc::Constraints, std::unordered_set<wfc::Tile*>> others;
    std::unordered_map<size_t, std::unordered_set<wfc::Tile*>> fixed;
  };

  const size_t width__;
  const size_t height__;
  const size_t rows__;
  const size_t columns__;
  const size_t tile_width__;
  const size_t tile_height__;
  wfc::DirectionType direction_type__;
  std::unordered_map<std::string, wfc::Tile*> tiles__;
  Constraints constraints__;
  SDL_Window* window__;
  SDL_Renderer* renderer__;
  std::vector<wfc::Spot> buffer__;
  SDL_Texture* null_texture__;
  size_t collapsed_count__;

  /*
   * Create a black tile texture with white border and store it in null_texture__ to represent
   * uncollapsed tile
   */
  void create_null_texture__();

  /*
   * Get the index of spot with lowest entropy. Selects a random spot if more than one spot has
   * the same lowest entropy
   *
   * Returns:
   *        Index of the selected spot in buffer__
   */
  size_t get_lowest_entropy_spot_idx__();

  /*
   * Reduces entropy arround the given spot index using the rules
   *
   * Params:
   *       size_t p_spot_idx: Index of the spot in buffer__ to look around
   */
  void reduce_entropy_arround__(size_t p_spot_idx);

  /*
   * Apply constraints defined in constraints__ to the buffer
   */
  void apply_constraints__();
};

}

#endif // !WFC_CANVAS_H_
