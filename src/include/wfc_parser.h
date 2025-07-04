#ifndef WFC_PARSER_H_
#define WFC_PARSER_H_

#include <filesystem>
#include <string>

#include "external/json.hpp"

namespace wfc {
using json = nlohmann::json;


class Parser {
public:
  /*
   * Construct the parser with the given path to config file
   *
   * Params:
   *       String p_config_path: Path to the config file
   */
  Parser(const std::string& p_config_path);

  /*
   * Parse "canvas" section of the config file and store values in the given refrence
   *
   * Params:
   *      size_t p_width : Store width of the canvas
   *      size_t p_height: Store height of the canvas
   *      size_t p_rows  : Store number of rows in the canvas
   *      size_t p_colums: Store number of columns in the canvas
   *
   * Throws:
   *       If faild to open input stream
   */
  void parse_canvas(size_t& p_width, size_t& p_height, size_t& p_rows, size_t& p_cols) const;

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

};

}

#endif // !WFC_PARSER_H_
