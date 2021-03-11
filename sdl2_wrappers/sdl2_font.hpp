#ifndef KTP_SDL2_WRAPPERS_SDL2_FONT_H_
#define KTP_SDL2_WRAPPERS_SDL2_FONT_H_

#include "sdl2_log.hpp"

#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>

namespace ktp {

template <typename T> struct deleter;

template <> struct deleter<TTF_Font> {
  void operator()(TTF_Font* font) { 
    TTF_CloseFont(font);
    font = nullptr;
  }
};

struct FontInfo {
  std::string face_family_name_ {};
  std::string face_style_name_ {};
  int hinting_ = 0;
  bool kerning_ = false;
  int line_skip_ = 0;
  int max_ascent_ = 0;
  int max_descent_ = 0;
  int max_height_ = 0;
  bool monospaced_ = false;
  int outline_ = 0;
  int style_ = 0;
};

class SDL2_Font {
 public:

  /**
   * Initializes SDL_ttf and queries for the versions used (linked and compiled).
   * @return True on success, or false on errors.
   */
  static bool initTTF();

  /**
   * Shutdown and cleanup the truetype font API.
   */
  static void closeTTF() { if (TTF_WasInit()) TTF_Quit(); }

  /**
   * Querys for the compiled version and the linked version of SDL_ttf libraries.
   */
  static void queryTTFVersions();
  inline static SDL_version ttf_compiled_version_;
  inline static const SDL_version* ttf_linked_version_;

  TTF_Font* getFont() const { return font_.get(); }

 /**
  * Get the current font face family name from the loaded font. This function 
  * may return a NULL pointer, in which case the information is not available.
  * @return The current family name of of the face of the font, or NULL perhaps.
  */
  const std::string& getFaceFamilyName() const { return font_info_.face_family_name_; }

  /**
   * Get the current font face style name from the loaded font. This function 
   * may return a NULL pointer, in which case the information is not available. 
   * @return The current style name of of the face of the font, or NULL perhaps.
   */
  const std::string& getFaceStyleName() const { return font_info_.face_style_name_; }

  /**
   * Get a reference to the FontInfo struct.
   * @return A reference to the FontInfo struct.
   */
  const FontInfo& getFontInfo() const { return font_info_; }

  /**
   * Get the current hinting setting of the loaded font.
   * @return The hinting type matching one of the following defined values:
              0 TTF_HINTING_NORMAL, 
              1 TTF_HINTING_LIGHT, 
              2 TTF_HINTING_MONO, 
              3 TTF_HINTING_NONE, 
            If no hinting is set then TTF_HINTING_NORMAL is returned. 
   */
  int getHinting() const { return font_info_.hinting_; }

  /**
   * Get the current kerning setting of the loaded font.
   * @return False if kerning is disabled. True is returned when enabled. 
   *         The default for a newly loaded font is enabled(1). 
   */
  bool getKerning() const { return font_info_.kerning_; }

  /**
   * Get the recommended pixel height of a rendered line of text of the loaded 
   * font. This is usually larger than the getMaxHeight() of the font.
   * @return The recommended spacing between lines of text for this font.
   */
  int getLineSkip() const { return font_info_.line_skip_; }

  /**
   * Get the maximum pixel ascent of all glyphs of the loaded font. This can 
   * also be interpreted as the distance from the top of the font to the baseline. 
   * It could be used when drawing an individual glyph relative to a top point, 
   * by combining it with the glyph's maxy metric to resolve the top of the 
   * rectangle used when blitting the glyph on the screen.
   * 
   * @return The maximum pixel ascent of all glyphs in the font. 
   */
  int getMaxAscent() const { return font_info_.max_ascent_; }

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
  int getMaxDescent() const { return font_info_.max_descent_; }

  /**
   * Get the maximum pixel height of all glyphs of the loaded font. You may use 
   * this height for rendering text as close together vertically as possible, 
   * though adding at least one pixel height to it will space it so they can't touch. 
   * Remember that SDL_ttf doesn't handle multiline printing, so you are 
   * responsible for line spacing, see the TTF_FontLineSkip as well.
   * @return The maximum pixel height of all glyphs in the font. 
   */
  int getMaxHeight() const { return font_info_.max_height_; }

   /**
   * Get the current outline size of the loaded font.
   * @return The size of the outline currently set on the font, in pixels. 
   */
  int getOutline() const { return font_info_.outline_; }

  /**
   * Get the rendering style of the loaded font.
   * @return The style as a bitmask composed of the following masks:
              TTF_STYLE_BOLD
              TTF_STYLE_ITALIC
              TTF_STYLE_UNDERLINE
              TTF_STYLE_STRIKETHROUGH
            If no style is set then TTF_STYLE_NORMAL is returned. 
   */
  int getStyle() const { return font_info_.style_; }

  /**
   * Get the status of the availability of the glyph for ch from the loaded font.
   * @param ch The unicode character to test glyph availability of.
   * @return The index of the glyph for ch in font, or 0 for an undefined character code. 
   */
  int isGlyphProvided(Uint16 ch) const { return TTF_GlyphIsProvided(font_.get(), ch); }

  /**
   * Test if the current font face of the loaded font is a fixed width font. 
   * Fixed width fonts are monospace, meaning every character that exists in 
   * the font is the same width, thus you can assume that a rendered string's 
   * width is going to be the result of a simple calculation: 
   *       glyph_width * string_length
   * @return True if font is a fixed width font. False if not a fixed width font. 
   */
  bool isMonospaced() const { return font_info_.monospaced_; }

  /**
  * Load file for use as a font, at ptsize size. This is actually 
  * TTF_OpenFontIndex(file, ptsize, 0). This can load TTF and FON files.
  * @param path Path to the font file to use.
  * @param size Point size (based on 72DPI) to load font as. This basically 
  *             translates to pixel height.
  * @return True on success, or false on errors.
  */
  bool loadFont(const std::string& path, int size);

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
  void setHinting(int hinting);

  /**
   * Set whther to use kerning when rendering the loaded font. This has no 
   * effect on individual glyphs, but rather when rendering whole strings of 
   * characters, at least a word at a time. Perhaps the only time to disable 
   * this is when kerning is not working for a specific font, resulting in 
   * overlapping glyphs or abnormal spacing within words.
   * @param allowed 
   */
  void setKerning(bool allowed);

  /**
   * Set the outline pixel width of the loaded font.
   * @param pixels The size of outline desired, in pixels. Use 0(zero) to turn 
   * off outlining.
   */
  void setOutline(int pixels);

  /**
   * Set the rendering style of the loaded font.
   * @param style The style as a bitmask composed of the following masks:
                    TTF_STYLE_BOLD
                    TTF_STYLE_ITALIC
                    TTF_STYLE_UNDERLINE
                    TTF_STYLE_STRIKETHROUGH
            If no style is desired then use TTF_STYLE_NORMAL, which is the default.
   */
  void setStyle(int style); 

 private:
 
  template <typename T>
  using unique_ptr_deleter = std::unique_ptr<T, deleter<T>>;

  /**
   * Gathers useful information about the current font. 
   */
  void queryFontInfo();

  unique_ptr_deleter<TTF_Font> font_{nullptr};
  FontInfo font_info_{};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_FONT_H_