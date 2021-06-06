#ifndef KTP_SDL2_WRAPPERS_SDL2_TEXTURE_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_TEXTURE_HPP_

#include "sdl2_font.hpp"
#include "sdl2_renderer.hpp"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>

namespace ktp {

template <typename T> struct deleter;

template <> struct deleter<SDL_Texture> {
  void operator()(SDL_Texture* tex) { 
    SDL_DestroyTexture(tex);
    tex = nullptr;
  }
};

class SDL2_Texture {
 public:
  
  bool create(Uint32 format, int access, const SDL_Point& size);

  inline auto getAccess() const { return access_; }
  
  inline auto getFormat() const { return format_; }

  inline auto getHeight() const { return height_; }

  inline auto getWidth() const { return width_; }

  inline SDL_Point getSize() const { return {width_, height_}; }

  inline SDL_Texture* getTexture() const { return texture_.get(); }

  /**
  * Load an image file to an SDL_Surface and then calls createTextureFromSurface() 
  * to create a texture from it.
  * @param path The path to the image file.
  * @return True on success, or false on errors.
  */
  bool loadFromFile(const std::string& path);

  /* 
  Slow Slow Slow, but Ultra Nice over another image
  Create a 32-bit ARGB surface and render the given text at high quality, using alpha blending to dither 
  the font with the given color. This results in a surface with alpha transparency, so you don't have a 
  solid colored box around the text. The text is antialiased. This will render slower than Solid, but in 
  about the same time as Shaded mode. The resulting surface will blit slower than if you had used Solid or Shaded. 
  Use this when you want high quality, and the text isn't changing too fast. */
  bool loadFromTextBlended(const SDL2_Font& font, const std::string& text, const SDL_Color& color);

  /* 
  Slow and Nice, but with a Solid Box
  Create an 8-bit palettized surface and render the given text at high quality with the given font and colors. 
  The 0 pixel value is background, while other pixels have varying degrees of the foreground color from the 
  background color. This results in a box of the background color around the text in the foreground color. 
  The text is antialiased. This will render slower than Solid, but in about the same time as Blended mode. 
  The resulting surface should blit as fast as Solid, once it is made. Use this when you need nice text, and can live with a box. */
  bool loadFromTextShaded(const SDL2_Font& font, const std::string& text, const SDL_Color& fg_color, const SDL_Color& bg_color);

  /* 
  Quick and Dirty
  Create an 8-bit palettized surface and render the given text at fast quality with the given font and color. 
  The pixel value of 0 is the colorkey, giving a transparent background when blitted. Pixel and colormap 
  value 1 is set to the text foreground color. This allows you to change the color without having to render 
  the text again. Palette index 0 is of course not drawn when blitted to another surface, since it is the colorkey, 
  and thus transparent, though its actual color is 255 minus each of the RGB components of the foreground color. 
  This is the fastest rendering speed of all the rendering modes. This results in no box around the text, but the 
  text is not as smooth. The resulting surface should blit faster than the Blended one. Use this mode for FPS and 
  other fast changing updating text displays. */
  bool loadFromTextSolid(const SDL2_Font& font, const std::string& text, const SDL_Color& color);

  /**
   * Lock a texture for write-only pixel access.
   * @param pixels This is filled in with a pointer to the locked pixels, 
   *                appropriately offset by the locked area.
   * @param pitch This is filled in with the pitch of the locked pixels; 
   *              the pitch is the length of one row in bytes
   */
  inline void lock(void** pixels, int* pitch) {
    SDL_LockTexture(texture_.get(), NULL, pixels, pitch);
  }

  /**
   * Lock a portion of the texture for write-only pixel access.
   * @param rect An SDL_Rect structure representing the area to lock for access; 
   *              NULL to lock the entire texture.
   * @param pixels This is filled in with a pointer to the locked pixels, 
   *                appropriately offset by the locked area.
   * @param pitch This is filled in with the pitch of the locked pixels; 
   *              the pitch is the length of one row in bytes
   */
  inline void lock(const SDL_Rect* rect, void** pixels, int* pitch) {
    SDL_LockTexture(texture_.get(), rect, pixels, pitch);
  }

  /**
   * Query the attributes of a texture.
   * @param texture The texture to query.
   * @param format A pointer filled in with the raw format of the texture; the 
   * actual format may differ, but pixel transfers will use this format (one of 
   * the SDL_PixelFormatEnum values).
   * @param access A pointer filled in with the actual access to the texture 
   * (one of the SDL_TextureAccess values).
   * @param width A pointer filled in with the width of the texture in pixels.
   * @param height A pointer filled in with the height of the texture in pixels.
   * @return True on success, or false on errors.
   */
  static bool queryTexture(SDL2_Texture texture, Uint32* format, int* access, int* width, int* height);  

  /**
  * Renders the whole texture to the renderer member.
  * @return True on success, or false on errors.
  */
  bool render();

