#include "wfc_directions.h"
#include <ostream>

std::ostream& operator<<(std::ostream& out, const wfc::Directions& dir) {
  using wfc::Directions;

  switch(dir) {
    case Directions::NORTH: out << "NORTH"; break;
    case Directions::NORTH_EAST: out << "NORTH_EAST"; break;
    case Directions::EAST: out << "EAST"; break;
    case Directions::SOUTH_EAST: out << "SOUTH_EAST"; break;
    case Directions::SOUTH: out << "SOUTH"; break;
    case Directions::SOUTH_WEST: out << "SOUTH_WEST"; break;
    case Directions::WEST: out << "WEST"; break;
    case Directions::NORTH_WEST: out << "NORTH_WEST"; break;
  }
  return out;
}

