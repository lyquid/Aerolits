#ifndef KTP_SDL2_WRAPPERS_SDL2_FONT_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_FONT_HPP_

#include "sdl2_log.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>
#include <utility> // std::move

namespace ktp {

struct FontInfo {
  std::string face_family_name_ {};
  std::string face_style_name_ {};
  int hinting_ {};
  bool kerning_ {};
  int line_skip_ {};
  int max_ascent_ {};
  int max_descent_ {};
  int max_height_ {};
  bool monospaced_ {};
  int outline_ {};
  std::string path_ {};
  int size_ {};
  int style_ {};
};

class SDL2_Font {
 public:
  SDL2_Font() = default;
  SDL2_Font(const SDL2_Font&) = delete;
  SDL2_Font(SDL2_Font&& other) noexcept { *this = std::move(other); }
  ~SDL2_Font() { if (font_) TTF_CloseFont(font_); }

  SDL2_Font& operator=(const SDL2_Font&) = delete;
  SDL2_Font& operator=(SDL2_Font&& other) noexcept {
    if (this != &other) {
      if (font_) TTF_CloseFont(font_);
      font_ = std::exchange(other.font_, nullptr);
      font_info_ = std::move(other.font_info_);
    }
    return *this;
  }

  /**
   * Initializes SDL_ttf and queries for the versions used (linked and compiled).
   * @return True on success, or false on errors.
   */
  static bool initTTF() {
    if (!TTF_WasInit() && TTF_Init() != 0) {
      logSDL2Error("TTF_Init");
      return false;
    } else {
      queryTTFVersions();
      logInfo("SDL_ttf compiled version: " +
                  std::to_string(ttf_compiled_version_.major) + '.' +
                  std::to_string(ttf_compiled_version_.minor) + '.' +
                  std::to_string(ttf_compiled_version_.patch));
      logInfo("SDL_ttf linked version: " +
                  std::to_string(ttf_linked_version_->major) + '.' +
                  std::to_string(ttf_linked_version_->minor) + '.' +
                  std::to_string(ttf_linked_version_->patch));
    }
    return true;
  }

  /**
   * Shutdown and cleanup the truetype font API.
   */
  static void closeTTF() { if (TTF_WasInit()) TTF_Quit(); }

  /**
   * @brief Get the Font object.
   * @return A pointer to the font object.
   */
  inline auto font() const { return font_; }

 /**
  * Get the current font face family name from the loaded font. This function
  * may return a NULL pointer, in which case the information is not available.
  * @return The current family name of of the face of the font, or NULL perhaps.
  */
  inline const std::string& faceFamilyName() const { return font_info_.face_family_name_; }

  /**
   * Get the current font face style name from the loaded font. This function
   * may return a NULL pointer, in which case the information is not available.
   * @return The current style name of of the face of the font, or NULL perhaps.
   */
  inline const std::string& faceStyleName() const { return font_info_.face_style_name_; }

  /**
   * Get a reference to the FontInfo struct.
   * @return A reference to the FontInfo struct.
   */
  inline const FontInfo& fontInfo() const { return font_info_; }

  /**
   * Get the current hinting setting of the loaded font.
   * @return The hinting type matching one of the following defined values:
              0 TTF_HINTING_NORMAL,
              1 TTF_HINTING_LIGHT,
              2 TTF_HINTING_MONO,
              3 TTF_HINTING_NONE,
            If no hinting is set then TTF_HINTING_NORMAL is returned.
   */
  inline auto hinting() const { return font_info_.hinting_; }

  /**
   * Get the current kerning setting of the loaded font.
   * @return False if kerning is disabled. True is returned when enabled.
   *         The default for a newly loaded font is enabled(1).
   */
  inline auto kerning() const { return font_info_.kerning_; }

  /**
   * Get the recommended pixel height of a rendered line of text of the loaded
   * font. This is usually larger than the getMaxHeight() of the font.
   * @return The recommended spacing between lines of text for this font.
   */
  inline auto lineSkip() const { return font_info_.line_skip_; }

  /**
   * Get the maximum pixel ascent of all glyphs of the loaded font. This can
   * also be interpreted as the distance from the top of the font to the baseline.
   * It could be used when drawing an individual glyph relative to a top point,
   * by combining it with the glyph's maxy metric to resolve the top of the
   * rectangle used when blitting the glyph on the screen.
   *
   * @return The maximum pixel ascent of all glyphs in the font.
   */
  inline auto maxAscent() const { return font_info_.max_ascent_; }

  /**
   * Get the maximum pixel descent of all glyphs of the loaded font. This can
   * also be interpreted as the distance from the baseline to the bottom of the font.
   * It could be used when drawing an individual glyph relative to a bottom
   * point, by combining it with the glyph's maxy metric to resolve the top of
   * the rectangle used when blitting the glyph on the screen.
   *
   * @return The offset from the baseline to the bottom of the font.
   *         This is a negative value, relative to the baseline.
   */
  inline auto maxDescent() const { return font_info_.max_descent_; }

  /**
   * Get the maximum pixel height of all glyphs of the loaded font. You may use
   * this height for rendering text as close together vertically as possible,
   * though adding at least one pixel height to it will space it so they can't touch.
   * Remember that SDL_ttf doesn't handle multiline printing, so you are
   * responsible for line spacing, see the TTF_FontLineSkip as well.
   * @return The maximum pixel height of all glyphs in the font.
   */
  inline auto maxHeight() const { return font_info_.max_height_; }

