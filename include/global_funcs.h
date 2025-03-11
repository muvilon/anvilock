#ifndef GLOBAL_FUNCS_H
#define GLOBAL_FUNCS_H

#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Has some functions and macros that are used globally
#define __ANVIL_FALLBACK_SCREEN_WIDTH__  1920
#define __ANVIL_FALLBACK_SCREEN_HEIGHT__ 1080

// Max/Min macros for different types
#define ANVIL_MAX(a, b) ((a) > (b) ? (a) : (b))
#define ANVIL_MIN(a, b) ((a) < (b) ? (a) : (b))

#define ANVIL_MAX_UINT(a, b) \
  ((unsigned int)(a) > (unsigned int)(b) ? (unsigned int)(a) : (unsigned int)(b))
#define ANVIL_MIN_UINT(a, b) \
  ((unsigned int)(a) < (unsigned int)(b) ? (unsigned int)(a) : (unsigned int)(b))

// Clamping function
#define ANVIL_CLAMP(value, min, max) (ANVIL_MAX((min), ANVIL_MIN((value), (max))))

// Swapping values
#define ANVIL_SWAP(a, b, type) \
  do                           \
  {                            \
    type temp = (a);           \
    (a)       = (b);           \
    (b)       = temp;          \
  } while (0)

// Get current time as a formatted string
void get_time_string(char* buffer, size_t size, const char* format)
{
  time_t     now   = time(NULL);
  struct tm* local = localtime(&now);
  if (!local)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to get local time.");
    return;
  }

  // Determine format based on input
  if (strcmp(format, "H:M:S") == 0)
  {
    strftime(buffer, size, "%H:%M:%S", local);
  }
  else if (strcmp(format, "H:M") == 0)
  {
    strftime(buffer, size, "%H:%M", local);
  }
  else
  {
    log_message(LOG_LEVEL_WARN, "Invalid time format. Expected 'H:M:S' or 'H:M'. Falling back...");
    strftime(buffer, size, "%H:%M:%S", local);
    return;
  }

  log_message(LOG_LEVEL_DEBUG, "Formatted time string updated.");
}

// OpenGL Shader Error Checking
void check_shader_compile_status(GLuint shader)
{
  GLint  success;
  GLchar infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
    log_message(LOG_LEVEL_ERROR, "Shader Compilation Error: %s", infoLog);
  }
}

void check_program_link_status(GLuint program)
{
  GLint  success;
  GLchar infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
    log_message(LOG_LEVEL_ERROR, "Shader Program Linking Error: %s", infoLog);
  }
}

static const GLfloat vertices_with_texcoords[] = {
  // Position      // Texture coords
  -0.3f, 0.9f, 0.0f, 0.0f, // Top left
  0.3f,  0.9f, 1.0f, 0.0f, // Top right
  -0.3f, 0.7f, 0.0f, 1.0f, // Bottom left
  0.3f,  0.7f, 1.0f, 1.0f  // Bottom right
};

// OpenGL Vertex Data
static const GLfloat quad_vertices[] = {
  -1.0f, 1.0f,  // Top left
  -1.0f, -1.0f, // Bottom left
  1.0f,  1.0f,  // Top right
  1.0f,  -1.0f  // Bottom right
};

static const GLfloat triangle_vertices[] = {
  0.0f,  0.5f,  // Top vertex
  -0.5f, -0.5f, // Bottom left vertex
  0.5f,  -0.5f  // Bottom right vertex
};

static const GLfloat tex_coords[] = {
  0.0f, 0.0f, // Top left
  0.0f, 1.0f, // Bottom left
  1.0f, 0.0f, // Top right
  1.0f, 1.0f  // Bottom right
};

// New vertex array for the password field
static const GLfloat password_field_vertices[] = {
  -0.4f, 0.1f,  // Top left
  -0.4f, -0.1f, // Bottom left
  0.4f,  0.1f,  // Top right
  0.4f,  -0.1f  // Bottom right
};

// Define vertices for a single dot (small square)
static const GLfloat dot_vertices[] = {
  -0.015f, 0.015f,  // Top left
  -0.015f, -0.015f, // Bottom left
  0.015f,  0.015f,  // Top right
  0.015f,  -0.015f  // Bottom right
};

/*
 * Power of Two in GPU:
 *
 * This is an issue I found when trying to render a time box.
 *
 * GPUs and texture sampling mechanisms require textures to have dimensions
 * that are powers of two (e.g., 32x32, 64x64 so on) to function correctly.
 *
 * If this is not obeyed then noticeable glitches and unexpected stuff occurs,
 * that will probably make you want to harm yourself.
 *
 * So yea this function is pretty important do note that.
 *
 */
int next_power_of_two(int v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

#endif // GLOBAL_FUNCS_H
