#include "wfc_tile.h"
#include "wfc_directions.h"
#include "wfc_test.h"

#include <cassert>

using wfc::Directions;
using wfc::Tile;

int main(void) {
  /// Initialize all tiles

  Tile up("tiles/up.jpg");
  Tile right("tiles/right.jpg");
  Tile down("tiles/down.jpg");
  Tile left("tiles/left.jpg");
  Tile blank("tiles/blank.jpg");

  /// Add rules for tile up

  up.add_rule(Directions::NORTH, {&right, &down, &left});
  up.add_rule(Directions::EAST, {&up, &down, &left});
  up.add_rule(Directions::SOUTH, {&blank, &down});
  up.add_rule(Directions::WEST, {&up, &down, &right});

  /// Add rules for tile right

  right.add_rule(Directions::NORTH, {&right, &down, &left});
  right.add_rule(Directions::EAST, {&up, &down, &left});
  right.add_rule(Directions::SOUTH, {&right, &up, &left});
  right.add_rule(Directions::WEST, {&blank, &left});

  /// Add rules for tile down

  down.add_rule(Directions::NORTH, {&blank, &up});
  down.add_rule(Directions::EAST, {&left, &up, &down});
  down.add_rule(Directions::SOUTH, {&up, &right, &left});
  down.add_rule(Directions::WEST, {&right, &up, &down});

  /// Add rules for tile left

  left.add_rule(Directions::NORTH, {&right, &down, &left});
  left.add_rule(Directions::EAST, {&blank, &right});
  left.add_rule(Directions::SOUTH, {&up, &left, &right});
  left.add_rule(Directions::WEST, {&right, &down, &up});

  /// Add rules fo tile blank

  blank.add_rule(Directions::NORTH, {&blank, &up});
  blank.add_rule(Directions::EAST, {&blank, &right});
  blank.add_rule(Directions::SOUTH, {&blank, &down});
  blank.add_rule(Directions::WEST, {&blank, &left});

  /// Tests

  test_case(
    up.check_rule(Directions::NORTH, &left) == true
  );

  test_case(
    up.check_rule(Directions::EAST, &right) == false
  );

  test_case(
    right.check_rule(Directions::EAST, &right) == false
  );

  test_case(
    right.check_rule(Directions::WEST, &blank) == true
  );

  test_case(
    down.check_rule(Directions::WEST, &blank) == false
  );

  test_case(
    down.check_rule(Directions::NORTH, &up) == true
  );

  test_case(
    right.check_rule(Directions::SOUTH, &right) == true
  );

  test_case(
    right.check_rule(Directions::NORTH, &down) == true
  );

  test_results();
  return 0;
}
