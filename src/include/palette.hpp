#ifndef KTP_SRC_INCLUDE_PALETTE_HPP_
#define KTP_SRC_INCLUDE_PALETTE_HPP_

#include "colors.hpp"
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <SDL.h>
#include <cmath> // sqrt
#include <vector>

namespace ktp {

using ColorsVector = std::vector<Color>;

namespace Palette {

/* https://lospec.com/palette-list/slso-clr17 */

inline constexpr Color black        {0x2e, 0x2c, 0x3b, 0xff};
inline constexpr Color dark_blue    {0x3e, 0x41, 0x5f, 0xff};
inline constexpr Color dark_grey    {0x55, 0x60, 0x7d, 0xff};
inline constexpr Color grey         {0x74, 0x7d, 0x88, 0xff};
inline constexpr Color copper_green {0x41, 0xde, 0x95, 0xff};
inline constexpr Color turquoise    {0x2a, 0xa4, 0xaa, 0xff};
inline constexpr Color blue         {0x3b, 0x77, 0xa6, 0xff};
inline constexpr Color dark_green   {0x24, 0x93, 0x37, 0xff};
inline constexpr Color green        {0x56, 0xbe, 0x44, 0xff};
inline constexpr Color yellow       {0xc6, 0xde, 0x78, 0xff};
inline constexpr Color dark_yellow  {0xf3, 0xc2, 0x20, 0xff};
inline constexpr Color orange       {0xc4, 0x65, 0x1c, 0xff};
inline constexpr Color red          {0xb5, 0x41, 0x31, 0xff};
inline constexpr Color violet       {0x61, 0x40, 0x7a, 0xff};
inline constexpr Color purple       {0x8f, 0x3d, 0xa7, 0xff};
inline constexpr Color pink         {0xea, 0x61, 0x9d, 0xff};
inline constexpr Color white        {0xc1, 0xe5, 0xea, 0xff};

inline const ColorsVector palette {
  {0x2e, 0x2c, 0x3b, 0xff},
  {0x3e, 0x41, 0x5f, 0xff},
  {0x55, 0x60, 0x7d, 0xff},
  {0x74, 0x7d, 0x88, 0xff},
  {0x41, 0xde, 0x95, 0xff},
  {0x2a, 0xa4, 0xaa, 0xff},
  {0x3b, 0x77, 0xa6, 0xff},
  {0x24, 0x93, 0x37, 0xff},
  {0x56, 0xbe, 0x44, 0xff},
  {0xc6, 0xde, 0x78, 0xff},
  {0xf3, 0xc2, 0x20, 0xff},
  {0xc4, 0x65, 0x1c, 0xff},
  {0xb5, 0x41, 0x31, 0xff},
  {0x61, 0x40, 0x7a, 0xff},
  {0x8f, 0x3d, 0xa7, 0xff},
  {0xea, 0x61, 0x9d, 0xff},
  {0xc1, 0xe5, 0xea, 0xff}
};

inline const auto colorDistance(const Color& color1, const Color& color2) {
  const auto rmean {(static_cast<long>(color1.r * 255) + static_cast<long>(color2.r * 255)) / 2};
  const auto r {static_cast<long>(color1.r * 255) - static_cast<long>(color2.r * 255)};
  const auto g {static_cast<long>(color1.g * 255) - static_cast<long>(color2.g * 255)};
  const auto b {static_cast<long>(color1.b * 255) - static_cast<long>(color2.b * 255)};

  return std::sqrt((((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8));
}

inline const auto getNearestColor(const Color& color) {
  int position {};
  double lowest_distance {255 * 3};
  double current_distance {};

  for (auto i = 0u; i < palette.size(); ++i) {
    current_distance = colorDistance(color, palette[i]);
    if (current_distance < lowest_distance) {
      lowest_distance = current_distance;
      position = i;
    }
  }
  /* logMessage("The nearest color to {"
    + std::to_string(color.r) + ", "
    + std::to_string(color.g) + ", "
    + std::to_string(color.b) + "} is {"
    + std::to_string(palette[position].r) + ", "
    + std::to_string(palette[position].g) + ", "
    + std::to_string(palette[position].b)
    + "} at position " + std::to_string(position + 1) + "."); */
  return palette[position];
}

constexpr glm::vec4 colorToGlmVec4(const Color& color) {
  return {color.r, color.g, color.b, color.a};
}

constexpr void colorToSDL2Color(const Color& color, SDL_Color& sdl_color) {
  sdl_color.r = static_cast<Uint8>(color.r * 255.f);
  sdl_color.g = static_cast<Uint8>(color.g * 255.f);
  sdl_color.b = static_cast<Uint8>(color.b * 255.f);
  sdl_color.a = static_cast<Uint8>(color.a * 255.f);
}

constexpr SDL_Color colorToSDL2Color(const Color& color) {
  return SDL_Color {
    static_cast<Uint8>(color.r * 255.f),
    static_cast<Uint8>(color.g * 255.f),
    static_cast<Uint8>(color.b * 255.f),
    static_cast<Uint8>(color.a * 255.f)
  };
}

constexpr void b2ColorToSDL2Color(const b2Color& orig, SDL_Color& dest) {
  dest.r = static_cast<Uint8>(orig.r * 255.f);
  dest.g = static_cast<Uint8>(orig.g * 255.f);
  dest.b = static_cast<Uint8>(orig.b * 255.f);
  dest.a = static_cast<Uint8>(orig.a * 255.f);
}

constexpr SDL_Color b2ColorToSDL2Color(const b2Color& orig) {
  return SDL_Color {
    static_cast<Uint8>(orig.r * 255.f),
    static_cast<Uint8>(orig.g * 255.f),
    static_cast<Uint8>(orig.b * 255.f),
    static_cast<Uint8>(orig.a * 255.f)
  };
}

constexpr Color glmVec4ToColor(const glm::vec4& color) {
  return {color.r, color.r, color.b, color.a};
}

constexpr void SDL2ColorToB2Color(const SDL_Color& orig, b2Color& dest) {
  dest.r = orig.r * Color::inv255();
  dest.g = orig.g * Color::inv255();
  dest.b = orig.b * Color::inv255();
  dest.a = orig.a * Color::inv255();
}

inline const b2Color SDL2ColorToB2Color(const SDL_Color& orig) {
  return b2Color {orig.r * Color::inv255(), orig.g * Color::inv255(), orig.b * Color::inv255(), orig.a * Color::inv255()};
}

// INTERPOLATION

template<typename T>
constexpr T interpolateRange(T start, T end, T time_step) { return start + (end - start) * time_step; }

template<typename T, typename U>
constexpr T interpolateRange(T start, T end, U time_step) { return start + (end - start) * time_step; }

template<typename T>
constexpr T interpolateRange3(T start, T mid, T end, T time_step) {
  if (time_step < 0.5f) {
    return (mid * time_step * 2.f) + start * (0.5f - time_step) * 2.f;
  } else {
    return end * (time_step - 0.5f) * 2.f + mid * (1.f - time_step) * 2.f;
  }
}

template<typename T, typename U>
constexpr T interpolateRange3(T start, T mid, T end, U time_step) {
  if (time_step < 0.5) {
    return (mid * time_step * 2.0) + start * (0.5 - time_step) * 2.0;
  } else {
    return end * (time_step - 0.5) * 2.0 + mid * (1.0 - time_step) * 2.0;
  }
}

constexpr glm::vec4 interpolate2Colors(const glm::vec4& start_color, const glm::vec4& end_color, float time_step) {
  return {
    interpolateRange(start_color.r, end_color.r, time_step),
    interpolateRange(start_color.g, end_color.g, time_step),
    interpolateRange(start_color.b, end_color.b, time_step),
    interpolateRange(start_color.a, end_color.a, time_step)
  };
}

constexpr glm::vec4 interpolate3Colors(const glm::vec4& start_color, const glm::vec4& mid_color, const glm::vec4& end_color, float time_step) {
  return {
    interpolateRange3(start_color.r, mid_color.r, end_color.r, time_step),
    interpolateRange3(start_color.g, mid_color.g, end_color.g, time_step),
    interpolateRange3(start_color.b, mid_color.b, end_color.b, time_step),
    interpolateRange3(start_color.a, mid_color.a, end_color.a, time_step)
  };
}

} // end namespace Palette
} // end namespace ktp

#endif // KTP_SRC_PALETTE_INCLUDE_HPP_
