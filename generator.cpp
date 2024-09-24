#include "generator.h"
#include <cstring>
#include <vector>

/**
 * Contains various terrain generator algorithims
 * Caves - Cellular Automata
 * Dungeons - To be decided
 *
 */

void fillArray(int arr[genWidth][genHeight], int i)
{
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            arr[x][y] = 1;
        }
    }
    //printArray(arr);
}

void CellularAutomataGenerate(int arr[genWidth][genHeight], int buf[genWidth][genHeight], float fill, int stepcount)
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
            arr[x][y] = r < fill ? 1 : 0;
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

int getNeighborCellCount(int arr[genWidth][genHeight], int x, int y)
{
    int count = 0;
    if (x > 0)
    {
        count += arr[x - 1][y];
        if (y > 0)
        {
            count += arr[x - 1][y - 1];
        }
    }

    if (y > 0)
    {
        count += arr[x][y - 1];
        if (y < genWidth - 1)
        {
            count += arr[x + 1][y - 1];
        }
    }

    if (x < genWidth - 1)
    {
        count += arr[x + 1][y];
        if (y < genHeight - 1)
        {
            count += arr[x + 1][y + 1];
        }
    }

    if (y < genHeight - 1)
    {
        count += arr[x][y + 1];
        if (x > 0)
        {
            count += arr[x - 1][y + 1];
        }
    }

    return count;
}

void step(int arr[genWidth][genHeight], int buf[genWidth][genHeight], int req)
{

    // printArray(buf);
    // printArray(arr);
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            // printf("%d ",buf[y][x]); //leads to segfault, accessing buf[x][y] in general
            int i = arr[x][y];
            // printf("%d ",i);
            buf[x][y] = i; // SEGFAULT HERE!!!!!!!
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
            int livecount = arr[x][y] + getNeighborCellCount(arr, x, y);
            // printf("%d,%d ",livecount,livereq);
            // if (livecount > livereq) {
            //     buf[x][y] = 1;
            // }else {
            //     buf[x][y] = 0;
            // }
            buf[x][y] = livecount > livereq ? 1 : 0;
        }
        // printf("\n");
    }

    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            arr[x][y] = buf[x][y];
        }
    }
}

void printArray(int arr[genWidth][genHeight])
{
    printf("RESULT:\n");
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            printf("%d ", arr[x][y]);
        }
        printf("\n");
    }
}

void printMap(int arr[genWidth][genHeight])
{
    printf("LEVEL MAP:\n");
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            if (arr[x][y] != 0)
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

void generateStaticPositions(int arr[genWidth][genHeight], struct Sprite list[], int type[])
{
    float p = 0.2f;
    int c = 0;
    srand(time(NULL));
    for (int xPos = 0; xPos < genWidth; xPos++)
    {
        for (int yPos = 0; yPos < genHeight; yPos++)
        {
            float r = (float)rand() / (float)RAND_MAX;
            if ((arr[xPos][yPos] == 0))
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
void copyMap(int arr[genWidth][genHeight], int ceil[genWidth][genHeight], int walls[genWidth][genHeight], int floor[genWidth][genHeight])
{
    //printArray(walls);
    for (int x = 0; x < genWidth; x++)
    {
        for (int y = 0; y < genHeight; y++)
        {
            int walltile = 10; // must be minus 1
            int ceiltile = 10;
            int rubbletile = 10;
            int floortile = 10;

            if (arr[x][y] == 1)
            {

                ceil[x][y] = ceiltile;
                walls[x][y] = walltile;
                floor[x][y] = floortile;
            }
            else if (arr[x][y] == 0)
            {
                ceil[x][y] = ceiltile;
                walls[x][y] = 0;
                floor[x][y] = floortile;
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
            

            if (arr[x][y] == 1)
            {
                if (((arr[x - 1][y] == 0) && (arr[x + 1][y]) == 0) || ((arr[x][y-1] == 0) && (arr[x][y+1]) == 0))
                {
                    //arr[x][y] = 5;
                    walls[x][y] = rubbletile;
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
