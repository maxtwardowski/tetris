#include "primlib.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

//BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE
//0-7

#define ROWS 20
#define COLUMNS 10
#define BLOCK_ROWS 4
#define BLOCK_COLUMNS 4

//the main structures of the game  newblockonscreen
int board[ROWS][COLUMNS],
    newblock[BLOCK_ROWS][BLOCK_COLUMNS],
    distancetodrop;
bool newblockonscreen = false;


void keyDetect();
void drawBoard();
int randomInt(int range_start, int range_end);
void createNewBlock();
void testPrintBoard();

int main(int argc, char* argv[]) {

    if(initGraph()) {
        exit(3);
    }

    while(1) {
        drawBoard();
        //createNewBlock();
        updateScreen();
        keyDetect();
    }

    return 0;
}

void keyDetect() {
    int key = getkey();
    if (key == SDLK_ESCAPE)
        exit(1);
}

void drawBoard() {
    int offset = 10,
        y1cord = offset,
        y2cord = screenHeight() - offset,
        blockwidth = (y2cord - y1cord) / ROWS,
        x1cord = (screenWidth() - blockwidth * COLUMNS) / 2,
        x2cord = screenWidth() - x1cord;
    line(x1cord, y1cord, x2cord, y1cord, WHITE); //TOP
    line(x1cord, y2cord, x2cord, y2cord, WHITE); //BOTTOM
    line(x1cord, y1cord, x1cord, y2cord, WHITE); //LEFT
    line(x2cord, y1cord, x2cord, y2cord, WHITE); //RIGHT
}

int randomInt(int range_start, int range_end) {
    srand(time(NULL));
    int random_int = rand() % (range_end - range_start + 1) + range_start; //+1 to avoid black
    return random_int;
}

/*void createNewBlock() {
    if (newblockonscreen == false) {
        int newblock_size = randomInt(2, 15),
            newblock_color = randomizeColor();


        newblockonscreen = true;
    }
}*/

void testPrintBoard() {
    for(int i = 0; i < ROWS; i++) {
        printf("\n");
        for (int j = 0; j < COLUMNS; j++) {
            printf("%d\t", board[i][j]);
        }
    }
}
