#include "wfc_canvas.h"
#include "wfc_directions.h"
#include "wfc_test.h"
#include "wfc_sdl_utils.h"
#include "wfc_log.h"

using wfc::DirectionType;
using wfc::Directions;
using wfc::Canvas;

void test() {
  Canvas canvas(480, 480, 20, 20);

  canvas.set_base_path("tiles");
  canvas.set_direction_type(DirectionType::QUAD_DIRECTIONS);

  /// Initialize all tiles

  canvas.add_tile("up", "up.png");
  canvas.add_tile("right", "right.png");
  canvas.add_tile("down", "down.png");
  canvas.add_tile("left", "left.png");
  canvas.add_tile("blank", "blank.png");

  /// Add rules for tile up

  canvas.add_rule("up", Directions::NORTH, {"right", "down", "left"});
  canvas.add_rule("up", Directions::EAST, {"up", "down", "left"});
  canvas.add_rule("up", Directions::SOUTH, {"blank", "down"});
  canvas.add_rule("up", Directions::WEST, {"up", "down", "right"});

  /// Add rules for tile right

  canvas.add_rule("right", Directions::NORTH, {"right", "down", "left"});
  canvas.add_rule("right", Directions::EAST, {"up", "down", "left"});
  canvas.add_rule("right", Directions::SOUTH, {"right", "up", "left"});
  canvas.add_rule("right", Directions::WEST, {"blank", "left"});

  /// Add rules for tile down

  canvas.add_rule("down", Directions::NORTH, {"blank", "up"});
  canvas.add_rule("down", Directions::EAST, {"left", "up", "down"});
  canvas.add_rule("down", Directions::SOUTH, {"up", "right", "left"});
  canvas.add_rule("down", Directions::WEST, {"right", "up", "down"});

  /// Add rules for tile left

  canvas.add_rule("left", Directions::NORTH, {"right", "down", "left"});
  canvas.add_rule("left", Directions::EAST, {"blank", "right"});
  canvas.add_rule("left", Directions::SOUTH, {"up", "left", "right"});
  canvas.add_rule("left", Directions::WEST, {"right", "down", "up"});

  /// Add rules for tile blank

  canvas.add_rule("blank", Directions::NORTH, {"blank", "up"});
  canvas.add_rule("blank", Directions::EAST, {"blank", "right"});
  canvas.add_rule("blank", Directions::SOUTH, {"blank", "down"});
  canvas.add_rule("blank", Directions::WEST, {"blank", "left"});

  /// Tests

  try {
    canvas.add_tile("blank", "blank.png");
    test_case(false);
  }
  catch(...) {
    test_case(true);
  }

  try {
    canvas.add_rule("tile", Directions::NORTH, "blank");
    test_case(false);
  }
  catch(...) {
    test_case(true);
  }

  try {
    canvas.add_rule("blank", Directions::EAST, {"blank", "tile"});
    test_case(false);
  }
  catch(...) {
    test_case(true);
  }

  test_results();
}

int main(void) {
  try {
    wfc::init_sdl();
  }
  catch (std::runtime_error& e) {
    wfc::Log::error("SDL Init faild");
    exit(1);
  }

  test();

  wfc::free_sdl();
  return 0;
}
