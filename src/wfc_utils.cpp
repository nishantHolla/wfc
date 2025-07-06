#include "wfc_utils.h"

#include <random>

int wfc::get_rand_int(int min, int max) {
  return (std::rand() % max) + min;
}
