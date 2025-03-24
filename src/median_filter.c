#include "median_filter.h"

// 初始化滤波器
MedianFilterStruct* median_filter_init(int window_size) {
    if (window_size % 2 == 0) {
        fprintf(stderr, "Error: Window size must be an odd number\n");
        return NULL;
    }

    MedianFilterStruct* filter = malloc(sizeof(MedianFilterStruct));
    filter->window = malloc(window_size * sizeof(float));
    filter->indices = malloc(window_size * sizeof(int));
    filter->window_size = window_size;
    filter->count = 0;
    filter->head = -1;
    filter->initialized = false;
    return filter;
}

// 释放滤波器资源
void median_filter_free(MedianFilterStruct* filter) {
    free(filter->window);
    free(filter->indices);
    free(filter);
}

// 二分查找插入位置（保持排序）
static int find_insert_pos(const float* window, const int* indices, int count, float value) {
    int left = 0;
    int right = count - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (window[indices[mid]] < value) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return left;
}

// 更新滤波器并返回当前中值
float median_filter_update(MedianFilterStruct* filter, float new_value) {
    // 环形缓冲区索引计算
    filter->head = (filter->head + 1) % filter->window_size;
    const int current_pos = filter->head;
    
    // 如果窗口已满，需要移除最旧的数据
    if (filter->count == filter->window_size) {
        // 查找要移除的旧数据在有序列表中的位置
        int remove_pos = 0;
        while (filter->indices[remove_pos] != current_pos) {
            remove_pos++;
        }
        
        // 将后面的元素前移
        for (int i = remove_pos; i < filter->count - 1; i++) {
            filter->indices[i] = filter->indices[i + 1];
        }
        filter->count--;
    }

    // 插入新数据到排序位置
    filter->window[current_pos] = new_value;
    int insert_pos = find_insert_pos(filter->window, filter->indices, filter->count, new_value);
    
    // 后移元素腾出插入位置
    for (int i = filter->count; i > insert_pos; i--) {
        filter->indices[i] = filter->indices[i - 1];
    }
    filter->indices[insert_pos] = current_pos;
    filter->count++;

    // 更新初始化标志
    if (!filter->initialized && filter->count == filter->window_size) {
        filter->initialized = true;
    }

    // 计算中值
    if (filter->count % 2 == 1) { // 奇数个元素
        return filter->window[filter->indices[filter->count / 2]];
    } else { // 偶数个元素
        int mid = filter->count / 2;
        return (filter->window[filter->indices[mid - 1]] + 
                filter->window[filter->indices[mid]]) / 2.0f;
    }
}