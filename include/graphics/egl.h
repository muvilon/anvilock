#ifndef EGL_H
#define EGL_H

#include "../client_state.h"
#include "../config/config.h"
#include "../freetype/freetype.h"
#include "../global_funcs.h"
#include "../graphics/shaders.h"
#include "../log.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

static GLuint create_texture_shader_program(void)
{
  const char* vertex_shader_source = load_shader_source(SHADERS_TEXTURE_EGL_VERTEX);

  const char* fragment_shader_source = load_shader_source(SHADERS_TEXTURE_EGL_FRAG);

  // Create and compile vertex shader
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  // Create and compile fragment shader
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);

  // Create and link shader program
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  // Clean up shaders
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return program;
}

static void render_password_field(struct client_state* state);

GLuint create_text_texture(const char* text)
{
  if (!text || !*text)
  {
    log_message(LOG_LEVEL_ERROR, "Invalid text string for texture.");
    return 0;
  }

  FT_GlyphSlot slot = ft_face->glyph;

  // First pass: measure dimensions
  int width         = 0;
  int max_height    = 0;
  int max_bearing_y = 0;

  for (const char* p = text; *p; p++)
  {
    if (FT_Load_Char(ft_face, *p, FT_LOAD_RENDER))
    {
      log_message(LOG_LEVEL_ERROR, "Failed to load glyph.");
      continue;
    }

    // Add proper spacing between characters
    width += slot->bitmap.width + 1; // +1 for spacing
    max_height    = ANVIL_MAX_UINT(max_height, slot->bitmap.rows);
    max_bearing_y = ANVIL_MAX_UINT(max_bearing_y, slot->bitmap_top);
  }

  int height = ANVIL_MAX_UINT(CHAR_HEIGHT, max_height);

  // Convert width and height into powers of two (check global_funcs.h for more docs)
  width  = next_power_of_two(width);
  height = next_power_of_two(height);

  // Allocate image buffer
  /*unsigned char* image = calloc(width * height, sizeof(unsigned char));*/
  unsigned char* image;
  ANVIL_SAFE_CALLOC(image, unsigned char, width* height);

  if (!image)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to allocate image buffer.");
    return 0;
  }

  // Second pass: render glyphs
  int x_offset = 0;
  for (const char* p = text; *p; p++)
  {
    if (FT_Load_Char(ft_face, *p, FT_LOAD_RENDER))
      continue;

    // Calculate vertical position for baseline alignment
    int y_offset = (max_bearing_y - slot->bitmap_top) + (height - max_height) / 2;

    for (unsigned int row = 0; row < slot->bitmap.rows; row++)
    {
      for (unsigned int col = 0; col < slot->bitmap.width; col++)
      {
        int dst_row = row + y_offset;
        if (dst_row >= 0 && dst_row < height)
        {
          image[(dst_row * width) + x_offset + col] =
            slot->bitmap.buffer[row * slot->bitmap.width + col];
        }
      }
    }

    x_offset += slot->bitmap.width + 1; // +1 for spacing
  }

  // Create OpenGL texture
  GLuint texture;
  glGenTextures(1, &texture);
  if (texture == 0)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to generate OpenGL texture.");
    ANVIL_SAFE_FREE(image);
    return 0;
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  ANVIL_SAFE_FREE(image);
  log_message(LOG_LEVEL_DEBUG, "Text texture successfully created.");
  return texture;
}

void update_time_texture(struct client_state* state)
{
  char time_str[16];
  get_time_string(time_str, sizeof(time_str), global_config.time_format);

  static char last_time_str[16] = "";
  if (strcmp(last_time_str, time_str) != 0)
  {
    strcpy(last_time_str, time_str);

    if (state->time_texture)
    {
      glDeleteTextures(1, &state->time_texture);
      state->time_texture = 0;
      log_message(LOG_LEVEL_DEBUG, "Old time texture deleted.");
    }
    state->time_texture = create_text_texture(time_str);
  }
}

void render_time_box(struct client_state* state)
{
  if (!state->time_texture)
  {
    log_message(LOG_LEVEL_ERROR, "No valid texture for rendering.");
    return;
  }

  static GLuint vbo = 0;
  if (vbo == 0)
  {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(state->global_config.time_box_vertices),
                 state->global_config.time_box_vertices, GL_STATIC_DRAW);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        (void*)(2 * sizeof(GLfloat)));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, state->time_texture);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    log_message(LOG_LEVEL_ERROR, "OpenGL error: 0x%x", error);
  }

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_BLEND);

  log_message(LOG_LEVEL_DEBUG, "Time box rendered successfully.");
}

static GLuint load_texture(const char* filepath)
{
  int            width, height, channels;
  unsigned char* image = stbi_load(filepath, &width, &height, &channels, STBI_rgb_alpha);
  if (!image)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to load image: %s", filepath);
    exit(EXIT_FAILURE);
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(image);
  return texture;
}

