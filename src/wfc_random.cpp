#include "wfc_random.h"


void wfc::Random::seed(unsigned int p_seed) {

  /// Set seed for the static random engine

    get_engine__() = std::mt19937(p_seed);
}

int wfc::Random::int_from_range(int p_min, int p_max) {

  /// Check if min is less than max

  if (p_min >= p_max) {
    throw std::invalid_argument("min must be less than max");
  }

  /// Generate random integer in the range

  std::uniform_int_distribution<> dist(p_min, p_max - 1);
  return dist(get_engine__());
}

std::mt19937& wfc::Random::get_engine__() {

  /// Define the static random number generator

  static std::mt19937 gen{std::random_device{}()};
  return gen;
}
