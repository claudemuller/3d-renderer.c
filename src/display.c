#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_ttf.h"
#include "display.h"
#include "light.h"
#include "triangle.h"

enum cull_method cull_method;
enum render_method render_method;

float rot = 0.01;
bool is_paused = false;

int win_width = 800;
int win_height = 600;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

uint32_t *colour_buf = NULL;
SDL_Texture *colour_buf_tex = NULL;
float *z_buf = NULL;

bool init_win(const bool debug)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "error initialising SDL: %s\n", SDL_GetError());
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    win_width = display_mode.w;
    win_height = display_mode.h;

    window = SDL_CreateWindow(
        "3d Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        win_width,
        win_height,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "error creating window: %s\n", SDL_GetError());
        return false;
    }

    // TODO: get the resolution of the monitor that we're running on and use that
    // int cur_display_idx = SDL_GetWindowDisplayIndex(window);

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "error creating renderer: %s\n", SDL_GetError());
        return false;
    }

    TTF_Init();

    if (!debug) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }

    return true;
}

void clear_colour_buf(const uint32_t colour)
{
    for (size_t y = 0; y < (size_t)win_height; y++) {
        for (size_t x = 0; x < (size_t)win_width; x++) {
            colour_buf[(win_width * y) + x] = colour;
        }
    }
}

void clear_z_buf(void)
{
    for (size_t y = 0; y < (size_t)win_height; y++) {
        for (size_t x = 0; x < (size_t)win_width; x++) {
            z_buf[(win_width * y) + x] = 1.0;
        }
    }
}

void render_colour_buf(void)
{
    SDL_UpdateTexture(
        colour_buf_tex,
        NULL,
        colour_buf,
        win_width * sizeof(uint32_t)
    );
    SDL_RenderCopy(renderer, colour_buf_tex, NULL, NULL);
}

void draw_pixel(const int x, const int y, const uint32_t colour)
{
    if (x >= 0 && x < win_width && y >= 0 && y < win_height) {
        colour_buf[(win_width * y) + x] = colour;
    }
}

void draw_line(const int x0, const int y0, const int x1, const int y1, const uint32_t colour)
{
    // m = Δx/Δy = rise/run
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int longest_side_len = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    // Calculate the x and y increment at which to draw
    float x_inc = delta_x / (float)longest_side_len;
    float y_inc = delta_y / (float)longest_side_len;

    // The current x and y at which to draw a pixel
    float cur_x = x0;
    float cur_y = y0;

    for (size_t i = 0; i <= (size_t)longest_side_len; i++) {
        draw_pixel(round(cur_x), round(cur_y), colour);
        cur_x += x_inc;
        cur_y += y_inc;
    }
}

void draw_triangle(
    const int x0, const int y0,
    const int x1, const int y1,
    const int x2, const int y2,
    uint32_t colour
)
{
    draw_line(x0, y0, x1, y1, colour);
    draw_line(x1, y1, x2, y2, colour);
    draw_line(x2, y2, x0, y0, colour);
}

void draw_rect(const int x, const int y, const int w, const int h, const uint32_t colour)
{
    int cur_x = 0, cur_y = 0;
    for (size_t i = 0; i < (size_t)w; i++) {
        for (size_t j = 0; j < (size_t)h; j++) {
            cur_x = x + i;
            cur_y = y + j;
            draw_pixel(cur_x, cur_y, colour);
        }
    }
}

void draw_grid(void)
{
    for (size_t y = 0; y < (size_t)win_height; y += 10) {
        for (size_t x = 0; x < (size_t)win_width; x += 10) {
            colour_buf[(win_width * y) + x] = 0xFF333333;
        }
    }
}

#define UI_LEN 12

