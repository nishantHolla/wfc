#include "wfc_parser.h"
#include "wfc_sdl_utils.h"
#include "wfc_directions.h"
#include "wfc_test.h"
#include "wfc_log.h"

#include <string>

void test(const std::string& p_config_path) {
  wfc::Parser parser(p_config_path);

  /// Parse canvas
  size_t width, height, rows, columns;
  parser.parse_canvas(width, height, rows, columns);
  printf("width: %zu, height: %zu, rows: %zu, columns: %zu\n", width, height, rows, columns);
  test_case(true);

  /// Parse settings
  wfc::DirectionType dir_type;
  std::string base_path;
  parser.parse_settings(dir_type, base_path);
  std::string dir_type_str = dir_type == wfc::DirectionType::QUAD_DIRECTIONS ? "quad" : "oct";
  printf("dir_type: %s, base_path: %s\n", dir_type_str.c_str(), base_path.c_str());
  test_case(true);

  /// Parse tiles
  std::vector<wfc::TileInfo> tiles;
  parser.parse_tiles(tiles);
  for (auto& tile : tiles) {
    printf("%s:\n\tpath: %s\n\trules:\n", tile.name.c_str(), tile.path.c_str());
    for (const auto& [key, value] : tile.rules) {
      if (value.size() == 0) {
        continue;
      }
      std::cout << "\t\t" << key << " {";
      for (auto& r : value) {
        std::cout << r << ",";
      }
      std::cout << "}\n";
    }
  }
  test_case(true);

  test_results();
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    wfc::Log::info("Test Usage: ./wfc-test </path/to/config.json>");
    exit(1);
  }

  test(argv[1]);
  return 0;
}

