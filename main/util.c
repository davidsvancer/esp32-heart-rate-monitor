#include "util.h"

void bubble_sort(int * array, int n) 
{
    for (int i = 0; i < n - 1; i++) 
    {
        for (int j = 0; j < n - i - 1; j++) 
        {
            if (array[j] > array[j + 1]) 
            {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

int calculate_median(const int *sorted_array, int n) 
{
    if (n <= 0) return 0; 

    if (n % 2 != 0) 
    {
        return sorted_array[n / 2];
    } 
    else 
    {   
        return (sorted_array[(n - 1) / 2] + sorted_array[n / 2]) / 2;
    }
}

