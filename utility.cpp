#include "utility.h"


/* does not work with sizeof size calculation */
void printArray(int arr[],int size) {
    printf("SIZE: %d\n",size);

    for (int x=0;x<size;x++) {
        for (int y=0;y<size;y++) {
            printf("%d ",arr[x * size + y]);
        }
        printf("\n");
    }
}