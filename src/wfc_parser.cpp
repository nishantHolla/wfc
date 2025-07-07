#include "wfc_parser.h"
#include "wfc_canvas.h"
#include "wfc.h"
#include "wfc_utils.h"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

wfc::Parser::Parser(const std::string& p_config_path) :
  config_path__(p_config_path) {

  std::ifstream config_stream(config_path__);
  if (!config_stream) {
    char msg[500];
    sprintf(msg, "Failed to open config file at %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  config_json__ = json::parse(config_stream);
  config_stream.close();
}

void wfc::Parser::parse_canvas(wfc::CanvasInfo& p_canvas_info) const {
  if (!config_json__.contains("canvas")) {
    char msg[500];
    sprintf(msg, "Path \"/canvas\" is missing in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

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

void wfc::Parser::parse_tiles(std::vector<TileInfo>& p_tiles) {
  if (!config_json__.contains("tiles")) {
    char msg[500];
    sprintf(msg, "Path \"/tiles\" is missing in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  auto parse_tile_section = [this](const json& section, std::vector<TileInfo>& p_tiles){
    for (auto& [key, item]: section.items()) {
      const std::string path = parse_string__(item, "path", "/tiles/" + key);
      wfc::TileInfo tile(key, std::filesystem::absolute(path));

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
  if (section.is_object()) {
    parse_tile_section(section, p_tiles);
  }
  else if (section.is_array()) {
    for (auto& v : section) {
      std::filesystem::path saved_path = std::filesystem::current_path();
      std::filesystem::path sub_config_path(v);
      std::filesystem::current_path(sub_config_path.parent_path());
      json sub_config = open_sub_config__(sub_config_path.filename());
      parse_tile_section(sub_config, p_tiles);
      std::filesystem::current_path(saved_path);
    }
  }
  else {
    char msg[500];
    sprintf(msg, "Path \"/tiles\" is expected to be an object or array in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  resolve_tile_inversion__(p_tiles);
}

size_t wfc::Parser::parse_positive_int__(const json& section, const std::string& p_key,
                                       const std::string& p_path) const {

  if (!section.contains(p_key)) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  if (section[p_key].type() != json::value_t::number_unsigned) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" was expected to be a positive integer", p_path.c_str(), p_key.c_str());
    throw std::runtime_error(msg);
  }

  return section[p_key];
}

std::string wfc::Parser::parse_string__(const json& section, const std::string& p_key,
                                        const std::string& p_path) const {

  if (!section.contains(p_key)) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  if (section[p_key].type() != json::value_t::string) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" was expected to be a string", p_path.c_str(), p_key.c_str());
    throw std::runtime_error(msg);
  }

  return section[p_key];
}

bool wfc::Parser::parse_bool__(const json& section, const std::string& p_key,
                                        const std::string& p_path) const {

  if (!section.contains(p_key)) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  if (section[p_key].type() != json::value_t::boolean) {
    char msg[500];
    sprintf(msg, "path \"%s/%s\" was expected to be a boolean", p_path.c_str(), p_key.c_str());
    throw std::runtime_error(msg);
  }

  return section[p_key];
}

void wfc::Parser::parse_tile_rules__(TileInfo& tile, const json& section) const {
  using wfc::Directions;
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

  for (const auto& [key, value] : dir_map) {
    std::string negation_value = "!" + value;

    if (section.contains(negation_value)) {
      std::vector<std::string> rule_vec = section[negation_value].get<std::vector<std::string>>();
      std::unordered_set<std::string> rules(rule_vec.begin(), rule_vec.end());
      tile.rules[key] = rules;
      tile.directions_to_invert.insert(key);
    }
    else if (section.contains(value)) {
      std::vector<std::string> rule_vec = section[value].get<std::vector<std::string>>();
      std::unordered_set<std::string> rules(rule_vec.begin(), rule_vec.end());
      tile.rules[key] = rules;
    }
    else {
      tile.rules[key] = {};
    }
  }
}

void wfc::Parser::resolve_tile_inversion__(std::vector<TileInfo>& p_tiles) {
  std::unordered_set<std::string> all_tile_names;
  for (const TileInfo& tile : p_tiles) {
    all_tile_names.insert(tile.name);
  }

  for (TileInfo& tile : p_tiles) {
    for (const wfc::Directions& dir : tile.directions_to_invert) {
      tile.rules[dir] = set_difference(all_tile_names, tile.rules[dir]);
    }
  }
}

json wfc::Parser::open_sub_config__(const std::string& p_path) {
  check_config_file(p_path);

  std::ifstream config_stream(p_path);
  if (!config_stream) {
    char msg[500];
    sprintf(msg, "Failed to open sub config file at %s", p_path.c_str());
    throw std::runtime_error(msg);
  }

  json config_json = json::parse(config_stream);
  config_stream.close();

  return config_json;
}
