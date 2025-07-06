#include "wfc_parser.h"
#include "wfc_canvas.h"
#include "wfc.h"

#include <fstream>

using json = nlohmann::json;

wfc::Parser::Parser(const std::string& p_config_path) :
  config_path__(p_config_path) {

  std::ifstream config_stream(config_path__);
  if (!config_stream) {
    char msg[100];
    sprintf(msg, "Failed to open config file at %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  config_json__ = json::parse(config_stream);
  config_stream.close();
}

void wfc::Parser::parse_canvas(size_t& p_width, size_t& p_height, size_t& p_rows, size_t& p_cols) const {
  if (!config_json__.contains("canvas")) {
    char msg[100];
    sprintf(msg, "Path \"/canvas\" is missing in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  auto& section = config_json__["canvas"];
  p_width = parse_positive_int__(section, "width", "/canvas");
  p_height = parse_positive_int__(section, "height", "/canvas");
  p_rows = parse_positive_int__(section, "rows", "/canvas");
  p_cols = parse_positive_int__(section, "columns", "/canvas");
}

void wfc::Parser::parse_settings(wfc::DirectionType& p_direction_type, std::string& p_base_path) {
  if (!config_json__.contains("settings")) {
    char msg[100];
    sprintf(msg, "Path \"/settings\" is missing in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  auto& section = config_json__["settings"];
  bool is_quad = parse_bool__(section, "quad_directions", "/settings");
  p_direction_type = is_quad ? wfc::DirectionType::QUAD_DIRECTIONS : wfc::DirectionType::OCT_DIRECTIONS;
  p_base_path = parse_string__(section, "base_path", "/settings");
}

void wfc::Parser::parse_tiles(std::vector<TileInfo>& p_tiles) {
  if (!config_json__.contains("tiles")) {
    char msg[100];
    sprintf(msg, "Path \"/tiles\" is missing in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  auto parse_tile_section = [this](const json& section, std::vector<TileInfo>& p_tiles){
    for (auto& [key, item]: section.items()) {
      const std::string path = parse_string__(item, "path", "/tiles/" + key);
      wfc::TileInfo tile(key, path);

      if (!item.contains("rules")) {
        char msg[100];
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
      json sub_section = open_sub_section__(v);
      parse_tile_section(sub_section, p_tiles);
    }
  }
  else {
    char msg[100];
    sprintf(msg, "Path \"/tiles\" is expected to be an object or array in config file %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }
}

size_t wfc::Parser::parse_positive_int__(const json& section, const std::string& p_key,
                                       const std::string& p_path) const {

  if (!section.contains(p_key)) {
    char msg[100];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  if (section[p_key].type() != json::value_t::number_unsigned) {
    char msg[100];
    sprintf(msg, "path \"%s/%s\" was expected to be a positive integer", p_path.c_str(), p_key.c_str());
    throw std::runtime_error(msg);
  }

  return section[p_key];
}

std::string wfc::Parser::parse_string__(const json& section, const std::string& p_key,
                                        const std::string& p_path) const {

  if (!section.contains(p_key)) {
    char msg[100];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  if (section[p_key].type() != json::value_t::string) {
    char msg[100];
    sprintf(msg, "path \"%s/%s\" was expected to be a string", p_path.c_str(), p_key.c_str());
    throw std::runtime_error(msg);
  }

  return section[p_key];
}

bool wfc::Parser::parse_bool__(const json& section, const std::string& p_key,
                                        const std::string& p_path) const {

  if (!section.contains(p_key)) {
    char msg[100];
    sprintf(msg, "path \"%s/%s\" is missing in config file %s", p_path.c_str(), p_key.c_str(),
            config_path__.c_str());
    throw std::runtime_error(msg);
  }

  if (section[p_key].type() != json::value_t::boolean) {
    char msg[100];
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
    if (section.contains(value)) {
      tile.rules[key] = section[value].get<std::vector<std::string>>();
    }
    else {
      tile.rules[key] = {};
    }
  }
}

json wfc::Parser::open_sub_section__(const std::string& p_path) {
  check_config_file(p_path);

  std::ifstream config_stream(p_path);
  if (!config_stream) {
    char msg[100];
    sprintf(msg, "Failed to open sub config file at %s", p_path.c_str());
    throw std::runtime_error(msg);
  }

  json config_json = json::parse(config_stream);
  config_stream.close();

  return config_json;
}
