#ifndef KTP_SDL2_WRAPPERS_SDL2_WINDOW_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_WINDOW_HPP_

#include <SDL.h>
#include <memory> // unique_ptr
#include <string>

namespace ktp {

template <typename T> struct deleter;

template <> struct deleter<SDL_Window> {
  void operator()(SDL_Window* window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
};

class SDL2_Window {
 public:

  /**
  * Creates a SDL_Window with the specified size.
  * @param size An SDL_Point structure with the desired size of the window.
  * @return True on success, or false on errors.
  */
  bool create(const SDL_Point& size) { return create("", size, 0); }

  /**
  * Creates a SDL_Window with the specified title and size.
  * @param title A string for use as title.
  * @param size An SDL_Point structure with the desired size of the window.
  * @return True on success, or false on errors.
  */
  bool create(const std::string& title, const SDL_Point& size) { return create(title, size, 0); }

  /**
  * Creates a SDL_Window with the specified title and size.
  * @param title A string for use as title.
  * @param size An SDL_Point structure with the desired size of the window.
  * @param flags The flags for the window, a mask of any of the following:
  * ::SDL_WINDOW_FULLSCREEN, ::SDL_WINDOW_OPENGL, ::SDL_WINDOW_HIDDEN,
  * ::SDL_WINDOW_BORDERLESS, ::SDL_WINDOW_RESIZABLE, ::SDL_WINDOW_MAXIMIZED,
  * ::SDL_WINDOW_MINIMIZED, ::SDL_WINDOW_INPUT_GRABBED, ::SDL_WINDOW_ALLOW_HIGHDPI,
  * ::SDL_WINDOW_VULKAN ::SDL_WINDOW_METAL.
  * @return True on success, or false on errors.
  */
  bool create(const std::string& title, const SDL_Point& size, Uint32 flags);

  /**
  * Creates a full screen SDL_Window with the specified title and size.
  * @param title A string for use as title.
  * @param size An SDL_Point structure with the desired size of the window.
  * @return True on success, or false on errors.
  */
  bool createFullScreen(const std::string& title, const SDL_Point& size) { return create(title, size, SDL_WINDOW_FULLSCREEN_DESKTOP); }

  /**
   * Use this function to get the window that currently has an input grab enabled.
   *
   * @return Returns the window if input is grabbed or NULL otherwise.
   */
  static inline SDL_Window* getGrabbedWindow() { return SDL_GetGrabbedWindow(); }

  /**
   * Use this function to get the size of a window's borders (decorations) around the
   * client area.
   *
   * @param top pointer to variable for storing the size of the top border; NULL is permitted
   * @param left pointer to variable for storing the size of the left border; NULL is permitted
   * @param bottom pointer to variable for storing the size of the bottom border; NULL is permitted
   * @param right pointer to variable for storing the size of the right border; NULL is permitted
   * @return Returns 0 on success or a negative error code on failure; call SDL_GetError()
   * for more information.
   */
  inline int getBordersSize(int* top, int* left, int* bottom, int* right) const {
    return SDL_GetWindowBordersSize(window_.get(), top, left, bottom, right);
  }

  /**
   * Use this function to get the brightness (gamma multiplier) for the display that
   * owns a given window.
   * Despite the name and signature, this method retrieves the brightness of the entire
   * display, not an individual window. A window is considered to be owned by the
   * display that contains the window's center pixel.
   * (The index of this display can be retrieved using SDL_GetWindowDisplayIndex().)
   *
   * @return Returns the brightness for the display where 0.0 is completely dark and
   * 1.0 is normal brightness.
   */
  inline float getBrightness() const { return SDL_GetWindowBrightness(window_.get()); }

  /**
   * Use this function to retrieve the data pointer associated with a window.
   *
   * @param name the name of the pointer
   * @return Returns the value associated with name.
   */
  inline void* getData(const std::string& name) const {
    return SDL_GetWindowData(window_.get(), name.c_str());
  }

  /**
   * Use this function to get the index of the display associated with a window.
   *
   * @return Returns the index of the display containing the center of the window on
   * success or a negative error code on failure; call SDL_GetError() for more information.
   */
  inline int getDisplayIndex() const { return SDL_GetWindowDisplayIndex(window_.get()); }

  /**
   * Use this function to fill in information about the display mode to use when a
   * window is visible at fullscreen.
   *
   * @param mode an SDL_DisplayMode structure filled in with the fullscreen display mode
   * @return Returns 0 on success or a negative error code on failure; call
   * SDL_GetError() for more information.
   */
  inline int getDisplayMode(SDL_DisplayMode* mode) const {
    return SDL_GetWindowDisplayMode(window_.get(), mode);
  }

  /**
   * Use this function to get the window flags.
   *
   * @return Returns a mask of the SDL_WindowFlags associated with window
   */
  inline Uint32 getFlags() const { return SDL_GetWindowFlags(window_.get()); }

  /**
   * Use this function to get a window from a stored ID.
   *
   * @param id the ID of the window
   * @return Returns the window associated with id or NULL if it doesn't exist; call
   * SDL_GetError() for more information.
   */
  static inline SDL_Window* getWindowFromID(Uint32 id) { return SDL_GetWindowFromID(id); }

  /**
   * Use this function to get the gamma ramp for the display that owns a given window.
   *
   * @param red a 256 element array of 16-bit quantities filled in with the translation table for the red channel, or NULL
   * @param green a 256 element array of 16-bit quantities filled in with the translation table for the green channel, or NULL
   * @param blue a 256 element array of 16-bit quantities filled in with the translation table for the blue channel, or NULL
   * @return Returns 0 on success or a negative error code on failure; call SDL_GetError()
   * for more information.
   */
  inline int getGammaRamp(Uint16* red, Uint16* green, Uint16* blue) const {
    return SDL_GetWindowGammaRamp(window_.get(), red, green, blue);
  }

  /**
   * Use this function to get a window's input grab mode.
   *
   * @return Returns true if input is grabbed, false otherwise.
   */
  inline bool getGrab() const { return SDL_GetWindowGrab(window_.get()); }

  /**
   * Use this function to get the numeric ID of a window, for logging purposes.
   *
   * Returns the ID of the window on success or 0 on failure; call SDL_GetError() for
   * more information.
   */
  inline Uint32 getID() const { return SDL_GetWindowID(window_.get()); }

  /**
   * Use this function to get the maximum size of a window's client area.
   *
   * @param w a pointer filled in with the maximum width of the window, may be NULL
   * @param h a pointer filled in with the maximum height of the window, may be NULL
   */
  inline void getMaximumSize(int* w, int* h) const { SDL_GetWindowMaximumSize(window_.get(), w, h); }

  /**
   * Use this function to get the maximum size of a window's client area.
   *
   * @return SDL_Point with the maximun width and height of the window.
   */
  inline SDL_Point getMaximumSize() const {
    int* w = nullptr; int* h = nullptr;
    SDL_GetWindowMaximumSize(window_.get(), w, h);
    return {*w, *h};
  }

  /**
   * Use this function to get the minimum size of a window's client area.
   *
   * @param w a pointer filled in with the minimum width of the window, may be NULL
   * @param h a pointer filled in with the minimum height of the window, may be NULL
   */
  inline void getMinimumSize(int* w, int* h) const { SDL_GetWindowMinimumSize(window_.get(), w, h); }

  /**
   * Use this function to get the minimum size of a window's client area.
   *
   * @return SDL_Point with the minimum width and height of the window.
   */
  inline SDL_Point getMinimumSize() const {
    int* w = nullptr; int* h = nullptr;
    SDL_GetWindowMinimumSize(window_.get(), w, h);
    return {*w, *h};
  }

  /**
   * Use this function to get the opacity of a window.
   *
   * @param opacity the float filled in (0.0f - transparent, 1.0f - opaque)
   * @return Returns 0 on success or a negative error code on failure; call
   * SDL_GetError() for more information.
   */
  inline int getOpacity(float* opacity) const { return SDL_GetWindowOpacity(window_.get(), opacity); }

  /**
   * Use this function to get the pixel format associated with the window.
   *
   * @return Returns the pixel format of the window on success or SDL_PIXELFORMAT_UNKNOWN
   * on failure; call SDL_GetError() for more information.
   */
  inline Uint32 getPixelFormat() const { return SDL_GetWindowPixelFormat(window_.get()); }

  /**
   * Use this function to get the position of a window.
   *
   * @param x a pointer filled in with the x position of the window, in screen coordinates, may be NULL
   * @param y a pointer filled in with the y position of the window, in screen coordinates, may be NULL
   */
  inline void getPosition(int* x, int* y) const { SDL_GetWindowPosition(window_.get(), x, y); }

  /* https://wiki.libsdl.org/SDL_GetWindowTitle?action=fullsearch&context=180&value=window&titlesearch=Titles */

  inline SDL_Point getSize() const { return size_; }
  inline SDL_Window* getWindow() const { return window_.get(); }
  inline std::string getTitle() const { return SDL_GetWindowTitle(window_.get()); }
  inline void setTitle(const std::string& title) { SDL_SetWindowTitle(window_.get(), title.c_str()); }

 private:

  template <typename T>
  using unique_ptr_deleter = std::unique_ptr<T, deleter<T>>;

  unique_ptr_deleter<SDL_Window> window_{nullptr};
  SDL_Point size_{};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_WINDOW_HPP_
