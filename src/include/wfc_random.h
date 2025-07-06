#ifndef WFC_RANDOM_H_
#define WFC_RANDOM_H_

#include <random>
#include <stdexcept>

namespace wfc {

class Random {
public:
  /*
   * Set the seed for the random number generator
   *
   * Params:
   *       unsigned int p_seed: Seed value to set
   */
  static void seed(unsigned int p_seed);

  /*
   * Generate a random integer from the range [min, max)
   *
   * Params:
   *       int p_min: Min value of the range
   *       int p_max: Max value of the range
   *
   * Returns:
   *        Random integer in the range
   */
  static int int_from_range(int p_min, int p_max);

private:
  /*
   * Get the random number generator engine
   *
   * Returns:
   *        Static reference to the engine
   */
  static std::mt19937& get_engine__();
};

}

#endif // !WFC_RANDOM_H_
