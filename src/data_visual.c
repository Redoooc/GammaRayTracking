#include "data_visual.h"

DataPoint data[MAX_WINDOW_RENDERER][MAX_POINTS];
int data_count[MAX_WINDOW_RENDERER] = {0};
SDL_Window* window[MAX_WINDOW_RENDERER] = {NULL};
SDL_Renderer* renderer[MAX_WINDOW_RENDERER] = {NULL};
// 加载字体
TTF_Font* font;
SDL_Color text_color = {255, 255, 255, 255};

void init_font(){
    font = TTF_OpenFont("Arial.ttf", 16);
    if (!font) {
        SDL_Log("Failed to load font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
    }
}

// 初始化图形窗口
void init_graph(int SDL_index) {
    // SDL_Init(SDL_INIT_VIDEO);
    char winName[100];
    sprintf(winName,"Window%d",SDL_index);
    window[SDL_index] = SDL_CreateWindow(winName, 
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        0);
    renderer[SDL_index] = SDL_CreateRenderer(window[SDL_index], -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

// 添加数据点
void add_data(float y, int winIndex) {
    if (data_count[winIndex] < MAX_POINTS) {
        data[winIndex][data_count[winIndex]].x = data_count[winIndex];
        data[winIndex][data_count[winIndex]].y = y;
        data_count[winIndex]++;
    } else {
        // 移除最旧的数据
        for(int i=0; i<MAX_POINTS-1; i++) {
            data[winIndex][i] = data[winIndex][i+1];
            data[winIndex][i].x = i;
        }
        data[winIndex][MAX_POINTS-1].x = MAX_POINTS-1;
        data[winIndex][MAX_POINTS-1].y = y;
    }
}

// 绘制坐标轴
void draw_axes(float min_y, float max_y, int winIndex) {
    SDL_SetRenderDrawColor(renderer[winIndex], 255, 255, 255, 255);
    
    // 绘制坐标轴
    SDL_RenderDrawLine(renderer[winIndex], 50, 50, 50, WINDOW_HEIGHT-50);  // Y轴
    SDL_RenderDrawLine(renderer[winIndex], 50, WINDOW_HEIGHT-50, 
                      WINDOW_WIDTH-50, WINDOW_HEIGHT-50); // X轴

    // 绘制刻度（示例）
    char buffer[20];
    for(int i=0; i<=10; i++) {
        int y = WINDOW_HEIGHT-50 - i*(WINDOW_HEIGHT-100)/10;
        SDL_RenderDrawLine(renderer[winIndex], 45, y, 50, y);
        
        float value = min_y + (max_y-min_y)*(i/10.0);
        sprintf(buffer, "%.1f", value);
        // 横坐标数值标注
        SDL_Surface* text_surface = TTF_RenderText_Solid(font, buffer, text_color);
        if (!text_surface) continue;

        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer[winIndex], text_surface);
        if (!text_texture) {
            SDL_FreeSurface(text_surface);
            continue;
        }

        SDL_Rect text_rect = {
            10,                        // X位置（左侧）
            y - text_surface->h / 2, // 垂直居中
            text_surface->w,
            text_surface->h
        };

        SDL_RenderCopy(renderer[winIndex], text_texture, NULL, &text_rect);
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
    }
}

// 主绘图函数
void plot_data(int winIndex) {
    if(data_count[winIndex] < 2) return;

    // 计算数据范围
    float min_y = data[winIndex][0].y;
    float max_y = data[winIndex][0].y;
    for(int i=0; i<data_count[winIndex]; i++) {
        if(data[winIndex][i].y < min_y) min_y = data[winIndex][i].y;
        if(data[winIndex][i].y > max_y) max_y = data[winIndex][i].y;
    }
    float y_range = max_y - min_y;
    if(y_range == 0) y_range = 1;

    // 清空画布
    SDL_SetRenderDrawColor(renderer[winIndex], 0, 0, 0, 255);
    SDL_RenderClear(renderer[winIndex]);

    // 绘制坐标轴
    draw_axes(min_y, max_y, winIndex);

    // 数据映射到屏幕坐标
    SDL_Point screen_points[MAX_POINTS];
    float x_scale = (WINDOW_WIDTH-100.0)/(data_count[winIndex] > 1 ? data_count[winIndex]-1 : 1);
    float y_scale = (WINDOW_HEIGHT-100.0)/y_range;

    for(int i=0; i<data_count[winIndex]; i++) {
        screen_points[i].x = 50 + (int)(data[winIndex][i].x * x_scale);
        screen_points[i].y = WINDOW_HEIGHT-50 - (int)((data[winIndex][i].y - min_y) * y_scale);
    }

    // 绘制折线
    SDL_SetRenderDrawColor(renderer[winIndex], 0, 255, 0, 255);
    for(int i=0; i<data_count[winIndex]-1; i++) {
        SDL_RenderDrawLine(renderer[winIndex], 
            screen_points[i].x, screen_points[i].y,
            screen_points[i+1].x, screen_points[i+1].y);
    }

    // 更新显示
    SDL_RenderPresent(renderer[winIndex]);
}

// 处理窗口事件
void process_events(int winIndex) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer[winIndex]);
            SDL_DestroyWindow(window[winIndex]);
            SDL_Quit();
            exit(0);
        }
    }
}

void plot_show(float plot_input, int winIndex){
    add_data(plot_input, winIndex);
    plot_data(winIndex);
    process_events(winIndex);
}