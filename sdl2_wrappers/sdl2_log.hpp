#ifndef KTP_SDL2_WRAPPERS_SDL2_LOG_H_
#define KTP_SDL2_WRAPPERS_SDL2_LOG_H_

#include <SDL.h>
#include <iostream>
#include <string>

namespace ktp {

/* Original loggers idea from Will Usher */
/* Check it here: https://github.com/Twinklebear/TwinklebearDev-Lessons */

/**
* Log a message to the standard error stream.
* @param msg The message to write, format will be msg + '\n'.
*/
inline void logErrorMessage(std::string_view msg) {
  std::cerr << msg << '\n';
}

/**
* Log a message to the standard error stream.
* @param msg The message to write, format will be msg + '\n'.
* @param src Source of the resource.
*/
inline void logErrorMessage(std::string_view msg, std::string_view src){
	std::cerr << msg << " (" << src << ")\n";
}

/**
* Log a message to the standard output stream.
* @param msg The message to write, format will be msg + '\n'.
*/
inline void logMessage(std::string_view msg) {
  std::cout << msg << '\n';
}

/**
* Log a message to the output stream of our choice.
* @param os The output stream to write the message to.
* @param msg The message to write, format will be msg + '\n'.
*/
inline void logMessage(std::ostream& os, std::string_view msg) {
  os << msg << '\n';
}

/**
* Log an SDL error with some error message to the standard output stream.
* @param msg The error message to write, format will be msg error: SDL_GetError() + '\n'.
*/
inline void logSDL2Error(std::string_view msg){
	std::cerr << msg << " error: " << SDL_GetError() << '\n';
}

/**
* Log an SDL error with some error message to the output stream of our choice.
* @param os The output stream to write the message to.
* @param msg The error message to write, format will be msg error: SDL_GetError() + '\n'.
*/
inline void logSDL2Error(std::ostream& os, std::string_view msg){
	os << msg << " error: " << SDL_GetError() << '\n';
}

/**
* Log an SDL error with some error message to the standard output stream.
* @param msg The error message to write, format will be msg error: SDL_GetError() + '\n'.
* @param src Source of the resource.
*/
inline void logSDL2Error(std::string_view msg, std::string_view src){
	std::cerr << msg << " error: " << SDL_GetError() << " (" << src << ")\n";
}

/**
* Log an SDL error with some error message to the output stream of our choice.
* @param os The output stream to write the message to.
* @param msg The error message to write, format will be msg error: SDL_GetError() + '\n'.
* @param src Source of the resource.
*/
inline void logSDL2Error(std::ostream& os, std::string_view msg, std::string_view src){
	os << msg << " error: " << SDL_GetError() << " (" << src << ")\n";
}

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_LOG_H_