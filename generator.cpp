#include "generator.h"
#include <cstring>
#include <vector>
#include "main.h"

/**
 * Contains various terrain generator algorithims
 * Caves - Cellular Automata
 * Dungeons - To be decided
 * width * row + column
 */

void fillArray(int arr[], int i)
{
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            arr[mapHeight * x + y] = 1;
        }
    }
    //printArray(arr);
}

void CellularAutomataGenerate(int arr[], int buf[], float fill, int stepcount)
{
    /* resetting automata */
    srand(time(NULL));

    fillArray(arr, 1);

    for (int x = 1; x < genWidth - 1; x++)
    {
        for (int y = 1; y < genHeight - 1; y++)
        {
            // int i = rand() % 2;
            float r = (float)rand() / (float)RAND_MAX;
            arr[mapHeight * x + y] = r < fill ? 1 : 0;
        }
    }

    // printArray(arr);

    // printArray(arr);
    for (int i = 0; i < stepcount; i++)
    {
        step(arr, buf, 4);
    }
    // printArray(arr);
    // step(arr,buf,4);

}

int getNeighborCellCount(int arr[], int x, int y)
{
    int count = 0;
    if (x > 0)
    {
        count += arr[mapHeight * (x-1) + y];
        if (y > 0)
        {
            count += arr[mapHeight * (x-1) + (y-1)];
        }
    }

    if (y > 0)
    {
        count += arr[mapHeight * x + (y-1)];
        if (y < genWidth - 1)
        {
            count += arr[mapHeight * (x+1) + (y-1)];
        }
    }

    if (x < genWidth - 1)
    {
        count += arr[mapHeight * (x+1) + y];
        if (y < genHeight - 1)
        {
            count += arr[mapHeight * (x+1) + (y+1)];
        }
    }

    if (y < genHeight - 1)
    {
        count += arr[mapHeight * (x) + (y+1)];
        if (x > 0)
        {
            count += arr[mapHeight * (x-1) + (y+1)];
        }
    }

    return count;
}

void step(int arr[], int buf[], int req)
{

    // printArray(buf);
    // printArray(arr);
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            // printf("%d ",buf[y][x]); //leads to segfault, accessing buf[x][y] in general
            int i = arr[mapHeight * (x) + (y)];
            // printf("%d ",i);
            buf[mapHeight * (x) + (y)] = i; // SEGFAULT HERE!!!!!!!
        }
    }

    // printArray(buf);
    // copy memory because idfk why you can't just directly assign it...
    // memcpy(buffer,arr,sizeof(arr));
    int livereq = req;

    for (int x = 1; x < genWidth - 1; x++)
    {
        for (int y = 1; y < genHeight - 1; y++)
        {
            int livecount = arr[mapHeight * (x) + (y)] + getNeighborCellCount(arr, x, y);
            // printf("%d,%d ",livecount,livereq);
            // if (livecount > livereq) {
            //     buf[x][y] = 1;
            // }else {
            //     buf[x][y] = 0;
            // }
            buf[mapHeight * (x) + (y)] = livecount > livereq ? 1 : 0;
        }
        // printf("\n");
    }

    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            arr[mapHeight * (x) + (y)] = buf[mapHeight * (x) + (y)];
        }
    }
}

void printArray(int arr[])
{
    printf("RESULT:\n");
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            printf("%d ", arr[mapHeight * (x) + (y)]);
        }
        printf("\n");
    }
}

void printMap(int arr[])
{
    printf("LEVEL MAP:\n");
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            if (arr[mapHeight * (x) + (y)] != 0)
            {
                printf(" ");
            }
            else
            {
                printf("X");
            }
        }
        printf("\n");
    }
}

void generateStaticPositions(int arr[], struct Sprite list[], int type[])
{
    float p = 0.2f;
    int c = 0;
    srand(time(NULL));
    for (int xPos = 0; xPos < genWidth; xPos++)
    {
        for (int yPos = 0; yPos < genHeight; yPos++)
        {
            float r = (float)rand() / (float)RAND_MAX;
            if ((arr[mapHeight * (xPos) + (yPos)] == 0))
            {
                if ((c <= 32) && (r < p))
                {
                    int r = rand() % (sizeof(type) / sizeof(type[0]));
                    list[c].x = xPos + 0.5;
                    list[c].y = yPos + 0.5;
                    list[c].texture = type[r];
                    // LOL
                    c++;
                }
            }
        }
    }
}

/* dimensions MUST be the same as the main map dimensions or else this will segfault!!!!! */
void copyMap(int arr[],struct World& wld)
{
;
    
    //printArray(walls);
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            int walltile = 10; // must be minus 1
            int ceiltile = 10;
            int rubbletile = 10;
            int floortile = 10;

            if (arr[mapHeight * (x) + (y)] == 1)
            {

                wld.ceiling[mapHeight * (x) + (y)] = ceiltile;
                wld.walls[mapHeight * (x) + (y)] = walltile;
                wld.floor[mapHeight * (x) + (y)] = floortile;
            }
            else if (arr[mapHeight * (x) + (y)] == 0)
            {
                wld.ceiling[mapHeight * (x) + (y)] = ceiltile;
                wld.walls[mapHeight * (x) + (y)] = 0;
                wld.floor[mapHeight * (x) + (y)] = floortile;
            }
        }
    }
    //printArray(walls);
    //decoration section
    
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            
            int rubbletile = 13;
            

            if (arr[mapHeight * (x) + (y)] == 1)
            {
                if (((arr[mapHeight * (x-1) + (y)] == 0) && (arr[mapHeight * (x+1) + (y)]) == 0) || ((arr[mapHeight * (x) + (y-1)] == 0) && (arr[mapHeight * (x) + (y+1)]) == 0))
                {
                    //arr[x][y] = 5;
                    wld.walls[mapHeight * (x) + (y)] = rubbletile;
                }
                //((arr[x][y - 1] && arr[x][y + 1])) == 0)
            }
        }
    }
    //printArray(arr);
    
}

/**
 * (-1, 1)(0, 1)(1, 1)
 * (-1, 0)(0, 0)(1, 0)
 * (-1,-1)(0,-1)(1,-1)
 *
 *
 */
