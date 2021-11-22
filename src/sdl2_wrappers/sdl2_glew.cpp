#include "sdl2_glew.hpp"
#include "sdl2_log.hpp"

bool ktp::SDL2_GLEW::init(SDL_Window* window) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  context_ = SDL_GL_CreateContext(window);
  if (context_ == NULL) {
    logSDL2Error("SDL_GL_CreateContext", SDL_LOG_CATEGORY_RENDER);
    // success = false;
    return false;
  } else {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError {glewInit()};
    if (glewError != GLEW_OK) {
      //logError(glewGetErrorString(glewError));
      //logMessage(glewGetErrorString(glewError));
      printf("Error initializing GLEW! %s", glewGetErrorString(glewError));
      return false;
    }

    // Use Vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
      logSDL2Error("SDL_GL_SetSwapInterval", SDL_LOG_CATEGORY_RENDER);
      // printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }

    // Initialize OpenGL
    if (!initGL()) {
      printf( "Unable to initialize OpenGL!\n" );
      // success = false;
      return false;
    }
  }
  return true;
}

bool ktp::SDL2_GLEW::initGL() {
  bool success {true};
  // Generate program
  gProgramID = glCreateProgram();

  // Create vertex shader
  GLuint vertexShader {glCreateShader(GL_VERTEX_SHADER)};

  // Get vertex source
  const GLchar* vertexShaderSource[] = {
    "#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
  };

  // Set vertex source
  glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

  // Compile vertex source
  glCompileShader(vertexShader);

  // Check vertex shader for errors
  GLint vShaderCompiled {GL_FALSE};
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
  if (vShaderCompiled != GL_TRUE) {
    printf("Unable to compile vertex shader %d!\n", vertexShader);
    // printShaderLog(vertexShader);
    success = false;
  } else {
    // Attach vertex shader to program
    glAttachShader(gProgramID, vertexShader);

    // Create fragment shader
    GLuint fragmentShader {glCreateShader(GL_FRAGMENT_SHADER)};

    // Get fragment source
    const GLchar* fragmentShaderSource[] = {
      "#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
    };

    // Set fragment source
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

    // Compile fragment source
    glCompileShader(fragmentShader);
    
    // Check fragment shader for errors
    GLint fShaderCompiled {GL_FALSE};
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
    if (fShaderCompiled != GL_TRUE) {
      printf( "Unable to compile fragment shader %d!\n", fragmentShader );
      // printShaderLog( fragmentShader );
      success = false;
    }
  }
  return success;
}
