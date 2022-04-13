#ifndef KTP_COLORS_HPP_
#define KTP_COLORS_HPP_

namespace ktp {

/**
 * @brief A RGBA color representation with range [0,1] for channel.
 */
class Color {
 public:

  Color(float red = 0.f, float green = 0.f, float blue = 0.f, float alpha = 1.f):
    r(red), g(green), b(blue), a(alpha) {}

  Color(int red, int green, int blue, int alpha = 255):
    r((float)red * kInv), g((float)green * kInv), b((float)blue * kInv), a((float)alpha * kInv) {}

  Color(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha = 255u):
    r((float)red * kInv), g((float)green * kInv), b((float)blue * kInv), a((float)alpha * kInv) {}

  float r, g, b, a;

  /**
   * @brief Gives you the inverse of 255.
   * @return Something like 0.003921568859f.
   */
  static auto inv255() { return kInv; }

 private:

  static constexpr float kInv {1.f / 255.f};
};

} // end namespace ktp

#endif // KTP_COLORS_HPP_
