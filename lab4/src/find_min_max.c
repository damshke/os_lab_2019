#include "find_min_max.h"

#include <limits.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  
  for (int i = begin; i < end; i++){ // цикл поиска
    if (*(array + i) > min_max.max){   // для максимального элемента
        min_max.max = *(array+i);
        }
    if (*(array + i) < min_max.min){ // для минимального элемента
        min_max.min = *(array + i);
        }
    }
    
  return min_max; // возвращение максимального и минимального элементов
}
