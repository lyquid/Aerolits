#ifndef KTP_SRC_INCLUDE_PALETTE_HPP_
#define KTP_SRC_INCLUDE_PALETTE_HPP_

#include <SDL.h>
#include <cmath> // sqrt
#include <string>
#include <vector>

namespace ktp {

namespace Colors {

  /* https://lospec.com/palette-list/slso-clr17 */

  inline constexpr SDL_Color black        {0x2e, 0x2c, 0x3b, 0xFF};
  inline constexpr SDL_Color dark_blue    {0x3e, 0x41, 0x5f, 0xFF};
  inline constexpr SDL_Color dark_grey    {0x55, 0x60, 0x7d, 0xFF};
  inline constexpr SDL_Color grey         {0x74, 0x7d, 0x88, 0xFF};
  inline constexpr SDL_Color copper_green {0x41, 0xde, 0x95, 0xFF};
  inline constexpr SDL_Color turquoise    {0x2a, 0xa4, 0xaa, 0xFF};
  inline constexpr SDL_Color blue         {0x3b, 0x77, 0xa6, 0xFF};
  inline constexpr SDL_Color dark_green   {0x24, 0x93, 0x37, 0xFF};
  inline constexpr SDL_Color green        {0x56, 0xbe, 0x44, 0xFF};
  inline constexpr SDL_Color yellow       {0xc6, 0xde, 0x78, 0xFF};
  inline constexpr SDL_Color dark_yellow  {0xf3, 0xc2, 0x20, 0xFF};
  inline constexpr SDL_Color orange       {0xc4, 0x65, 0x1c, 0xFF};
  inline constexpr SDL_Color red          {0xb5, 0x41, 0x31, 0xFF};
  inline constexpr SDL_Color violet       {0x61, 0x40, 0x7a, 0xFF};
  inline constexpr SDL_Color purple       {0x8f, 0x3d, 0xa7, 0xFF};
  inline constexpr SDL_Color pink         {0xea, 0x61, 0x9d, 0xFF};
  inline constexpr SDL_Color white        {0xc1, 0xe5, 0xea, 0xFF};

  inline const std::vector<SDL_Color> palette {
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

  inline auto colorDistance (const SDL_Color& color1, const SDL_Color& color2) {
    const auto rmean {(static_cast<long>(color1.r) + static_cast<long>(color2.r)) / 2};
    const auto r {static_cast<long>(color1.r) - static_cast<long>(color2.r)};
    const auto g {static_cast<long>(color1.g) - static_cast<long>(color2.g)};
    const auto b {static_cast<long>(color1.b) - static_cast<long>(color2.b)};

    return sqrt((((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8));
  }

  inline SDL_Color getNearestColor(const SDL_Color& color) {
    int position{};
    double lowest_distance{255 * 3};
    double current_distance{};
    
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

} // end namespace Colors

} // end namespace ktp

#endif // KTP_SRC_PALETTE_INCLUDE_HPP_