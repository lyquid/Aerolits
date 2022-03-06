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

inline const Color black        {0x2e, 0x2c, 0x3b, 0xFF};
inline const Color dark_blue    {0x3e, 0x41, 0x5f, 0xFF};
inline const Color dark_grey    {0x55, 0x60, 0x7d, 0xFF};
inline const Color grey         {0x74, 0x7d, 0x88, 0xFF};
inline const Color copper_green {0x41, 0xde, 0x95, 0xFF};
inline const Color turquoise    {0x2a, 0xa4, 0xaa, 0xFF};
inline const Color blue         {0x3b, 0x77, 0xa6, 0xFF};
inline const Color dark_green   {0x24, 0x93, 0x37, 0xFF};
inline const Color green        {0x56, 0xbe, 0x44, 0xFF};
inline const Color yellow       {0xc6, 0xde, 0x78, 0xFF};
inline const Color dark_yellow  {0xf3, 0xc2, 0x20, 0xFF};
inline const Color orange       {0xc4, 0x65, 0x1c, 0xFF};
inline const Color red          {0xb5, 0x41, 0x31, 0xFF};
inline const Color violet       {0x61, 0x40, 0x7a, 0xFF};
inline const Color purple       {0x8f, 0x3d, 0xa7, 0xFF};
inline const Color pink         {0xea, 0x61, 0x9d, 0xFF};
inline const Color white        {0xc1, 0xe5, 0xea, 0xFF};

inline const ColorsVector palette {
  {0x2e, 0x2c, 0x3b, 0xFF},
  {0x3e, 0x41, 0x5f, 0xFF},
  {0x55, 0x60, 0x7d, 0xFF},
  {0x74, 0x7d, 0x88, 0xFF},
  {0x41, 0xde, 0x95, 0xFF},
  {0x2a, 0xa4, 0xaa, 0xFF},
  {0x3b, 0x77, 0xa6, 0xFF},
  {0x24, 0x93, 0x37, 0xFF},
  {0x56, 0xbe, 0x44, 0xFF},
  {0xc6, 0xde, 0x78, 0xFF},
  {0xf3, 0xc2, 0x20, 0xFF},
  {0xc4, 0x65, 0x1c, 0xFF},
  {0xb5, 0x41, 0x31, 0xFF},
  {0x61, 0x40, 0x7a, 0xFF},
  {0x8f, 0x3d, 0xa7, 0xFF},
  {0xea, 0x61, 0x9d, 0xFF},
  {0xc1, 0xe5, 0xea, 0xFF}
};

inline auto colorDistance(const Color& color1, const Color& color2) {
  const auto rmean {(static_cast<long>(color1.r * 255) + static_cast<long>(color2.r * 255)) / 2};
  const auto r {static_cast<long>(color1.r * 255) - static_cast<long>(color2.r * 255)};
  const auto g {static_cast<long>(color1.g * 255) - static_cast<long>(color2.g * 255)};
  const auto b {static_cast<long>(color1.b * 255) - static_cast<long>(color2.b * 255)};

  return std::sqrt((((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8));
}

inline auto getNearestColor(const Color& color) {
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

inline glm::vec4 colorToGlmVec4(const Color& color) {
  return {color.r, color.g, color.b, color.a};
}

inline void colorToSDL2Color(const Color& color, SDL_Color& sdl_color) {
  sdl_color.r = static_cast<Uint8>(color.r * 255.f);
  sdl_color.g = static_cast<Uint8>(color.g * 255.f);
  sdl_color.b = static_cast<Uint8>(color.b * 255.f);
  sdl_color.a = static_cast<Uint8>(color.a * 255.f);
}

inline SDL_Color colorToSDL2Color(const Color& color) {
  return SDL_Color {
    static_cast<Uint8>(color.r * 255.f),
    static_cast<Uint8>(color.g * 255.f),
    static_cast<Uint8>(color.b * 255.f),
    static_cast<Uint8>(color.a * 255.f)
  };
}

inline void b2ColorToSDL2Color(const b2Color& orig, SDL_Color& dest) {
  dest.r = static_cast<Uint8>(orig.r * 255.f);
  dest.g = static_cast<Uint8>(orig.g * 255.f);
  dest.b = static_cast<Uint8>(orig.b * 255.f);
  dest.a = static_cast<Uint8>(orig.a * 255.f);
}

inline SDL_Color b2ColorToSDL2Color(const b2Color& orig) {
  return SDL_Color {
    static_cast<Uint8>(orig.r * 255.f),
    static_cast<Uint8>(orig.g * 255.f),
    static_cast<Uint8>(orig.b * 255.f),
    static_cast<Uint8>(orig.a * 255.f)
  };
}

inline Color glmVec4ToColor(const glm::vec4& color) {
  return {color.r, color.r, color.b, color.a};
}

inline void SDL2ColorToB2Color(const SDL_Color& orig, b2Color& dest) {
  constexpr float inv {1.f / 255.f};
  dest.r = orig.r * inv;
  dest.g = orig.g * inv;
  dest.b = orig.b * inv;
  dest.a = orig.a * inv;
}

inline b2Color SDL2ColorToB2Color(const SDL_Color& orig) {
  constexpr float inv {1.f / 255.f};
  return b2Color {orig.r * inv, orig.g * inv, orig.b * inv, orig.a * inv};
}

} // end namespace Palette
} // end namespace ktp

#endif // KTP_SRC_PALETTE_INCLUDE_HPP_
