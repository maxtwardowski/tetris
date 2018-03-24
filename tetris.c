#include "primlib.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

//BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE
//0-7

#define ROWS 20
#define COLUMNS 10

//the main structures of the game  newblockonscreen
int board[ROWS][COLUMNS],
    currentpiece_shape,
    currentpiece_variant,
    distancetodrop;
bool newblockonscreen = false;

void keyDetect();
void drawBoard();
int randomInt(int range_start, int range_end);
void pickNewBlock();
void drawBlocks();
void testPrintBoard();

int main(int argc, char* argv[]) {

    if(initGraph()) {
        exit(3);
    }

    while(1) {
        drawBoard();
        drawBlocks();
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
    const int OFFSET = 10, DISPLACEMENT = 2;
    int y1cord = OFFSET - DISPLACEMENT,
        y2cord = screenHeight() - OFFSET + DISPLACEMENT,
        blockwidth = (y2cord - y1cord) / ROWS,
        x1cord = (screenWidth() - blockwidth * COLUMNS) / 2 - DISPLACEMENT,
        x2cord = screenWidth() - x1cord + DISPLACEMENT;
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

void pickNewBlock() {
    if (newblockonscreen == false) {
        currentpiece_shape = randomInt(0, 3);
        currentpiece_shape = 0;
    }
}

void drawBlocks() {
    const int OFFSET = 10, BLOCKWIDTH = (screenHeight() - 2 * OFFSET) / ROWS;
    int x_base = (screenWidth() - BLOCKWIDTH * COLUMNS) / 2,
        y_base = screenHeight() - OFFSET,
        x_cord,
        y_cord = y_base;

    for (int i = 0; i < ROWS; i++) {
        x_cord = x_base;

        for (int j = 0; j < COLUMNS; j++) {
            if (board[i][j]) {
                filledRect(x_cord,
                           y_cord,
                           x_cord + BLOCKWIDTH,
                           y_cord - BLOCKWIDTH,
                           board[i][j]);
                x_cord += BLOCKWIDTH;
            }
        }
        y_cord += BLOCKWIDTH;
    }
}

void testPrintBoard() {
    printf("=====PRINTBOARD TEST=====");
    for (int i = 0; i < ROWS; i++) {
        printf("\n");
        for (int j = 0; j < COLUMNS; j++) {
            printf("%d\t", board[i][j]);
        }
    }
    printf("=====PRINTBOARD TEST=====");
}
