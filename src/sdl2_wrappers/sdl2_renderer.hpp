#ifndef KTP_SDL2_WRAPPERS_SDL2_RENDERER_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_RENDERER_HPP_

#include "sdl2_geometry.hpp"
#include "sdl2_window.hpp"
#include <SDL.h>
#include <box2d/box2d.h>
#include <string>
#include <utility> // std::exchange, std::move
#include <vector>

namespace ktp {

namespace SDL2Video {
  void logVideoDrivers();
}

class SDL2_Renderer {
 public:

  SDL2_Renderer() = default;
  SDL2_Renderer(const SDL2_Renderer&) = delete;
  SDL2_Renderer(SDL2_Renderer&& other) noexcept { *this = std::move(other); }
  ~SDL2_Renderer() { if (renderer_) SDL_DestroyRenderer(renderer_); }

  SDL2_Renderer& operator=(const SDL2_Renderer&) = delete;
  SDL2_Renderer& operator=(SDL2_Renderer&& other) noexcept;

  /**
  * Use this function to clear the current rendering target with the drawing color.
  * This function clears the entire rendering target, ignoring the viewport
  * and the clip rectangle.
  * @return True on success, or false on errors.
  */
  inline auto clear() const {
    return SDL_RenderClear(renderer_) == 0 ? true : false;
  }

  /**
  * Use this function to create a 2D rendering context for a window.
  * @param window The window where rendering is displayed.
  * @param flags The renderer flags.
  * @param scale_q The render scaling quality. Nearest, linear or best.
  * @return True on success, or false on errors.
  */
  bool create(const SDL2_Window& window, Uint32 flags, const std::string& scale_q = "nearest");

  /**
  * Use this function to create a 2D rendering context for a window.
  * @param window The window where rendering is displayed.
  * @param size The renderer logical size.
  * @param flags The renderer flags.
  * @param scale_q The render scaling quality.
  * @return True on success, or false on errors.
  */
  bool create(const SDL2_Window& window, const SDL_Point& size, Uint32 flags, const std::string& scale_q = "nearest");

  /**
   * @brief
   *
   * @param center
   * @param radius
   */
  void drawCircle(const b2Vec2& center, float radius) const;

  /**
   * @brief
   *
   * @param center
   * @param radius
   * @param color
   */
  void drawCircle(const SDL_Point& center, float radius) const;

  /**
   * @brief
   *
   * @param center
   * @param radius
   * @param color
   */
  void drawCircle(const SDL_FPoint& center, float radius) const;

  /**
   * @brief Draws 2 perpendicular lines intersecting in the middle of the screen.
   * @param color The desired color for the cross.
   */
  void drawCross() const;

  /**
   * @brief Draws a squared grid of the specified pixel size.
   * @param size The size in pixels.
   * @param color The desired color for the grid.
   */
  void drawGrid(int size) const;

  /**
   * @brief
   * @param start
   * @param end
   * @return true
   */
  inline auto drawLine(const SDL_Point& start, const SDL_Point& end) const {
    return SDL_RenderDrawLine(renderer_, start.x, start.y, end.x, end.y) == 0 ? true : false;
  }

  /**
   * @brief
   * @param start
   * @param end
   * @return true
   */
  inline auto drawLine(const SDL_FPoint& start, const SDL_FPoint& end) const {
    return SDL_RenderDrawLineF(renderer_, start.x, start.y, end.x, end.y) == 0 ? true : false;
  }

  /**
   * @brief
   * @param x1
   * @param y1
   * @param x2
   * @param y2
   * @return true
   * @return false
   */
  inline auto drawLine(int x1, int y1, int x2, int y2) const {
    return SDL_RenderDrawLine(renderer_, x1, y1, x2, y2) == 0 ? true : false;
  }

  /**
   * @brief
   * @param x1
   * @param y1
   * @param x2
   * @param y2
   * @return true
   * @return false
   */
  inline auto drawLine(float x1, float y1, float x2, float y2) const {
    return SDL_RenderDrawLine(renderer_, x1, y1, x2, y2) == 0 ? true : false;
  }

