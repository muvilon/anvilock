#ifndef SHADERS_H
#define SHADERS_H

#include <stdio.h>

// Define the list of shaders using an X Macro
#define SHADER_PATHS                                                                     \
  X(INIT_EGL_VERTEX, "shaders/egl/init/vertex_shader.glsl")                              \
  X(INIT_EGL_FRAG, "shaders/egl/init/fragment_shader.glsl")                              \
  X(RENDER_PWD_FIELD_EGL_VERTEX, "shaders/egl/render_password_field/vertex_shader.glsl") \
  X(RENDER_PWD_FIELD_EGL_FRAG, "shaders/egl/render_password_field/fragment_shader.glsl") \
  X(TEXTURE_EGL_VERTEX, "shaders/egl/texture/vertex_shader.glsl")                        \
  X(TEXTURE_EGL_FRAG, "shaders/egl/texture/fragment_shader.glsl")

// Declare extern const char* for each shader path (for future use)
#define X(name, path) extern const char* SHADER_##name;
SHADER_PATHS
#undef X

// Define each shader path with the actual file path (inline initialization)
#define X(name, path) const char* SHADERS_##name = path;
SHADER_PATHS
#undef X

static char* load_shader_source(const char* filepath)
{
  FILE* file = fopen(filepath, "r");
  if (!file)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to open shader file: %s", filepath);
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  size_t length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* source = malloc(length + 1);
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
static inline void load_all_shaders()
{
#define X(name, path) load_shader_source(SHADER_##name);
  SHADER_PATHS
#undef X
}

#endif // SHADERS_H