static void init_egl(struct client_state* state)
{
  // Get the EGL display connection using Wayland's display
  state->egl_display = eglGetDisplay((EGLNativeDisplayType)state->wl_display);
  if (state->egl_display == EGL_NO_DISPLAY)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to get EGL display\n");
    exit(EXIT_FAILURE);
  }

  // Initialize the EGL display
  if (!eglInitialize(state->egl_display, NULL, NULL))
  {
    log_message(LOG_LEVEL_ERROR, "Failed to initialize EGL");
    exit(EXIT_FAILURE);
  }

  // Bind the OpenGL ES API
  if (!eglBindAPI(EGL_OPENGL_ES_API))
  {
    log_message(LOG_LEVEL_ERROR, "Failed to bind OpenGL ES API");
    exit(EXIT_FAILURE);
  }

  // EGL configuration: specifies rendering type and color depth
  EGLint    attribs[] = {EGL_RENDERABLE_TYPE,
                         EGL_OPENGL_ES2_BIT,
                         EGL_SURFACE_TYPE,
                         EGL_WINDOW_BIT,
                         EGL_RED_SIZE,
                         8,
                         EGL_GREEN_SIZE,
                         8,
                         EGL_BLUE_SIZE,
                         8,
                         EGL_NONE};
  EGLConfig config;
  EGLint    num_configs;
  if (!eglChooseConfig(state->egl_display, attribs, &config, 1, &num_configs) || num_configs < 1)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to choose EGL config");
    exit(EXIT_FAILURE);
  }

  // Create an EGL context for OpenGL ES 2.0
  EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
  state->egl_context =
    eglCreateContext(state->egl_display, config, EGL_NO_CONTEXT, context_attribs);
  if (state->egl_context == EGL_NO_CONTEXT)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create EGL context");
    exit(EXIT_FAILURE);
  }

  // Ensure Wayland surface events are processed before creating EGL window
  wl_display_roundtrip(state->wl_display);

  // Validate output dimensions and create EGL window surface
  int width =
    state->output_state.width > 0 ? state->output_state.width : __ANVIL_FALLBACK_SCREEN_WIDTH__;
  int height =
    state->output_state.height > 0 ? state->output_state.height : __ANVIL_FALLBACK_SCREEN_HEIGHT__;
  state->egl_window = wl_egl_window_create(state->wl_surface, width, height);
  if (!state->egl_window)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to create wl_egl_window");
    exit(EXIT_FAILURE);
  }

  state->egl_surface = eglCreateWindowSurface(state->egl_display, config,
                                              (EGLNativeWindowType)state->egl_window, NULL);
  if (state->egl_surface == EGL_NO_SURFACE)
  {
    EGLint error = eglGetError();
    log_message(LOG_LEVEL_ERROR, "Failed to create EGL surface, error code: %x", error);
    exit(EXIT_FAILURE);
  }

  // Make the EGL context current
  if (!eglMakeCurrent(state->egl_display, state->egl_surface, state->egl_surface,
                      state->egl_context))
  {
    log_message(LOG_LEVEL_ERROR, "Failed to make EGL context current");
    exit(EXIT_FAILURE);
  }

  // Set the OpenGL viewport to match the window size
  glViewport(0, 0, width, height);

  // Load the image and create a texture
  GLuint texture = load_texture(state->global_config.bg_path);

  // Use the texture for rendering
  glBindTexture(GL_TEXTURE_2D, texture);

  // Clear color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  // Render the quad with the texture
  const char* vertex_shader_source = load_shader_source(SHADERS_INIT_EGL_VERTEX);
  GLuint      vertex_shader        = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  // Check for vertex shader compile errors
  GLint compile_status;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status == GL_FALSE)
  {
    log_message(LOG_LEVEL_ERROR, "Vertex shader compilation failed");
    exit(EXIT_FAILURE);
  }

  const char* fragment_shader_source = load_shader_source(SHADERS_INIT_EGL_FRAG);
  GLuint      fragment_shader        = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);

  // Check for fragment shader compile errors
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status == GL_FALSE)
  {
    log_message(LOG_LEVEL_ERROR, "Fragment shader compilation failed");
    exit(EXIT_FAILURE);
  }

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  GLint link_status;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &link_status);
  if (link_status == GL_FALSE)
  {
    log_message(LOG_LEVEL_ERROR, "Shader program linking failed");
    exit(EXIT_FAILURE);
  }

  glUseProgram(shader_program);

  GLint position_location = glGetAttribLocation(shader_program, "position");
  GLint texCoord_location = glGetAttribLocation(shader_program, "texCoord");
  glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, quad_vertices);
  glEnableVertexAttribArray(position_location);
  glVertexAttribPointer(texCoord_location, 2, GL_FLOAT, GL_FALSE, 0, tex_coords);
  glEnableVertexAttribArray(texCoord_location);

  glUniform1i(glGetUniformLocation(shader_program, "uTexture"), 0);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  update_time_texture(state);
  render_time_box(state);
  render_password_field(state);

  eglSwapBuffers(state->egl_display, state->egl_surface);
}

