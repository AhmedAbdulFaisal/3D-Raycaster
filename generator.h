#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "entity.h"

#define genWidth 24
#define genHeight 24

//https://bronsonzgeb.com/index.php/2022/01/30/procedural-generation-with-cellular-automata/


void CellularAutomataGenerate(int arr[genWidth][genHeight], int buf[genWidth][genHeight], float fill, int stepcount);

void DungeonGenerator(int arr[genWidth][genHeight], int buf[genWidth][genHeight]);

int getNeighborCellCount(int arr[genWidth][genHeight], int x, int y);

void step(int arr[genWidth][genHeight],int buf[genWidth][genHeight],int req);

void printArray(int arr[genWidth][genHeight]);

void printMap(int arr[genWidth][genHeight]);

void copyMap(int arr[genWidth][genHeight], int ceil[genWidth][genHeight], int walls[genWidth][genHeight], int floor[genWidth][genHeight]);

void fillArray(int arr[genWidth][genHeight], int i);

void generateStaticPositions(int arr[genWidth][genHeight],struct Sprite list_entity[], int type[]);

