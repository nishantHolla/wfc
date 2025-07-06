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


struct TileInfo {
  std::string name;
  std::string path;
  std::unordered_map<wfc::Directions, std::vector<std::string>> rules;
  TileInfo(const std::string& p_name, const std::string& p_path) :
    name(p_name),
    path(p_path) {
  }

  void add_rule(wfc::Directions p_dir, const std::string& p_tile_name) {
    rules[p_dir].emplace_back(p_tile_name);
  }
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
   *      size_t p_width : Store width of the canvas
   *      size_t p_height: Store height of the canvas
   *      size_t p_rows  : Store number of rows in the canvas
   *      size_t p_colums: Store number of columns in the canvas
   */
  void parse_canvas(size_t& p_width, size_t& p_height, size_t& p_rows, size_t& p_cols) const;

  /*
   * Parse "settings" section of the config file and store values in the given references
   *
   * Params:
   *       wfc::DirectionType p_direction_type: Store the direction type
   *       String             p_base_path     : Store the base path
   */
  void parse_settings(wfc::DirectionType& p_direction_type, std::string& p_base_path);

  /*
   * Parse "tile" section of the config file and store vlaues in the given reference to vector
   *
   * Params:
   *       Vector<TileInfo> p_tiles: Store the tiles
   */
  void parse_tiles(std::vector<TileInfo>& p_tiles);

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
   * Open the json file at given path and read it
   *
   * Params:
   *       String p_path: Path to the config file
   */
  json open_sub_config__(const std::string& p_path);
};

}

#endif // !WFC_PARSER_H_
