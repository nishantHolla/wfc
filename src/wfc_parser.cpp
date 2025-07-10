#include "wfc_parser.h"
#include "wfc_canvas.h"
#include "wfc.h"
#include "wfc_utils.h"
#include "wfc_log.h"

#include <sstream>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;

wfc::Parser::Parser(const std::string& p_config_path) :
  config_path__(fs::absolute(p_config_path)) {

  /// Open input stream for config file

  std::ifstream config_stream(config_path__);
  if (!config_stream) {
    std::stringstream msg("Failed to open config file at ");
    msg << config_path__;
    throw std::runtime_error(msg.str());
  }

  /// Create json object

  config_json__ = json::parse(config_stream);

  /// Close input stream for config file

  config_stream.close();
}

void wfc::Parser::parse_canvas(wfc::CanvasInfo& p_canvas_info) const {

  /// Check if canvas section is defined

  if (!config_json__.contains("canvas")) {
    std::stringstream msg("Path \"/canvas\" is missing in config file ");
    msg << config_path__;
    throw std::runtime_error(msg.str());
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
    std::stringstream msg("path \"/canvas/directions\" must have the value \"quad\" or \"oct\" in config file ");
    msg << config_path__;
    throw std::runtime_error(msg.str());
  }

}

void wfc::Parser::parse_tiles(std::vector<TileInfo>& p_tiles, const wfc::GroupInfo& p_groups) const {

  /// Check if tiles section is defined

  if (!config_json__.contains("tiles")) {
    std::stringstream msg("Path \"/tiles\" is missing in config file ");
    msg << config_path__;
    throw std::runtime_error(msg.str());
  }

  auto parse_tile_section = [this, &p_groups, &p_tiles](const json& section){
    for (auto& [key, item]: section.items()) {
      const std::string path = parse_string__(item, "path", "/tiles/" + key);
      wfc::TileInfo tile(key, fs::absolute(path));

      if (!item.contains("rules")) {
        std::stringstream msg("Path \"/tiles/");
        msg << key << "/rules\" is missing in config file " << config_path__;
        throw std::runtime_error(msg.str());
      }

      parse_tile_rules__(tile, item["rules"], p_groups);
      p_tiles.emplace_back(tile);
    }
  };

  auto& section = config_json__["tiles"];

  if (section.is_object()) { /// If tiles section is object then parse the tiles
    parse_tile_section(section);
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
      parse_tile_section(sub_config);

      /// Restore directory

      fs::current_path(saved_path);
    }
  }
  else {
    std::stringstream msg("Path \"/tiles\" is expected to be an object or an array in config file ");
    msg << config_path__;
    throw std::runtime_error(msg.str());
  }

  /// Resolve rules of tiles that were defined using inversion

  resolve_tile_inversion__(p_tiles);
}

void wfc::Parser::parse_groups(wfc::GroupInfo& p_group) const {
  if (!config_json__.contains("groups")) {
    return;
  }

  if (!config_json__["groups"].is_object()) {
    std::stringstream msg("Path \"/groups\" is expected to be an object in config file ");
    msg << config_path__;
    throw std::runtime_error(msg.str());
  }

  json section = config_json__["groups"];
  for (auto& [key, value]: section.items()) {
    p_group.groups[key] = value.get<std::vector<std::string>>();
  }
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
      std::stringstream msg("Path \"/constraints/");
      msg << name << "\" is expected to be an array in config file " << config_path__;
      throw std::runtime_error(msg.str());
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
    std::stringstream msg("Path \"/constraints/fixed\" is expected to be an array in config file ");
    msg << config_path__;
    throw std::runtime_error(msg.str());
  }

  for (auto& item : section["fixed"]) {
    if (!item.is_object()) {
      std::stringstream msg("Path \"/constraints/fixed\" is expected to be an array of objects in config file ");
      msg << config_path__;
      throw std::runtime_error(msg.str());
    }

    ConstraintInfo::Fixed f;
    f.row = parse_positive_int__(item, "row", "/constraints/fixed");
    f.column = parse_positive_int__(item, "column", "/constraints/fixed");
    if (!item.contains("tiles") || !item["tiles"].is_array()) {
      std::stringstream msg("Path \"/constraints/fixed/*/tiles\" is expected to be an array in config file ");
      msg << config_path__;
      throw std::runtime_error(msg.str());
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
    std::stringstream msg("Path \"");
    msg << p_path << "/" << p_key << "\" is missing in config file " << config_path__;
    throw std::runtime_error(msg.str());
  }

  /// Check if the key is a positive number

  if (section[p_key].type() != json::value_t::number_unsigned) {
    std::stringstream msg("Path \"");
    msg << p_path << "/" << p_key << "\" is expected to be a positive integer in config file ";
    msg << config_path__;
    throw std::runtime_error(msg.str());
  }

  return section[p_key];
}

