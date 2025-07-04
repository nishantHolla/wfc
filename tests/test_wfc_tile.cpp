#include "wfc_tile.h"
#include "wfc_directions.h"
#include "wfc_sdl_utils.h"
#include "wfc_test.h"
#include "wfc_log.h"

#include <cassert>
#include <SDL2/SDL.h>

using wfc::Directions;
using wfc::Tile;

void test() {
  SDL_Window* window = SDL_CreateWindow("",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        1, 1, SDL_WINDOW_HIDDEN);

  if (!window) {
    wfc::Log::error("SDL Window init faild");
    exit(1);
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    wfc::Log::error("SDL Renderer init faild");
    exit(1);
  }

  /// Initialize all tiles

  Tile up("tiles/up.png", renderer);
  Tile right("tiles/right.png", renderer);
  Tile down("tiles/down.png", renderer);
  Tile left("tiles/left.png", renderer);
  Tile blank("tiles/blank.png", renderer);

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
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
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
