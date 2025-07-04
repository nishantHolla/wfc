#include "wfc_parser.h"

#include <fstream>

wfc::Parser::Parser(const std::string& p_config_path) :
  config_path__(p_config_path) {

  std::ifstream config_stream(config_path__);
  if (!config_stream) {
    char msg[100];
    sprintf(msg, "Failed to open config file at %s", config_path__.c_str());
    throw std::runtime_error(msg);
  }

  config_json__ = json::parse(config_stream);
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
