#define SDL_MAIN_HANDLED //通过宏禁用 SDL 的默认入口，从而从main函数而非WinMain作为程序入口
#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_POINTS 300
#define MAX_WINDOW_RENDERER 10

#ifndef DATA_VISUAL_H
#define DATA_VISUAL_H
typedef struct {
    float x;
    float y;
} DataPoint;
#endif

void init_font();
void init_graph(int SDL_index);
void add_data(float y, int winIndex);
void draw_axes(float min_y, float max_y, int winIndex);
void plot_data(int winIndex);
void process_events(int winIndex);
void plot_show(float plot_input, int winIndex);