void draw_ui(SDL_Renderer *renderer)
{
    int font_size = 12;
    TTF_Font *font = TTF_OpenFont("./assets/fonts/FiraCode-Regular.ttf", font_size);
    if (!font) {
        fprintf(stderr, "error loading font: %s\n", SDL_GetError());
    }

    char *ui[UI_LEN] = {
        "<1> - wire",
        "<2> - wire vertex",
        "<3> - fill triangle",
        "<4> - fill triangle wire",
        "<5> - turn on lighting",
        "<6> - textured",
        "<7> - textured wire",
        "<c> - cull backface",
        "<d> - cull none",
        "<p> - pause",
        "<mouse-wheel> zoom in/out",
        "<esc> - quit"
    };
    SDL_Color white = { 62, 81, 100, 255 };
    SDL_Color green = { 159, 226, 191, 255 };

    // char r[20] = { 0 };
    // sprintf(r, "rot:%f", rot);
    // draw_text(renderer, font, r, win_width - 100, 15 * 0 + 10, green);

    // TODO: make this better 😒
    if (render_method == RENDER_WIRE) {
        draw_text(renderer, font, ui[0], 15, 15 * 0 + 10, green);
    } else {
        draw_text(renderer, font, ui[0], 15, 15 * 0 + 10, white);
    }

    if (render_method == RENDER_WIRE_VERTEX) {
        draw_text(renderer, font, ui[1], 15, 15 * 1 + 10, green);
    } else {
        draw_text(renderer, font, ui[1], 15, 15 * 1 + 10, white);
    }

    if (render_method == RENDER_FILL_TRIANGLE) {
        draw_text(renderer, font, ui[2], 15, 15 * 2 + 10, green);
    } else {
        draw_text(renderer, font, ui[2], 15, 15 * 2 + 10, white);
    }

    if (render_method == RENDER_FILL_TRIANGLE_WIRE) {
        draw_text(renderer, font, ui[3], 15, 15 * 3 + 10, green);
    } else {
        draw_text(renderer, font, ui[3], 15, 15 * 3 + 10, white);
    }

    if (lighting) {
        draw_text(renderer, font, ui[4], 15, 15 * 4 + 10, green);
    } else {
        draw_text(renderer, font, ui[4], 15, 15 * 4 + 10, white);
    }

    if (render_method == RENDER_TEXTURED) {
        draw_text(renderer, font, ui[5], 15, 15 * 5 + 10, green);
    } else {
        draw_text(renderer, font, ui[5], 15, 15 * 5 + 10, white);
    }

    if (render_method == RENDER_TEXTURED_WIRE) {
        draw_text(renderer, font, ui[6], 15, 15 * 6 + 10, green);
    } else {
        draw_text(renderer, font, ui[6], 15, 15 * 6 + 10, white);
    }

    if (cull_method == CULL_BACKFACE) {
        draw_text(renderer, font, ui[7], 15, 15 * 7 + 10, green);
    } else {
        draw_text(renderer, font, ui[7], 15, 15 * 7 + 10, white);
    }

    if (cull_method == CULL_NONE) {
        draw_text(renderer, font, ui[8], 15, 15 * 8 + 10, green);
    } else {
        draw_text(renderer, font, ui[8], 15, 15 * 8 + 10, white);
    }

    if (is_paused) {
        draw_text(renderer, font, ui[9], 15, 15 * 9 + 10, green);
    } else {
        draw_text(renderer, font, ui[9], 15, 15 * 9 + 10, white);
    }

    for (size_t i = 10; i < UI_LEN; i++) {
        draw_text(renderer, font, ui[i], 15, 15 * i + 10, white);
    }

    TTF_CloseFont(font);
}

void draw_text(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const char *text,
    const int x,
    const int y,
    SDL_Color colour
)
{
    SDL_Surface *msg_surface = TTF_RenderText_Solid(font, text, colour);
    if (!msg_surface) {
        fprintf(stderr, "error creating surface: %s\n", SDL_GetError());
    }

    SDL_Texture *msg_tex = SDL_CreateTextureFromSurface(renderer, msg_surface);
    if (!msg_tex) {
        fprintf(stderr, "error creating texture: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(msg_surface);

    int w = 0, h = 0;
    SDL_QueryTexture(msg_tex, NULL, NULL, &w, &h);
    SDL_Rect msg_rect;
    msg_rect.x = x;
    msg_rect.y = y;
    msg_rect.w = w;
    msg_rect.h = h;

    SDL_RenderCopy(renderer, msg_tex, NULL, &msg_rect);
    SDL_DestroyTexture(msg_tex);
}

void cleanup(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
