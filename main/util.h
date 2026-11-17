#ifndef UTIL_H
#define UTIL_H

#include <stdint.h> 

/**
 * @brief Function sorts the array using bubble sort algoritmus
 * @param array Array to be sorted
 * @param n Number of elements in array
 */
void bubble_sort(int * array, int n);

/**
 * @brief Function calculates median value from sorted array
 * @param sorted_array Sorted array 
 * @param n Number of elements in array
 * @return Median value
 */
int calculate_median(const int *sorted_array, int n);

#endif
