#ifndef KTP_SRC_PALETTE_HPP_
#define KTP_SRC_PALETTE_HPP_

#include <SDL.h>

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
}

} // end namespace ktp

#endif // KTP_SRC_PALETTE_HPP_