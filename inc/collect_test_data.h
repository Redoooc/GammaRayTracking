#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern volatile int isAllowRecord;

void collect_test_data();
void collect_test_data_horizontal();
void collect_test_data_vertical();
void collect_test_data_30degree();
void collect_test_data_45degree();
void collect_test_data_60degree();
void collect_test_data_static();
void always_record();
char *array_int_to_char(int *array, int array_len);