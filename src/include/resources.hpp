#pragma once

#include "../sdl2_wrappers/sdl2_font.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <map>
#include <string>

namespace ktp { namespace Resources {

using FontsMap    = std::map<std::string, SDL2_Font>;
using ShadersMap  = std::map<std::string, GLuint>;
using TexturesMap = std::map<std::string, GLuint>;

extern FontsMap    fonts_map;
extern ShadersMap  shaders_map;
extern TexturesMap textures_map;

/**
 * @brief Deletes the OpenGL resources.
 */
void cleanOpenGL();

/**
 * @brief Gets the path to the config directory.
 * @param sub_dir Specify a sub directory.
 * @return The full path to the config directory (and subdirectory if instructed).
 */
std::string getConfigPath(const std::string& sub_dir = "");

/**
 * @brief Gets the path to the resources directory.
 * @param sub_dir Specify a sub directory, ie. textures.
 * @return The full path to the resources directory (and subdirectory if instructed).
 */
std::string getResourcesPath(const std::string& sub_dir = "");

/* FONTS */

/**
 * @brief Looks for a font. And gives it to you!
 * @param name The name of the font to look for.
 * @return A pointer to the TTF_Font. Don't delete it >:(
 */
inline auto getFont(const std::string name) { return fonts_map.at(name).font(); }

/**
 * @brief Loads a font from a file and adds it to the fonts map.
 * @param name The name you want to give to the font.
 * @param file The full path to the font file.
 * @param size The size of the font you desire.
 */
void loadFont(const std::string& name, const std::string& file, int size);

/* SHADERS */

/**
 * @brief Retrieves a shader by name.
 * @param name The name of the shader you want.
 * @return A ShaderProgram with the shader requested.
 */
inline auto getShader(const std::string& name) { return ShaderProgram{shaders_map.at(name)}; }

/**
 * @brief Loads and compiles a shader program.
 * @param name The name you wan to give to the shader program.
 * @param vertex_shader_path Vertex shader file path.
 * @param fragment_shader_path Fragment shader file path.
 * @param geometry_shader_path Geometry shader file path.
 */
bool loadShader(const std::string& name, const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path = "");

/**
 * @brief Prints any problems with the shader program. If any...
 * @param program The id of the shader program.
 */
bool printProgramLog(GLuint program);

/**
 * @brief Prints problems with the shader, if any.
 * @param shader The shader id.
 */
bool printShaderLog(GLuint shader);

/* TEXTURES */

/**
 * @brief Round up to next higher power of 2 (return x if it's already a power of 2).
 * @param x The number!
 * @return The next higher power of 2.
 */
inline int pow2RoundUp(int x) {
  if (x < 0) return 0;
  --x;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return x + 1;
}

/**
 * @brief Retrieves a texture by name.
 * @param name The name of the texture.
 * @return A Texture2D object with the requested texture.
 */
inline auto getTexture(const std::string& name) { return Texture2D{textures_map.at(name)}; }

/**
 * @brief Image file to opengl texture loader. Use with caution.
 * @param name The name you want for the texture. Don't forget it!
 * @param file The full path to the image file.
 * @param alpha Alpha channel?
 */
void loadTexture(const std::string& name, const std::string& file, bool alpha);

/**
 * @brief Tries to create textures with text. Blended in this case, which may be slow.
 * @param name The name of the texture for the textures map.
 * @param text The desired text to display.
 * @param font The font to use.
 * @param color The color desired.
 * @return Texture2D
 */
Texture2D loadTextureFromTextBlended(const std::string& name, const std::string& text, const std::string& font, SDL_Color color);

/**
 * @brief Tries to create textures with text. Solid in this case, which should be fast.
 * @param name The name of the texture for the textures map.
 * @param text The desired text to display.
 * @param font The font to use.
 * @param color The color desired.
 * @return Texture2D
 */
Texture2D loadTextureFromTextSolid(const std::string& name, const std::string& text, const std::string& font, SDL_Color color);

} } // namespace resources / ktp
