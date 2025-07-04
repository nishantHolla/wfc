#ifndef WFC_CANVAS_H_
#define WFC_CANVAS_H_

#include "wfc_directions.h"
#include "wfc_tile.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <initializer_list>

namespace wfc {

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
   */
  Canvas(size_t p_width, size_t p_height, size_t p_rows, size_t p_columns);

  /*
   * Destroy the canvas and all the resources it holds
   */
  ~Canvas();

  /*
   * Set the base path for loading of images
   *
   * Params:
   *       String p_base_path: Path to set
   */
  void set_base_path(const std::string& p_base_path);

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
   *       String p_path: Tile path relative to the base_path__
   */
  void add_tile(const std::string& p_name, const std::string& p_path);

  /*
   * Add rule to a tile in the canvas
   *
   * Params:
   *       String     p_for: Name of the tile for which to add the rule
   *       Directions p_dir: Direction to add the rule in
   *       String     p_to : Name of the tile in the rule
   */
  void add_rule(const std::string& p_for, wfc::Directions p_dir, const std::string& p_to);

  /*
   * Add more than one rule of placement for tile in the canvas
   *
   * Params:
   *       String     p_for: Name of the tile for which to add the rule
   *       Directions p_dir: Direction to add the rule in
   *       String     p_to : Initializer list of names of the tile in the rule
   */
  void add_rule(const std::string& p_for, wfc::Directions p_dir,
                const std::initializer_list<const std::string> p_to);

private:
  const size_t width__;
  const size_t height__;
  const size_t rows__;
  const size_t columns__;
  const size_t tile_width__;
  const size_t tile_height__;
  std::filesystem::path base_path__;
  wfc::DirectionType direction_type__;
  std::unordered_map<std::string, wfc::Tile*> tiles__;
};

}

#endif // !WFC_CANVAS_H_
