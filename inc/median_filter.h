#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H
typedef struct {
    float* window;      // 滑动窗口数组
    int* indices;       // 维护有序索引的环形缓冲区
    int window_size;    // 最大窗口尺寸（必须为奇数）
    int count;          // 当前窗口中的有效数据个数
    int head;           // 最新数据位置
    bool initialized;   // 窗口是否已填满标志
} MedianFilterStruct;
#endif

MedianFilterStruct* median_filter_init(int window_size);
void median_filter_free(MedianFilterStruct* filter);
static int find_insert_pos(const float* window, const int* indices, int count, float value);
float median_filter_update(MedianFilterStruct* filter, float new_value);