#ifndef SHADERS_H
#define SHADERS_H

#include "../global_funcs.h"
#include "../log.h"
#include "../memory/anvil_mem.h"

// Define the list of shaders using an X Macro (relative paths)
#define SHADER_PATHS                                                             \
  X(INIT_EGL_VERTEX, "egl/init/vertex_shader.glsl")                              \
  X(INIT_EGL_FRAG, "egl/init/fragment_shader.glsl")                              \
  X(RENDER_PWD_FIELD_EGL_VERTEX, "egl/render_password_field/vertex_shader.glsl") \
  X(RENDER_PWD_FIELD_EGL_FRAG, "egl/render_password_field/fragment_shader.glsl") \
  X(RENDER_TIME_FIELD_EGL_VERTEX, "egl/render_time_box/vertex_shader.glsl")      \
  X(RENDER_TIME_FIELD_EGL_FRAG, "egl/render_time_box/fragment_shader.glsl")      \
  X(TEXTURE_EGL_VERTEX, "egl/texture/vertex_shader.glsl")                        \
  X(TEXTURE_EGL_FRAG, "egl/texture/fragment_shader.glsl")

// Declare extern const char* for each shader path (for future use)
#define X(name, path) extern const char* SHADER_##name;
SHADER_PATHS
#undef X

// Define each shader path with the actual file path (inline initialization)
#define X(name, path) const char* SHADERS_##name = path;
SHADER_PATHS
#undef X

// Define base paths for global and local shader directories
#define GLOBAL_SHADER_DIR "/usr/share/anvilock/shaders/"
#define LOCAL_SHADER_DIR  "/.local/share/anvilock/shaders/"

char* find_shader_runtime(const char* home)
{
  char path_buffer[512];

  // Check global shader directory
  if (access(GLOBAL_SHADER_DIR, F_OK) == 0)
  {
    log_message(LOG_LEVEL_DEBUG, "Found Global shader runtime at: '%s'", GLOBAL_SHADER_DIR);
    return GLOBAL_SHADER_DIR;
  }

  snprintf(path_buffer, sizeof(path_buffer), "%s%s", home, LOCAL_SHADER_DIR);
  if (access(path_buffer, F_OK) == 0)
  {
    log_message(LOG_LEVEL_DEBUG, "Found Local shader runtime at: '%s'", path_buffer);
    return strdup(path_buffer);
  }

  log_message(LOG_LEVEL_DEBUG, "Shader runtime not found!");
  return NULL;
}

// Function to load a shader source file (absolute file path required)
static char* load_shader_source(const char* absfilepath)
{
  log_message(LOG_LEVEL_DEBUG, "Loading shader %s...", absfilepath);
  if (!absfilepath)
  {
    exit(EXIT_FAILURE);
  }

  FILE* file = fopen(absfilepath, "r");
  if (!file)
  {
    log_message(LOG_LEVEL_ERROR, "[SHADERS] Failed to open shader file: %s", absfilepath);
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  size_t length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* source;
  ANVIL_SAFE_ALLOC(source, char, length + 1);

  if (!source)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to allocate memory for shader source");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  fread(source, 1, length, file);
  source[length] = '\0';

  fclose(file);
  return source;
}

// Function to load all shaders (iterating through all SHADER_PATHS)
static inline void load_all_shaders(void)
{
#define X(name, path) load_shader_source(path);
  SHADER_PATHS
#undef X
}

#endif // SHADERS_H