  /**
  * Renders the whole texture to the renderer member.
  * @param where The coordinates to render the texture at.
  * @return True on success, or false on errors.
  */
  bool render(const SDL_Point& where);

  /**
  * Renders the whole texture to the renderer member.
  * @param where The coordinates to render the texture at.
  * @return True on success, or false on errors.
  */
  bool render(const SDL_FPoint& where);

  /**
  * Renders the whole texture to the renderer member.
  * @param dest_rect The destination rectangle.
  * @return True on success, or false on errors.
  */
  bool render(const SDL_Rect& dest_rect);

  /**
   * Use this function to copy a portion of the texture to the current rendering target.
   * @param src_rect The source rectangle of the texture atlas.
   * @param dest_rect The destination rectangle.
   * @return True on success, or false on errors. 
   */
  bool render(const SDL_Rect& src_rect, const SDL_Rect& dest_rect);

  /**
   * Use this function to copy a portion of the texture to the current rendering target, 
   * optionally rotating it by angle.
   * @param src_rect The source rectangle of the texture atlas.
   * @param dest_rect The destination rectangle.
   * @param angle The angle in degrees. Clockwise.
   * @return True on success, or false on errors. 
   */
  bool render(const SDL_Rect& src_rect, const SDL_Rect& dest_rect, float angle);

  /**
  * Renders the whole texture to the renderer member.
  * @param where The coordinates to render the texture at.
  * @param ren The renderer to use.
  * @return True on success, or false on errors.
  */
  bool render(const SDL_FPoint& where, const SDL2_Renderer& ren);

  /**
   * Use this function to set an additional alpha value multiplied into render copy operations.
   * @param alpha The source alpha value multiplied into copy operations.
   * @return Returns 0 on success or a negative error code on failure. 
   */
  inline int setAlphaMod(Uint8 alpha) { return SDL_SetTextureAlphaMod(texture_.get(), alpha); }

  /**
   * Use this function to set the blend mode for a texture, used by SDL_RenderCopy().
   * @param mode The SDL_BlendMode to use for texture blending.
   * @return Returns 0 on success or a negative error code on failure. 
   */
  inline int setBlendMode(const SDL_BlendMode& mode) const { return SDL_SetTextureBlendMode(texture_.get(), mode); }

  /**
   * Use this function to set an additional color value multiplied into render copy operations.
   * @param r The red color value multiplied into copy operations.
   * @param g The green color value multiplied into copy operations.
   * @param b The blue color value multiplied into copy operations.
   * @return Returns 0 on success or a negative error code on failure. 
   */
  inline int setColorMod(Uint8 r, Uint8 g, Uint8 b) const { return SDL_SetTextureColorMod(texture_.get(), r, g, b); }

  /**
   * Use this function to set an additional color value multiplied into render copy operations.
   * @param color The SDL_Color multiplied into copy operations.
   * @return Returns 0 on success or a negative error code on failure. 
   */
  inline int setColorMod(const SDL_Color& color) const { return SDL_SetTextureColorMod(texture_.get(), color.r, color.g, color.b); }

  /**
  * Sets a pointer to the renderer where the texture will be rendered in. 
  * Don't try to render the texture without having called this function.
  * @param ren The address of the renderer in which the texture shall be rendered in.
  */
  inline void setRenderer(const SDL2_Renderer& ren) { renderer_ = &ren; }

  /**
   * Updates the given texture rectangle with new pixel data.
   * @param pixels The raw pixel data in the format of the texture.
   * @param pitch The number of bytes in a row of pixel data, including padding between lines
   */
  inline void update(const void* pixels, int pitch) {
    SDL_UpdateTexture(texture_.get(), NULL, pixels, pitch);
  }

  /**
   * Unlocks a texture, uploading the changes to video memory, if needed.
   */
  inline void unlock() {
    SDL_UnlockTexture(texture_.get());
  }

  /**
   * Updates the given texture rectangle with new pixel data.
   * @param rect An SDL_Rect structure representing the area to update, or NULL 
   *              to update the entire texture.
   * @param pixels The raw pixel data in the format of the texture.
   * @param pitch The number of bytes in a row of pixel data, including padding between lines
   */
  inline void update(const SDL_Rect* rect, const void* pixels, int pitch) {
    SDL_UpdateTexture(texture_.get(), rect, pixels, pitch);
  }

 private:

  /**
  * Creates a texture from the specified surface.
  * @param surface The SDL_Surface structure containing pixel data used to fill 
  *                the texture.
  */
  void createTextureFromSurface(SDL_Surface& surface);

  template <typename T>
  using unique_ptr_deleter = std::unique_ptr<T, deleter<T>>;

  unique_ptr_deleter<SDL_Texture> texture_ {nullptr};
  const SDL2_Renderer* renderer_ {nullptr};
  
  int access_ {};
  Uint32 format_ {};
  int height_ {};
  int width_ {};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_TEXTURE_HPP_