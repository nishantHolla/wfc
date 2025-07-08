#ifndef WFC_DIRECTIONS_H_
#define WFC_DIRECTIONS_H_

#include <ostream>

namespace wfc {

enum class Directions {
  NORTH,
  NORTH_EAST,
  EAST,
  SOUTH_EAST,
  SOUTH,
  SOUTH_WEST,
  WEST,
  NORTH_WEST,
};

enum class DirectionType {
  QUAD_DIRECTIONS,
  OCT_DIRECTIONS
};

enum class Constraints {
  TOP,
  TOP_RIGHT,
  RIGHT,
  BOTTOM_RIGHT,
  BOTTOM,
  BOTTOM_LEFT,
  LEFT,
  TOP_LEFT,
  EDGES,
  CORNERS
};

}

std::ostream& operator<<(std::ostream& out, const wfc::Directions& dir);

#endif // !WFC_DIRECTIONS_H_
