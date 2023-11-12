#include "SDL_ttf.h"
#include "display.h"
#include "triangle.h"

enum cull_method cull_method;
enum render_method render_method;

int win_width = 800;
int win_height = 600;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

uint32_t *colour_buf = NULL;
SDL_Texture *colour_buf_tex = NULL;

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

void draw_ui(SDL_Renderer *renderer)
{
    int font_size = 12;
    TTF_Font *font = TTF_OpenFont("./assets/fonts/FiraCode-Regular.ttf", font_size);
    if (!font) {
        fprintf(stderr, "error loading font: %s\n", SDL_GetError());
    }

    char *ui = "<1> - wire\n<2> - wire vertex\n<3> - fill triangle\n<4> - fill triangle wire\n<c> - cull backface\n<d> - cull none\n<mouse-wheel> zoom in/out\n<esc> - quit";
    SDL_Color white = { 62, 81, 100, 255 };

    SDL_Surface *msg_surface = TTF_RenderText_Solid_Wrapped(font, ui, white, 0);
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
    msg_rect.x = 15;
    msg_rect.y = 10;
    msg_rect.w = w;
    msg_rect.h = h;

    SDL_RenderCopy(renderer, msg_tex, NULL, &msg_rect);
    SDL_DestroyTexture(msg_tex);
    TTF_CloseFont(font);
}

void cleanup(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
