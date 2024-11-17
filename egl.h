#ifndef EGL_H
#define EGL_H

#include "client_state.h"
#include "log.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <wayland-client.h>
#include <wayland-egl.h>

static const GLfloat vertices[] = {
  0.0f,  0.5f,  // Top vertex
  -0.5f, -0.5f, // Bottom left vertex
  0.5f,  -0.5f  // Bottom right vertex
};

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
                         EGL_OPENGL_ES2_BIT, // OpenGL ES 2.0 support
                         EGL_SURFACE_TYPE,
                         EGL_WINDOW_BIT, // Windowed rendering
                         EGL_RED_SIZE,        8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_NONE};
  EGLConfig config;
  EGLint    num_configs;
  if (!eglChooseConfig(state->egl_display, attribs, &config, 1, &num_configs) || num_configs < 1)
  {
    log_message(LOG_LEVEL_ERROR, "Failed to choose EGL config");
    exit(EXIT_FAILURE);
  }

  // Create an EGL context for OpenGL ES 2.0
  EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, // OpenGL ES 2.0 context
                              EGL_NONE};
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
  int width         = state->output_state.width > 0 ? state->output_state.width : 1920;
  int height        = state->output_state.height > 0 ? state->output_state.height : 1080;
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
}

static void render_triangle(struct client_state* state)
{
  // Clear the color buffer with black background
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Create and compile the vertex shader
  const char* vertex_shader_source = "attribute vec2 position;\n"
                                     "void main() {\n"
                                     "    gl_Position = vec4(position, 0.0, 1.0);\n"
                                     "}\n";
  GLuint      vertex_shader        = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  // Check for vertex shader compile errors
  GLint compile_status;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status == GL_FALSE)
  {
    fprintf(stderr, "Vertex shader compilation failed\n");
    exit(EXIT_FAILURE);
  }

  /*
   * $This is a basic implementation
   *
   * -> It will render a yellow triangle every time a keyboard event occurs
   *
   * -> If CTRL+BackSpace is detected, it will render red triangle
   *
   */
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  if (state->pam.password_index != 0 && !state->pam.authenticated)
  {
    const char* fragment_shader_source_1 =
      "void main() {\n"
      "    gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n" // Red color output
      "}\n";
    glShaderSource(fragment_shader, 1, &fragment_shader_source_1, NULL);
  }
  else if (state->pam.authenticated) {
    const char* fragment_shader_source_1 =
      "void main() {\n"
      "    gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n" // Red color output
      "}\n";
    glShaderSource(fragment_shader, 1, &fragment_shader_source_1, NULL);
  }
  else
  {
    const char* fragment_shader_source_2 =
      "void main() {\n"
      "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" // Red color output
      "}\n";
    glShaderSource(fragment_shader, 1, &fragment_shader_source_2, NULL);
  }
  
  glCompileShader(fragment_shader);

  // Check for fragment shader compile errors
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
  if (compile_status == GL_FALSE)
  {
    fprintf(stderr, "Fragment shader compilation failed\n");
    exit(EXIT_FAILURE);
  }

  // Link the vertex and fragment shaders into a program
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glUseProgram(shader_program); // Use the shader program

  // Bind the triangle vertex positions to the shader's "position" attribute
  GLint position_location = glGetAttribLocation(shader_program, "position");
  glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 0, vertices);
  glEnableVertexAttribArray(position_location);

  // Draw the triangle (3 vertices)
  glDrawArrays(GL_TRIANGLES, 0, 3);

  // Swap buffers (render the triangle on the screen)
  eglSwapBuffers(state->egl_display, state->egl_surface);
  if (state->pam.authenticated) sleep(1); // to let the user know we have authenticated successfully
}

#endif