   /**
   * Get the current outline size of the loaded font.
   * @return The size of the outline currently set on the font, in pixels.
   */
  inline auto outline() const { return font_info_.outline_; }

  /**
   * Get the rendering style of the loaded font.
   * @return The style as a bitmask composed of the following masks:
              TTF_STYLE_BOLD
              TTF_STYLE_ITALIC
              TTF_STYLE_UNDERLINE
              TTF_STYLE_STRIKETHROUGH
            If no style is set then TTF_STYLE_NORMAL is returned.
   */
  inline auto style() const { return font_info_.style_; }

  /**
   * Get the status of the availability of the glyph for ch from the loaded font.
   * @param ch The unicode character to test glyph availability of.
   * @return The index of the glyph for ch in font, or 0 for an undefined character code.
   */
  inline auto isGlyphProvided(Uint16 ch) const { return TTF_GlyphIsProvided(font_, ch); }

  /**
   * Test if the current font face of the loaded font is a fixed width font.
   * Fixed width fonts are monospace, meaning every character that exists in
   * the font is the same width, thus you can assume that a rendered string's
   * width is going to be the result of a simple calculation:
   *       glyph_width * string_length
   * @return True if font is a fixed width font. False if not a fixed width font.
   */
  inline auto isMonospaced() const { return font_info_.monospaced_; }

  /**
  * Load file for use as a font, at ptsize size. This is actually
  * TTF_OpenFontIndex(file, ptsize, 0). This can load TTF and FON files.
  * @param path Path to the font file to use.
  * @param size Point size (based on 72DPI) to load font as. This basically
  *             translates to pixel height.
  * @return True on success, or false on errors.
  */
  bool loadFont(const std::string& path, int size) {
    if (font_) TTF_CloseFont(font_);
    font_ = TTF_OpenFont(path.c_str(), size);
    if (font_) {
      queryFontInfo();
      font_info_.path_ = path;
      font_info_.size_ = size;
      return true;
    } else {
      logSDL2Error("TTF_OpenFont");
      return false;
    }
  }

  /**
   * Querys for the compiled version and the linked version of SDL_ttf libraries.
   */
  static void queryTTFVersions() {
    SDL_TTF_VERSION(&ttf_compiled_version_);
    ttf_linked_version_ = TTF_Linked_Version();
  }

  /**
   * Set the hinting of the loaded font. You should experiment with this
   * setting if you know which font you are using beforehand, especially when
   * using smaller sized fonts. If the user is selecting a font, you may wish
   * to let them select the hinting mode for that font as well.
   * @param hinting The hinting setting desired, which is one of:
                        TTF_HINTING_NORMAL
                        TTF_HINTING_LIGHT
                        TTF_HINTING_MONO
                        TTF_HINTING_NONE
            The default is TTF_HINTING_NORMAL.
   */
  void setHinting(int hinting) {
    if (font_info_.hinting_ != hinting) {
      TTF_SetFontHinting(font_, hinting);
      queryFontInfo();
    }
  }

  /**
   * Set whther to use kerning when rendering the loaded font. This has no
   * effect on individual glyphs, but rather when rendering whole strings of
   * characters, at least a word at a time. Perhaps the only time to disable
   * this is when kerning is not working for a specific font, resulting in
   * overlapping glyphs or abnormal spacing within words.
   * @param allowed
   */
  void setKerning(bool allowed) {
    if (font_info_.kerning_ != allowed) {
      allowed ? TTF_SetFontKerning(font_, 1) : TTF_SetFontKerning(font_, 0);
      queryFontInfo();
    }
  }

  /**
   * Set the outline pixel width of the loaded font.
   * @param pixels The size of outline desired, in pixels. Use 0(zero) to turn
   * off outlining.
   */
  void setOutline(int pixels) {
    if (font_info_.outline_ != pixels) {
      TTF_SetFontOutline(font_, pixels);
      queryFontInfo();
    }
  }

  /**
   * Set the rendering style of the loaded font.
   * @param style The style as a bitmask composed of the following masks:
                    TTF_STYLE_BOLD
                    TTF_STYLE_ITALIC
                    TTF_STYLE_UNDERLINE
                    TTF_STYLE_STRIKETHROUGH
            If no style is desired then use TTF_STYLE_NORMAL, which is the default.
   */
  void setStyle(int style) {
    if (font_info_.style_ != style) {
      TTF_SetFontStyle(font_, style);
      queryFontInfo();
    }
  }

 private:

  /**
   * Gathers useful information about the current font.
   */
  void queryFontInfo() {
    font_info_.face_family_name_ = TTF_FontFaceFamilyName(font_);
    font_info_.face_style_name_  = TTF_FontFaceStyleName(font_);
    font_info_.hinting_          = TTF_GetFontHinting(font_);
    font_info_.kerning_          = TTF_GetFontKerning(font_) != 0;
    font_info_.line_skip_        = TTF_FontLineSkip(font_);
    font_info_.max_ascent_       = TTF_FontAscent(font_);
    font_info_.max_descent_      = TTF_FontDescent(font_);
    font_info_.max_height_       = TTF_FontHeight(font_);
    font_info_.monospaced_       = TTF_FontFaceIsFixedWidth(font_) != 0;
    font_info_.outline_          = TTF_GetFontOutline(font_);
    font_info_.style_            = TTF_GetFontStyle(font_);
  }

  inline static SDL_version ttf_compiled_version_ {};
  inline static const SDL_version* ttf_linked_version_ {nullptr};

  TTF_Font* font_ {nullptr};
  FontInfo font_info_ {};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_FONT_HPP_
