#ifndef WFC_PARSER_H_
#define WFC_PARSER_H_

#include "wfc_directions.h"
#include "wfc_tile.h"

#include <filesystem>
#include <string>
#include <vector>

#include "external/json.hpp"

namespace wfc {
using json = nlohmann::json;

struct CanvasInfo {
  size_t width;
  size_t height;
  size_t rows;
  size_t columns;
  wfc::DirectionType direction_type;
};

struct TileInfo {
  std::string name;
  std::string path;
  std::unordered_set<wfc::Directions> directions_to_invert;
  std::unordered_map<wfc::Directions, std::unordered_set<std::string>> rules;
  TileInfo(const std::string& p_name, const std::string& p_path) :
    name(p_name),
    path(p_path) {
  }

  void add_rule(wfc::Directions p_dir, const std::string& p_tile_name) {
    rules[p_dir].insert(p_tile_name);
  }
};

struct ConstraintInfo {
  struct Fixed {
    int row; int column;
    std::unordered_set<std::string> tiles;
  };

  std::unordered_set<std::string> top_right;
  std::unordered_set<std::string> bottom_right;
  std::unordered_set<std::string> bottom_left;
  std::unordered_set<std::string> top_left;
  std::unordered_set<std::string> top;
  std::unordered_set<std::string> right;
  std::unordered_set<std::string> bottom;
  std::unordered_set<std::string> left;

  std::unordered_set<std::string> corners;
  std::unordered_set<std::string> edges;

  std::vector<Fixed> fixed;
};

class Parser {
public:
  /*
   * Construct the parser with the given path to config file
   *
   * Params:
   *       String p_config_path: Path to the config file
   *
   * Throws:
   *       If faild to open input stream
   */
  Parser(const std::string& p_config_path);

  /*
   * Parse "canvas" section of the config file and store values in the given references
   *
   * Params:
   *      CanvasInfo p_canvas_info: Reference to CanvasInfo struct to store parsed data
   */
  void parse_canvas(wfc::CanvasInfo& p_canvas_info) const;

  /*
   * Parse "tile" section of the config file and store vlaues in the given reference to vector
   *
   * Params:
   *       Vector<TileInfo> p_tiles: Store the tiles
   */
  void parse_tiles(std::vector<TileInfo>& p_tiles) const;

  /*
   * Parse "constraints" section of the config file and stroe values in the given reference
   *
   * Params:
   *       ConstraintInfo p_cons_info: Reference to ConstraintInfo struct to store parsed data
   */
  void parse_constraints(wfc::ConstraintInfo& p_cons_info) const;

private:
  std::filesystem::path config_path__;
  json config_json__;

  /*
   * Get a positive integer from the give section in the config
   *
   * Params:
   *       json   section: Json section to search in
   *       String p_key  : Key to search in the section
   *       String p_path : Path of the key
   *
   * Returns:
   *        positive integer parsed
   *
   * Throws:
   *       If value is not found
   *       If value is not positive integer
   */
  size_t parse_positive_int__(const json& section, const std::string& p_key, const std::string& p_path) const;

  /*
   * Get a string from the given section in the config
   * Params:
   *       json   section: Json section to search in
   *       String p_key  : Key to search in the section
   *       String p_path : Path of the key
   *
   * Returns:
   *        string parsed
   *
   * Throws:
   *       If value is not found
   */
  std::string parse_string__(const json& section, const std::string& p_key, const std::string& p_path) const;

  /*
   * Get a boolean from the given section in the config
   *
   * Params:
   *       json   section: Json section to search in
   *       String p_key  : Key to search in the section
   *       String p_path : Path of the key
   *
   * Returns:
   *        boolean parsed
   *
   * Throws:
   *       If value is not found
   */
  bool parse_bool__(const json& section, const std::string& p_key, const std::string& p_path) const;

  /*
   * Parse the rules section of tiles and fill the rules property of the given tile
   *
   * Params:
   *       TileInfo tile   : Reference to the tile for which to fill the rules for
   *       json     section: Json section that contains the rules
   */
  void parse_tile_rules__(TileInfo& tile, const json& section) const;

  /*
   * Resolve the rules that need to be inverted
   *
   * Params:
   *       vector<TileInfo> p_tiles: Reference to the vector that contains all the tiles
   */
  void resolve_tile_inversion__(std::vector<TileInfo>& p_tiles) const;

  /*
   * Open the json file at given path and read it
   *
   * Params:
   *       String p_path: Path to the config file
   */
  json open_sub_config__(const std::string& p_path) const;
};

}

#endif // !WFC_PARSER_H_