static void render_password_field(struct client_state* state)
{
  const char* vertex_shader_source = load_shader_source(SHADERS_RENDER_PWD_FIELD_EGL_VERTEX);

  const char* fragment_shader_source = load_shader_source(SHADERS_RENDER_PWD_FIELD_EGL_FRAG);

  // Create and compile shaders
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);

  // Create and link program
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  // Enable blending for transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Use the shader program
  glUseProgram(program);

  // Get uniform locations
  GLint color_location    = glGetUniformLocation(program, "color");
  GLint offset_location   = glGetUniformLocation(program, "offset");
  GLint position_location = glGetAttribLocation(program, "position");

  // Width and height of the password field
  float field_width  = 0.7f;  // Adjusted width for the field
  float field_height = 0.15f; // Adjusted height for the field

  // Position offset to center at the bottom of the screen
  float offset_x = 0;                           // Horizontally center the field
  float offset_y = -0.8f + field_height / 2.0f; // Vertically align it at the bottom

  // Set up the password field background (using GL_TRIANGLE_STRIP for a rectangle)
  glUniform4f(color_location, 1.0f, 1.0f, 1.0f, 0.70f); // Light background with transparency
  glUniform2f(offset_location, offset_x, offset_y);

  glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, password_field_vertices);
  glEnableVertexAttribArray(position_location);

  // Draw the background of the password field
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Draw the border with a subtle shadow effect
  glUniform4f(color_location, 0.8f, 0.8f, 0.8f, 1.0f);
  glDrawArrays(GL_LINE_LOOP, 0, 4);

  // Draw password dots
  glUniform4f(color_location, 0.3f, 0.3f, 0.3f, 0.8f); // Gray dots

  // Set up vertices for dots
  glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, dot_vertices);

  // Adjust dot positions based on password input
  float dot_spacing = field_width / (state->pam.password_index + 1);
  for (int i = 0; i < state->pam.password_index; i++)
  {
    float x_position = offset_x + (i + 1) * dot_spacing - field_width / 2; // Center the dots
    glUniform2f(offset_location, x_position, offset_y);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  // Handle Authentication Failure (Red border for failure)
  if (state->pam.auth_state.auth_failed)
  {
    float failColor[] = {1.0f, 0.0f, 0.0f, 1.0f}; // Red for failure

    glUniform4fv(color_location, 1, failColor);
    glUniform2f(offset_location, offset_x, offset_y);
    glDrawArrays(GL_LINE_LOOP, 0, 4); // Re-draw border with failure color
  }

  // Handle Authentication Success (Green border for success)
  if (!state->pam.auth_state.auth_failed && state->pam.password_index > 0)
  {
    float successColor[] = {0.0f, 1.0f, 0.0f, 1.0f}; // Green for success

    glUniform4fv(color_location, 1, successColor);
    glUniform2f(offset_location, offset_x, offset_y);
    glDrawArrays(GL_LINE_LOOP, 0, 4); // Re-draw border with success color
  }

  // Disable blending and clean up
  glDisable(GL_BLEND);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  glDeleteProgram(program);

  // Swap buffers to render the final frame
  eglSwapBuffers(state->egl_display, state->egl_surface);

  if (state->pam.auth_state.auth_failed)
    sleep(1);
}

void render_lock_screen(struct client_state* state)
{
  // Bind the context to ensure it's current
  if (!eglMakeCurrent(state->egl_display, state->egl_surface, state->egl_surface,
                      state->egl_context))
  {
    log_message(LOG_LEVEL_ERROR, "Failed to make EGL context current for rendering");
    return;
  }

  // Initialize static resources on first run
  static GLuint texture                = 0;
  static GLuint texture_shader_program = 0;
  static int    initialized            = 0;

  if (!initialized)
  {
    // Load the background image path from the TOML configuration
    log_message(LOG_LEVEL_WARN, "EGL not initialized.");
    texture = load_texture(state->global_config.bg_path); // Use the bg path here
    // Create shader program for texture
    texture_shader_program = create_texture_shader_program();
    initialized            = 1;
  }

  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT);

  // First render the texture
  glUseProgram(texture_shader_program);

  // Set up texture vertices and coordinates
  GLint position_loc = glGetAttribLocation(texture_shader_program, "position");
  GLint texcoord_loc = glGetAttribLocation(texture_shader_program, "texCoord");

  glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, 0, quad_vertices);
  glEnableVertexAttribArray(position_loc);

  glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, tex_coords);
  glEnableVertexAttribArray(texcoord_loc);

  // Bind and render texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(glGetUniformLocation(texture_shader_program, "uTexture"), 0);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Then render the triangle
  update_time_texture(state);
  render_time_box(state);
  render_password_field(state);
}

#endif
