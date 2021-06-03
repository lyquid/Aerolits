#ifndef AEROLITS_SRC_INCLUDE_RANDOM_HPP_
#define AEROLITS_SRC_INCLUDE_RANDOM_HPP_

#include <random> // std::rand

namespace ktp {

/**
 * @brief 
 * 
 * @param min 
 * @param max 
 * @return int 
 */
inline int generateRand(int min, int max) {
  //const auto random {rand() / RAND_MAX};
	//return min + (random * (max - min));
  return min + (rand() % (max - min + 1));
}

/**
 * @brief 
 * 
 * @param min 
 * @param max 
 * @return float 
 */
inline float generateRand(float min, float max) {
  const auto random {static_cast<float>(rand()) / static_cast<float>(RAND_MAX)};
	return min + (random * (max - min));
}

/**
 * @brief 
 * 
 * @param min 
 * @param max 
 * @return double 
 */
inline double generateRand(double min, double max) {
  const auto random{static_cast<double>(rand()) / static_cast<double>(RAND_MAX)};
	return min + (random * (max - min));
}

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_RANDOM_HPP_