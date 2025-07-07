#include "wfc_parser.h"
#include "wfc_canvas.h"
#include "wfc.h"
#include "wfc_utils.h"
#include "wfc_log.h"

#include <fstream>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;

wfc::Parser::Parser(const std::string& p_config_path) :
  config_path__(p_config_path) {

  /// Open input stream for config file

  std::ifstream config_stream(config_path__);
  if (!config_stream) {
    char msg[500];
    sprintf(msg, "Failed to open config file at %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  /// Create json object

  config_json__ = json::parse(config_stream);

  /// Close input stream for config file

  config_stream.close();
}

void wfc::Parser::parse_canvas(wfc::CanvasInfo& p_canvas_info) const {

  /// Check if canvas section is defined

  if (!config_json__.contains("canvas")) {
    char msg[500];
    sprintf(msg, "Path \"/canvas\" is missing in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  /// Parse values in canvas section

  auto& section = config_json__["canvas"];
  p_canvas_info.width = parse_positive_int__(section, "width", "/canvas");
  p_canvas_info.height = parse_positive_int__(section, "height", "/canvas");
  p_canvas_info.rows = parse_positive_int__(section, "rows", "/canvas");
  p_canvas_info.columns = parse_positive_int__(section, "columns", "/canvas");

  std::string direction_type = parse_string__(section, "directions", "/canvas");
  if (direction_type == "quad") {
    p_canvas_info.direction_type = wfc::DirectionType::QUAD_DIRECTIONS;
  }
  else if (direction_type == "oct") {
    p_canvas_info.direction_type = wfc::DirectionType::OCT_DIRECTIONS;
  }
  else {
    char msg[500];
    sprintf(msg, "Path \"/canvas/directions\" must have the value \"quad\" or \"oct\" in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

}

void wfc::Parser::parse_tiles(std::vector<TileInfo>& p_tiles) const {

  /// Check if tiles section is defined

  if (!config_json__.contains("tiles")) {
    char msg[500];
    sprintf(msg, "Path \"/tiles\" is missing in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  auto parse_tile_section = [this](const json& section, std::vector<TileInfo>& p_tiles){
    for (auto& [key, item]: section.items()) {
      const std::string path = parse_string__(item, "path", "/tiles/" + key);
      wfc::TileInfo tile(key, fs::absolute(path));

      if (!item.contains("rules")) {
        char msg[500];
        sprintf(msg, "Path \"/tiles/%s/rules\" is missing in config file %s", key.c_str(), config_path__.c_str());
        throw std::runtime_error(msg);
      }

      parse_tile_rules__(tile, item["rules"]);
      p_tiles.emplace_back(tile);
    }
  };

  auto& section = config_json__["tiles"];

  if (section.is_object()) { /// If tiles section is object then parse the tiles
    parse_tile_section(section, p_tiles);
  }

  else if (section.is_array()) { /// If tiles section is an array then it is a list of sub config files
    for (auto& v : section) {
      /// Save current directory

      fs::path saved_path = fs::current_path();

      /// Resolve the path to sub config file

      fs::path sub_config_path(v);
      fs::current_path(sub_config_path.parent_path());

      /// Parse sub config file

      wfc::Log::info("Parsing sub config file at " + sub_config_path.string() + "...");
      json sub_config = open_sub_config__(sub_config_path.filename());
      parse_tile_section(sub_config, p_tiles);

      /// Restore directory

      fs::current_path(saved_path);
    }
  }
  else {
    char msg[500];
    sprintf(msg, "Path \"/tiles\" is expected to be an object or array in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  /// Resolve rules of tiles that were defined using inversion

  resolve_tile_inversion__(p_tiles);
}

void wfc::Parser::parse_constraints(wfc::ConstraintInfo& p_cons_info) const {
  if (!config_json__.contains("constraints")) {
    return;
  }

  auto parse_contraint = [this](const json& section, const std::string& name) {
    if (!section.contains(name)) {
      return std::unordered_set<std::string>();
    }

    if (!section[name].is_array()) {
      char msg[500];
      sprintf(msg, "Path \"/constraints/%s\" is expected to be an array in config file %s", name.c_str(), config_path__.c_str());
      throw std::runtime_error(msg);
    }

    std::vector<std::string> vec = section[name].get<std::vector<std::string>>();
    return std::unordered_set<std::string>(vec.begin(), vec.end());
  };

  json section = config_json__["constraints"];
  p_cons_info.top_right = parse_contraint(section, "top_right");
  p_cons_info.bottom_right = parse_contraint(section, "bottom_right");
  p_cons_info.bottom_left = parse_contraint(section, "bottom_left");
  p_cons_info.top_left = parse_contraint(section, "top_left");
  p_cons_info.top = parse_contraint(section, "top");
  p_cons_info.right = parse_contraint(section, "right");
  p_cons_info.bottom = parse_contraint(section, "bottom");
  p_cons_info.left = parse_contraint(section, "left");
  p_cons_info.corners = parse_contraint(section, "corners");
  p_cons_info.edges = parse_contraint(section, "edges");

  if (!section.contains("fixed")) {
    return;
  }

  if (!section["fixed"].is_array()) {
    char msg[500];
    sprintf(msg, "Path \"/constraints/fixed\" is expected to be an array in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  for (auto& item : section["fixed"]) {
    if (!item.is_object()) {
      char msg[500];
      sprintf(msg, "Path \"/constraints/fixed\" is expected to be an array of objects in config file %s", config_path__.c_str());
      throw std::runtime_error(msg);
    }

    ConstraintInfo::Fixed f;
    f.row = parse_positive_int__(item, "row", "/constraints/fixed");
    f.column = parse_positive_int__(item, "column", "/constraints/fixed");
    if (!item.contains("tiles") || !item["tiles"].is_array()) {
      char msg[500];
      sprintf(msg, "Path \"/constraints/fixed/*/tiles\" is expected to be an array in config file %s", config_path__.c_str());
      throw std::runtime_error(msg);
    }
    std::vector<std::string> vec = item["tiles"].get<std::vector<std::string>>();
    f.tiles = std::unordered_set<std::string>(vec.begin(), vec.end());
    p_cons_info.fixed.emplace_back(f);
  }
}

size_t wfc::Parser::parse_positive_int__(const json& section, const std::string& p_key,
                                       const std::string& p_path) const {
  /// Check if the key exists

  if (!section.contains(p_key)) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  /// Check if the key is a positive number

  if (section[p_key].type() != json::value_t::number_unsigned) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" was expected to be a positive integer", p_path.c_str(), p_key.c_str());
    throw std::runtime_error(msg);
  }

  return section[p_key];
}

std::string wfc::Parser::parse_string__(const json& section, const std::string& p_key,
                                        const std::string& p_path) const {

  /// Check if the key exists

  if (!section.contains(p_key)) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  /// Check if the key is a string

  if (section[p_key].type() != json::value_t::string) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" was expected to be a string", p_path.c_str(), p_key.c_str());
    throw std::runtime_error(msg);
  }

  return section[p_key];
}

bool wfc::Parser::parse_bool__(const json& section, const std::string& p_key,
                                        const std::string& p_path) const {

  /// Check if the key exists

  if (!section.contains(p_key)) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  /// Check if the key is a boolean

  if (section[p_key].type() != json::value_t::boolean) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" was expected to be a boolean", p_path.c_str(), p_key.c_str());
    throw std::runtime_error(msg);
  }

  return section[p_key];
}

void wfc::Parser::parse_tile_rules__(TileInfo& tile, const json& section) const {
  using wfc::Directions;

  /// Map enums to the strings expected in the config file

  std::unordered_map<wfc::Directions, std::string> dir_map = {
    {Directions::NORTH, "north"},
    {Directions::NORTH_EAST, "north_east"},
    {Directions::EAST, "east"},
    {Directions::SOUTH_EAST, "south_east"},
    {Directions::SOUTH, "south"},
    {Directions::SOUTH_WEST, "south_west"},
    {Directions::WEST, "west"},
    {Directions::NORTH_WEST, "north_west"},
  };

  /// Go through all the rules of the tile

  for (const auto& [key, value] : dir_map) {
    std::string negation_value = "!" + value;

    if (section.contains(negation_value)) { /// Check if the rule for the direction is negated
      std::vector<std::string> rule_vec = section[negation_value].get<std::vector<std::string>>();
      std::unordered_set<std::string> rules(rule_vec.begin(), rule_vec.end());
      tile.rules[key] = rules;
      tile.directions_to_invert.insert(key);
    }
    else if (section.contains(value)) { /// Check if the rule for the direction exists
      std::vector<std::string> rule_vec = section[value].get<std::vector<std::string>>();
      std::unordered_set<std::string> rules(rule_vec.begin(), rule_vec.end());
      tile.rules[key] = rules;
    }
    else { /// The rule for the direction is empty
      tile.rules[key] = {};
    }
  }
}

void wfc::Parser::resolve_tile_inversion__(std::vector<TileInfo>& p_tiles) const {

  /// Collect names of all tiles

  std::unordered_set<std::string> all_tile_names;
  for (const TileInfo& tile : p_tiles) {
    all_tile_names.insert(tile.name);
  }

  /// Go throug all tiles and see if any of their rules is neagated

  for (TileInfo& tile : p_tiles) {
    for (const wfc::Directions& dir : tile.directions_to_invert) {

      /// Negate the rule set

      tile.rules[dir] = set_difference(all_tile_names, tile.rules[dir]);
    }
  }
}

json wfc::Parser::open_sub_config__(const std::string& p_path) const {
  /// Check the sub config file

  check_config_file(p_path);

  /// Open input stream for sub config file

  std::ifstream config_stream(p_path);
  if (!config_stream) {
    char msg[500];
    sprintf(msg, "Failed to open sub config file at %s", p_path.c_str());
    throw std::runtime_error(msg);
  }

  /// Create json object

  json config_json = json::parse(config_stream);
  config_stream.close();

  /// Close input stream for config file

  return config_json;
}
