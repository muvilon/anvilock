#ifndef XDG_SURFACE_HANDLER_H
#define XDG_SURFACE_HANDLER_H

#include <wayland-client.h>
#include <string.h>
#include "client_state.h"
#include "log.h"

static struct wl_buffer* draw_lock_screen(struct client_state* state) {
    const int width = state->output_state.width;
    const int height = state->output_state.height;
    int stride = width * 4;
    int size = stride * height;

    int fd = allocate_shm_file(size);
    if (fd == -1) {
        log_message(LOG_LEVEL_ERROR, "Failed to allocate shared memory file");
        return NULL;
    }

    uint32_t* data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        log_message(LOG_LEVEL_ERROR, "Memory mapping failed: %s", strerror(errno));
        close(fd);
        return NULL;
    }

    struct wl_shm_pool* pool = wl_shm_create_pool(state->wl_shm, fd, size);
    if (!pool) {
        log_message(LOG_LEVEL_ERROR, "Failed to create SHM pool: %s", strerror(errno));
        munmap(data, size);
        close(fd);
        return NULL;
    }

    struct wl_buffer* buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    if (!buffer) {
        log_message(LOG_LEVEL_ERROR, "Failed to create buffer: %s", strerror(errno));
        wl_shm_pool_destroy(pool);
        munmap(data, size);
        close(fd);
        return NULL;
    }

    // Draw a gradient background
    for (int y = 0; y < height; ++y) {
        // Calculate the color for this row
        uint8_t r = 102 + (y * 153) / height; // Gradient from 102 to 255
        uint8_t g = 102 + (y * 153) / height; // Gradient from 102 to 255
        uint8_t b = 102 + (y * 153) / height; // Gradient from 102 to 255
        uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b; // ARGB format

        for (int x = 0; x < width; ++x) {
            data[y * width + x] = color;
        }
    }

    // Draw password prompt box
    int box_width = 300;
    int box_height = 50;
    int box_x = (width - box_width) / 2;
    int box_y = (height - box_height) / 2;

    // Draw password box background
    for (int y = box_y; y < box_y + box_height; ++y) {
        for (int x = box_x; x < box_x + box_width; ++x) {
            data[y * width + x] = 0xFF222222;  // Darker gray for password box
        }
    }

    // Display the password input
    int password_length = strlen(state->password);
    int text_x = box_x + 10; // Padding from left
    int text_y = box_y + (box_height - 20) / 2; // Center vertically

    // Draw each character of the password
    for (int i = 0; i < password_length; i++) {
        uint32_t color = 0xFFFFFFFF; // White for password text
        int char_x = text_x + (i * 15); // Assuming a fixed-width font of 15 pixels
        // Here, you can implement your own character drawing logic
        // For demonstration, we just fill a pixel area for each character
        for (int dy = -5; dy < 5; dy++) {
            for (int dx = -5; dx < 5; dx++) {
                if (char_x + dx >= box_x && char_x + dx < box_x + box_width &&
                    text_y + dy >= box_y && text_y + dy < box_y + box_height) {
                    data[(text_y + dy) * width + (char_x + dx)] = color;
                }
            }
        }
    }

    munmap(data, size);
    wl_shm_pool_destroy(pool);
    close(fd);
    return buffer;
}

static void xdg_surface_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
    struct client_state* state = data;
    xdg_surface_ack_configure(xdg_surface, serial);

    struct wl_buffer* buffer = draw_lock_screen(state);
    if (!buffer) {
        log_message(LOG_LEVEL_ERROR, "Failed to create buffer for lock screen");
        return;
    }

    wl_surface_attach(state->wl_surface, buffer, 0, 0);
    wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

#endif
