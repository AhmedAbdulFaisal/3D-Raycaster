#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "entity.h"

#define genWidth 24
#define genHeight 24

//https://bronsonzgeb.com/index.php/2022/01/30/procedural-generation-with-cellular-automata/


void CellularAutomataGenerate(int arr[], int buf[], float fill, int stepcount);

void DungeonGenerator(int arr[], int buf[]);

int getNeighborCellCount(int arr[], int x, int y);

void step(int arr[],int buf[],int req);

void printArray(int arr[]);

void printMap(int arr[]);

void copyMap(int arr[],struct World& wld);

void fillArray(int arr[], int i);

void generateStaticPositions(int arr[],struct Sprite list_entity[], int type[]);

