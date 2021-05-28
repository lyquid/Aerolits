#ifndef KTP_SDL2_WRAPPERS_SDL2_RENDERER_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_RENDERER_HPP_

#include "sdl2_window.hpp"
#include <SDL.h>
#include <memory>
#include <string>
#include <vector>

namespace ktp {

template <typename T> struct deleter;

template <> struct deleter<SDL_Renderer> {
  void operator()(SDL_Renderer* ren) { 
    SDL_DestroyRenderer(ren);
    ren = nullptr;
  }
};

class SDL2_Renderer {
 public:

  /**
  * Use this function to clear the current rendering target with the drawing color. 
  * This function clears the entire rendering target, ignoring the viewport 
  * and the clip rectangle.
  * @return True on success, or false on errors.
  */
  bool clear() const;

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

  void drawCross(const SDL_Color& color) const ;

  void drawGrid(int size, const SDL_Color& color) const ;
  
  bool drawLine(const SDL_Point& start, const SDL_Point& end) const;

  bool drawLine(int x1, int y1, int x2, int y2) const;

  bool drawLines(const std::vector<SDL_Point>& points) const;
  
  bool drawLines(const std::vector<SDL_FPoint>& points) const;

  bool drawPoint(const SDL_Point& point) const;

  bool drawPoint(const SDL_FPoint& point) const;
  
  bool drawPoints(const std::vector<SDL_Point>& points) const;

  bool drawPoints(const std::vector<SDL_FPoint>& points) const;
  
  bool drawRect(const SDL_Rect& rect) const;

  bool drawRectFill(const SDL_Rect& rect) const;

  inline SDL_Renderer* getRenderer() const { return renderer_.get(); }

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
  inline void present() const { SDL_RenderPresent(renderer_.get()); }

  /**
   * Use this function to set the blend mode used for drawing operations (Fill and Line).
   * 
   * @param mode The SDL_BlendMode to use for blending 
   * @return Returns 0 on success or a negative error code on failure 
   */
  inline int setBlendMode(const SDL_BlendMode& mode) const { return SDL_SetRenderDrawBlendMode(renderer_.get(), mode); }

  /**
  * Use this function to set the color used for drawing operations.
  * This function can set the color for drawing or filling rectangles, lines, 
  * and points, and for SDL_RenderClear().
  * @param color The SDL_Color used to draw on the rendering target.
  * @return True on success, or false on errors.
  */
  bool setDrawColor(const SDL_Color& color) const;

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
  bool setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;
  
 private:

  bool getRendererInfo();

  template <typename T>
  using unique_ptr_deleter = std::unique_ptr<T, deleter<T>>;

  unique_ptr_deleter<SDL_Renderer> renderer_{nullptr};
  SDL_RendererInfo renderer_info_{};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_RENDERER_HPP_