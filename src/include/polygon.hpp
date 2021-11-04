#ifndef KTP_POLYGON_HPP_
#define KTP_POLYGON_HPP_

#include <vector>

namespace ktp {

template<typename T>
struct PointTemplate { T x, y; };

// integers
using SPoint   = PointTemplate<short int>;
using USPoint  = PointTemplate<unsigned short int>;
using Point    = PointTemplate<int>;
using UPoint   = PointTemplate<unsigned int>;
using LPoint   = PointTemplate<long int>;
using ULPoint  = PointTemplate<unsigned long int>;
using LLPoint  = PointTemplate<long long int>;
using ULLPoint = PointTemplate<unsigned long long int>;
// floating point
using FPoint = PointTemplate<float>;
using DPoint = PointTemplate<double>;

// void generateRegularPolygon(std::vector<> unsigned int sides) {
//   if (sides <= 2) return;
// }

} // namespace ktp

#endif // KTP_POLYGON_HPP_