  /**
   * @brief
   * @tparam T
   * @param line
   * @return auto
   */
  template<typename T>
  inline auto drawLine(Geometry::Line<T> line) const {
    return SDL_RenderDrawLine(renderer_, line.begin.x, line.begin.y, line.end.x, line.end.y) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param points
   * @return true
   * @return false
   */
  inline auto drawLines(const std::vector<SDL_Point>& points) const {
    return SDL_RenderDrawLines(renderer_, points.data(), points.size()) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param points
   * @return true
   * @return false
   */
  inline auto drawLines(const std::vector<SDL_FPoint>& points) const {
    return SDL_RenderDrawLinesF(renderer_, points.data(), points.size()) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param point
   * @return true
   * @return false
   */
  inline auto drawPoint(const b2Vec2& point) const {
    return SDL_RenderDrawPointF(renderer_, point.x, point.y) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param point
   * @return true
   * @return false
   */
  inline auto drawPoint(const SDL_Point& point) const {
    return SDL_RenderDrawPoint(renderer_, point.x, point.y) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param point
   * @return true
   * @return false
   */
  inline auto drawPoint(const SDL_FPoint& point) const {
    return SDL_RenderDrawPointF(renderer_, point.x, point.y) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param x
   * @param y
   * @return true
   * @return false
   */
  inline auto drawPoint(int x, int y) const {
    return SDL_RenderDrawPoint(renderer_, x, y) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param x
   * @param y
   * @return true
   * @return false
   */
  inline auto drawPoint(float x, float y) const {
    return SDL_RenderDrawPointF(renderer_, x, y) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param points
   * @return true
   * @return false
   */
  inline auto drawPoints(const std::vector<SDL_Point>& points) const {
    return SDL_RenderDrawPoints(renderer_, points.data(), points.size()) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param points
   * @return true
   * @return false
   */
  inline auto drawPoints(const std::vector<SDL_FPoint>& points) const {
    return SDL_RenderDrawPointsF(renderer_, points.data(), points.size()) == 0 ? true : false;
  }

  /**
   * @brief
   *
   * @param rect
   * @return true
   * @return false
   */
  inline auto drawRect(const SDL_Rect& rect) const {
    return SDL_RenderDrawRect(renderer_, &rect) ? true : false;
  }

  /**
   * @brief
   *
   * @param rect
   * @return true
   * @return false
   */
  inline auto drawRectFill(const SDL_Rect& rect) const {
    return SDL_RenderFillRect(renderer_, &rect) == 0 ? true : false;
  }

  /**
   * @brief Get the Renderer object
   *
   * @return auto
   */
  inline auto renderer() const { return renderer_; }

  /**
  * Use this function to update the screen with any rendering performed since
  * the previous call.
  *
  * SDL's rendering functions operate on a backbuffer; that is, calling a
  * rendering function such as SDL_RenderDrawLine() does not directly put a
  * line on the screen, but rather updates the backbuffer. As such, you compose
  * your entire scene and present the composed backbuffer to the screen as a
  * complete picture.
  *
  * Therefore, when using SDL's rendering API, one does all drawing intended
  * for the frame, and then calls this function once per frame to present the
  * final drawing to the user.
  *
  * The backbuffer should be considered invalidated after each present; do not
  * assume that previous contents will exist between frames. You are strongly
  * encouraged to call SDL_RenderClear() to initialize the backbuffer before
  * starting each new frame's drawing, even if you plan to overwrite every pixel.
  */
  inline void present() const { SDL_RenderPresent(renderer_); }

  /**
   * Use this function to set the blend mode used for drawing operations (Fill and Line).
   *
   * @param mode The SDL_BlendMode to use for blending.
   * @return Returns true on success or false on failure.
   */
  inline auto setBlendMode(const SDL_BlendMode& mode) const {
    return SDL_SetRenderDrawBlendMode(renderer_, mode) == 0 ? true : false;
  }

  /**
  * Use this function to set the color used for drawing operations.
  * This function can set the color for drawing or filling rectangles, lines,
  * and points, and for SDL_RenderClear().
  * @param color The b2Color used to draw on the rendering target.
  * @return True on success, or false on errors.
  */
  inline auto setDrawColor(const b2Color& color) const {
    return SDL_SetRenderDrawColor(renderer_, color.r * 255, color.g * 255, color.b * 255, color.a * 255) == 0 ? true : false;
  }

  /**
  * Use this function to set the color used for drawing operations.
  * This function can set the color for drawing or filling rectangles, lines,
  * and points, and for SDL_RenderClear().
  * @param color The SDL_Color used to draw on the rendering target.
  * @return True on success, or false on errors.
  */
  inline auto setDrawColor(const SDL_Color& color) const {
    return SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a) == 0 ? true : false;
  }

  /**
  * Use this function to set the color used for drawing operations.
  * This function can set the color for drawing or filling rectangles, lines,
  * and points, and for SDL_RenderClear().
  * @param r The red value used to draw on the rendering target.
  * @param g The green value used to draw on the rendering target.
  * @param b The blue value used to draw on the rendering target.
  * @param a The alpha value used to draw on the rendering target; usually
  *          SDL_ALPHA_OPAQUE (255). Use SDL_SetRenderDrawBlendMode to specify
  *          how the alpha channel is used.
  * @return True on success, or false on errors.
  */
  inline auto setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
    return SDL_SetRenderDrawColor(renderer_, r, g, b, a) == 0 ? true : false;
  }

 private:

  /**
   * @brief Get information about a rendering context.
   * @return True if the query was successfull.
   */
  bool getRendererInfo();

  SDL_Renderer*    renderer_ {nullptr};
  SDL_RendererInfo renderer_info_ {};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_RENDERER_HPP_
