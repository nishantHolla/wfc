#include "wfc_parser.h"
#include "wfc_sdl_utils.h"
#include "wfc_directions.h"
#include "wfc_test.h"
#include "wfc_log.h"
#include "wfc.h"

#include <string>
#include <filesystem>
#include <ostream>

std::ostream& operator<<(std::ostream& out, const std::unordered_set<std::string>& set) {
  for (auto& item : set) {
    out << item << " ";
  }
  out << "\n";
  return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& set) {
  for (auto& item : set) {
    out << item << " ";
  }
  out << "\n";
  return out;
}

void test(const std::string& p_config_path) {
  std::filesystem::path config_path(p_config_path);
  wfc::check_config_file(p_config_path);
  std::filesystem::current_path(config_path.parent_path());
  wfc::Parser parser(config_path.filename());

  /// Parse canvas
  wfc::CanvasInfo canvas_info;
  parser.parse_canvas(canvas_info);
  std::string dir_type_str = canvas_info.direction_type == wfc::DirectionType::QUAD_DIRECTIONS ? "quad" : "oct";
  printf("width: %zu, height: %zu, rows: %zu, columns: %zu, dir_type: %s\n",
         canvas_info.width,
         canvas_info.height,
         canvas_info.rows,
         canvas_info.columns,
         dir_type_str.c_str());
  test_case(true);

  /// Parse grousp
  wfc::GroupInfo groups;
  parser.parse_groups(groups);
  for (auto& [key, value]: groups.groups) {
    std::cout << key << ": " << value << "\n";
  }
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

  /// Parse constraints
  wfc::ConstraintInfo constraints;
  parser.parse_constraints(constraints);
  std::cout << "top right: " << constraints.top_right;
  std::cout << "bottom right: " << constraints.bottom_right;
  std::cout << "bottom left: " << constraints.bottom_left;
  std::cout << "top left: " << constraints.top_left;
  std::cout << "top: " << constraints.top;
  std::cout << "right: " << constraints.right;
  std::cout << "bottom: " << constraints.bottom;
  std::cout << "left: " << constraints.left;
  std::cout << "edges: " << constraints.edges;
  std::cout << "corners: " << constraints.corners;
  std::cout << "fixed: \n";
  for (auto& item : constraints.fixed) {
    std::cout << item.row << " " << item.column << " " << item.tiles;
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

