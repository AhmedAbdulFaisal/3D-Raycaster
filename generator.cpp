#include "generator.h"
#include <cstring>
#include <vector>


/**
 * 
 * 
 * 
 */

void fillArray(int arr[genWidth][genHeight], int i) {
    for (int x=0; x < genWidth; x++) {
        for (int y=0; y < genHeight; y++) {
            arr[x][y] = 1;
            
        }
    }
    printArray(arr);
}


void CellularAutomataGenerate(int arr[genWidth][genHeight], int buf[genWidth][genHeight], float fill, int stepcount) {
    /* resetting automata */
    srand(time(NULL));

    fillArray(arr,1);
    
    for (int x = 1; x < genWidth - 1; x++) {
        for (int y = 1; y < genHeight - 1; y++) {
            //int i = rand() % 2;
            float r = (float)rand()/(float)RAND_MAX;
            arr[x][y] = r < fill ? 1 : 0;
            
        }
    }

    printArray(arr);
    

    //printArray(arr);
    for (int i=0;i<stepcount;i++) {
        step(arr,buf,4);
    }
    //printArray(arr);
    //step(arr,buf,4);
}

int getNeighborCellCount(int arr[genWidth][genHeight], int x, int y) {
    int count = 0;
    if (x > 0) {
        count += arr[x-1][y];
        if (y > 0) {
            count += arr[x-1][y-1];
        }
    }

    if (y > 0) {
        count += arr[x][y-1];
        if (y < genWidth - 1) {
            count += arr[x+1][y-1];
        }
    }

    if (x < genWidth - 1) {
        count += arr[x+1][y];
        if (y < genHeight - 1) {
            count += arr[x+1][y+1];
        }
    }

    if (y < genHeight - 1) {
        count += arr[x][y+1];
        if (x > 0) {
            count += arr[x-1][y+1];
        }
    }

    return count;
}

void step(int arr[genWidth][genHeight],int buf[genWidth][genHeight],int req) {

    
    
    

    //printArray(buf);
    //printArray(arr);
    for (int x = 0; x < genWidth; x++) {
        for (int y = 0; y < genHeight; y++) {
            //printf("%d ",buf[y][x]); //leads to segfault, accessing buf[x][y] in general
            int i = arr[x][y];
            //printf("%d ",i);
            buf[x][y] = i; //SEGFAULT HERE!!!!!!!
        }
    }

    //printArray(buf);
    //copy memory because idfk why you can't just directly assign it...
    //memcpy(buffer,arr,sizeof(arr));
    int livereq = req;

    for (int x = 1; x < genWidth - 1; x++) {
        for (int y = 1; y < genHeight - 1; y++) {
            int livecount = arr[x][y] + getNeighborCellCount(arr,x,y);
            //printf("%d,%d ",livecount,livereq);
            //if (livecount > livereq) {
            //    buf[x][y] = 1;
            //}else {
            //    buf[x][y] = 0;
            //}
            buf[x][y] = livecount > livereq ? 1 : 0;
        }
        //printf("\n");
    }

    for (int x = 0; x < genWidth; x++) {
        for (int y = 0; y < genHeight; y++) {
            arr[x][y] = buf[x][y];
        }
    }


}

void printArray(int arr[genWidth][genHeight]) {
    printf("RESULT:\n");
    for (int x = 0; x < genWidth; x++) {
        for (int y = 0; y < genHeight; y++) {
            printf("%d ",arr[x][y]);
        }
        printf("\n");
    }
}

void printMap(int arr[genWidth][genHeight]) {
    printf("LEVEL MAP:\n");
    for (int x = 0; x < genWidth; x++) {
        for (int y = 0; y < genHeight; y++) {
            if (arr[x][y] != 0) {
                printf(" ");
            }else {
                printf("X");
            }
        }
        printf("\n");
    }
}

/* dimensions MUST be the same as the main map dimensions or else this will segfault!!!!! */
void copyMap(int arr[genWidth][genHeight], int ceil[genWidth][genHeight], int walls[genWidth][genHeight], int floor[genWidth][genHeight]) {
    for (int x = 0; x < genWidth; x++) {
        for (int y = 0; y < genHeight; y++) {
            int walltile = 4;
            int ceiltile = 8;
            int floortile = 8;
            
            if (arr[x][y] == 1) {
                ceil[x][y] = ceiltile;
                walls[x][y] = walltile;
                floor[x][y] = floortile; 

            }else if (arr[x][y] == 0) {
                ceil[x][y] = ceiltile;
                walls[x][y] = 0;
                floor[x][y] = floortile; 
            }
        }
    }
}