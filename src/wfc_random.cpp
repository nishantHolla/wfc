#include "wfc_random.h"


void wfc::Random::seed(unsigned int p_seed) {
    get_engine__() = std::mt19937(p_seed);
}

int wfc::Random::int_from_range(int p_min, int p_max) {
  if (p_min >= p_max) {
    throw std::invalid_argument("min must be less than max");
  }

  std::uniform_int_distribution<> dist(p_min, p_max - 1);
  return dist(get_engine__());
}

std::mt19937& wfc::Random::get_engine__() {
  static std::mt19937 gen{std::random_device{}()};
  return gen;
}
