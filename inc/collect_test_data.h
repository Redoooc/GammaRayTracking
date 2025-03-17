#include <stdio.h>
#include <stdlib.h>

extern volatile int isAllowRecord;

void collect_test_data();
char *array_int_to_char(int *array, int array_len);