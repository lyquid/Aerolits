#ifndef KTP_SDL2_WRAPPERS_SDL2_LOG_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_LOG_HPP_

#include <SDL.h>
#include <string>

namespace ktp {

/* Original loggers idea from Will Usher */
/* Check it here: https://github.com/Twinklebear/TwinklebearDev-Lessons */

/**
 * @brief Log a message with SDL_LOG_PRIORITY_CRITICAL.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logCritical(const char* msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogCritical(category, msg);
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_CRITICAL.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logCritical(const std::string& msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogCritical(category, msg.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_DEBUG.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logDebug(const char* msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogDebug(category, msg);
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_DEBUG.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logDebug(const std::string& msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogDebug(category, msg.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_ERROR.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_ERROR.
 */
inline void logError(const char* msg, int category = SDL_LOG_CATEGORY_ERROR) {
  SDL_LogError(category, msg);
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_ERROR.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_ERROR.
 */
inline void logError(const std::string& msg, int category = SDL_LOG_CATEGORY_ERROR) {
  SDL_LogError(category, msg.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_ERROR.
 *
 * @param msg The message to write.
 * @param src The source of the file.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_ERROR.
 */
inline void logError(const char* msg, const char* src, int category = SDL_LOG_CATEGORY_ERROR){
  const auto fs {std::string{msg} + " (" + src + ")"};
  SDL_LogError(category, fs.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_ERROR.
 *
 * @param msg The message to write.
 * @param src The source of the file.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_ERROR.
 */
inline void logError(const std::string& msg, std::string_view src, int category = SDL_LOG_CATEGORY_ERROR){
  const auto fs {msg.data() + std::string{" ("} + src.data() + ')'};
  SDL_LogError(category, fs.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_INFO.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logInfo(const char* msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogInfo(category, msg);
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_INFO.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logInfo(const std::string& msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogInfo(category, msg.data());
}

/**
 * @brief Log a message with the specified category and priority.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 * @param priority The priority of the message. Defaults to SDL_LOG_PRIORITY_INFO.
 */
inline void logMessage(const char* msg, int category = SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority priority = SDL_LOG_PRIORITY_INFO) {
  SDL_LogMessage(category, priority, msg);
}

/**
 * @brief Log a message with the specified category and priority.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 * @param priority The priority of the message. Defaults to SDL_LOG_PRIORITY_INFO.
 */
inline void logMessage(const std::string& msg, int category = SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority priority = SDL_LOG_PRIORITY_INFO) {
  SDL_LogMessage(category, priority, msg.data());
}

/**
 * @brief Log a message with the specified category and priority.
 *
 * @param msg The message to write.
 * @param ap A variable argument list.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 * @param priority The priority of the message. Defaults to SDL_LOG_PRIORITY_INFO.
 */
inline void logMessage(const char* msg, va_list ap, int category = SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority priority = SDL_LOG_PRIORITY_INFO) {
  SDL_LogMessageV(category, priority, msg, ap);
}

/**
 * @brief Log a message with the specified category and priority.
 *
 * @param msg The message to write.
 * @param ap A variable argument list.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 * @param priority The priority of the message. Defaults to SDL_LOG_PRIORITY_INFO.
 */
inline void logMessage(const std::string& msg, va_list ap, int category = SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority priority = SDL_LOG_PRIORITY_INFO) {
  SDL_LogMessageV(category, priority, msg.data(), ap);
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_VERBOSE.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logVerbose(const char* msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogVerbose(category, msg);
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_VERBOSE.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logVerbose(const std::string& msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogVerbose(category, msg.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_WARN.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logWarn(const char* msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogWarn(category, msg);
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_WARN.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_APPLICATION.
 */
inline void logWarn(const std::string& msg, int category = SDL_LOG_CATEGORY_APPLICATION) {
  SDL_LogWarn(category, msg.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_ERROR with the SDL_GetError() result.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_ERROR.
 */
inline void logSDL2Error(const char* msg, int category = SDL_LOG_CATEGORY_ERROR){
  const auto fs {msg + ' ' + std::string{SDL_GetError()}};
  SDL_LogError(category, fs.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_ERROR with the SDL_GetError() result.
 *
 * @param msg The message to write.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_ERROR.
 */
inline void logSDL2Error(const std::string& msg, int category = SDL_LOG_CATEGORY_ERROR){
  const auto fs {msg.data() + ' ' + std::string{SDL_GetError()}};
  SDL_LogError(category, fs.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_ERROR with the SDL_GetError() result.
 *
 * @param msg The message to write.
 * @param src The source of the file.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_ERROR.
 */
inline void logSDL2Error(const char* msg, const char* src, int category = SDL_LOG_CATEGORY_ERROR){
  const auto fs {msg + ' ' + std::string{SDL_GetError()} + " (" + src + ')'};
  SDL_LogError(category, fs.data());
}

/**
 * @brief Log a message with SDL_LOG_PRIORITY_ERROR with the SDL_GetError() result.
 *
 * @param msg The message to write.
 * @param src The source of the file.
 * @param category The category of the message. Defaults to SDL_LOG_CATEGORY_ERROR.
 */
inline void logSDL2Error(const std::string& msg, std::string_view src, int category = SDL_LOG_CATEGORY_ERROR){
  const auto fs {msg.data() + ' ' + std::string{SDL_GetError()} + " (" + src.data() + ')'};
  SDL_LogError(category, fs.data());
}

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_LOG_HPP_