std::string wfc::Parser::parse_string__(const json& section, const std::string& p_key,
                                        const std::string& p_path) const {

  /// Check if the key exists

  if (!section.contains(p_key)) {
    std::stringstream msg("Path \"");
    msg << p_path << "/" << p_key << "\" is missing in config file " << config_path__;
    throw std::runtime_error(msg.str());
  }

  /// Check if the key is a string

  if (section[p_key].type() != json::value_t::string) {
    std::stringstream msg("Path \"");
    msg << p_path << "/" << p_key << "\" is expected to be a string in config file " << config_path__;
    throw std::runtime_error(msg.str());
  }

  return section[p_key];
}

bool wfc::Parser::parse_bool__(const json& section, const std::string& p_key,
                                        const std::string& p_path) const {

  /// Check if the key exists

  if (!section.contains(p_key)) {
    std::stringstream msg("Path \"");
    msg << p_path << "/" << p_key << "\" is missing in config file " << config_path__;
    throw std::runtime_error(msg.str());
  }

  /// Check if the key is a boolean

  if (section[p_key].type() != json::value_t::boolean) {
    std::stringstream msg("Path \"");
    msg << p_path << "/" << p_key << "\" is expected to be a boolean in config file " << config_path__;
    throw std::runtime_error(msg.str());
  }

  return section[p_key];
}

void wfc::Parser::parse_tile_rules__(TileInfo& p_tile, const json& p_section, const wfc::GroupInfo& p_group_info) const {
  auto& groups = p_group_info.groups;
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

  auto add_rules = [&groups, &p_tile](wfc::Directions key, std::vector<std::string>& rules) {
    std::unordered_set<std::string> rule_set;
    for (const std::string& rule : rules) {
      if (groups.find(rule) != groups.end()) {
        for (const std::string& gr: groups.at(rule)) {
          rule_set.insert(gr);
        }
      }
      else {
        rule_set.insert(rule);
      }
    }
    p_tile.rules[key] = rule_set;
  };

  /// Go through all the rules of the tile

  for (const auto& [key, value] : dir_map) {
    std::string negation_value = "!" + value;

    if (p_section.contains(negation_value)) { /// Check if the rule for the direction is negated
      std::vector<std::string> rule_vec = p_section[negation_value].get<std::vector<std::string>>();
      add_rules(key, rule_vec);
      p_tile.directions_to_invert.insert(key);
    }
    else if (p_section.contains(value)) { /// Check if the rule for the direction exists
      std::vector<std::string> rule_vec = p_section[value].get<std::vector<std::string>>();
      add_rules(key, rule_vec);
    }
    else { /// The rule for the direction is empty
      p_tile.rules[key] = {};
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
    std::stringstream msg("Failed to open sub config file at ");
    msg << p_path;
    throw std::runtime_error(msg.str());
  }

  /// Create json object

  json config_json = json::parse(config_stream);
  config_stream.close();

  /// Close input stream for config file

  return config_json;
}
