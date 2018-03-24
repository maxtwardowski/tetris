#include "primlib.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

//BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE
//0-7

#define ROWS 20
#define COLUMNS 10

//the main structures of the game  newpieceonscreen
int board[ROWS][COLUMNS],
    boardinfo[COLUMNS],
    distancetodropinfo[4],
    piece_shape,
    piece_variant,
    piece_position,
    pieceinfo[4],
    distancetodrop;
bool newpieceonscreen = false;

void keyDetect();
void drawBoard();
int randomInt(int range_start, int range_end);
void pickNewPiece();
void drawBlocks();
void getBoardInfo();
int getPieceWidth();
void getPieceInfo();
void testPrintBoard();
void testPrintCurrentPiece();
void testPrintBoardInfo();
void testPrintPieceInfo();

int main(int argc, char* argv[]) {

    if(initGraph()) {
        exit(3);
    }

    while(1) {
        drawBoard();
        drawBlocks();
        updateScreen();
        pickNewPiece();
        testPrintCurrentPiece();
        testPrintPieceInfo();
        //testPrintBoardInfo();
        keyDetect();
    }

    return 0;
}

void keyDetect() {
    int key = pollkey();
    if (key == SDLK_UP) {
        if (piece_variant == 0) {
            piece_variant = 3;
            if (getPieceWidth() + piece_position >= COLUMNS - 1) {
                piece_position -= (getPieceWidth() + piece_position - (COLUMNS - 1));
            }
        } else {
            piece_variant--;
            if (getPieceWidth() + piece_position >= COLUMNS - 1) {
                piece_position -= (getPieceWidth() + piece_position - (COLUMNS - 1));
            }
        }
    } else if (key == SDLK_DOWN) {
        if (piece_variant == 3) {
            piece_variant = 0;
            if (getPieceWidth() + piece_position >= COLUMNS - 1) {
                piece_position -= (getPieceWidth() + piece_position - (COLUMNS - 1));
            }
        } else {
            piece_variant++;
            if (getPieceWidth() + piece_position >= COLUMNS - 1) {
                piece_position -= (getPieceWidth() + piece_position - (COLUMNS - 1));
            }
        }
    } else if (key == SDLK_LEFT) {
        if (piece_position) {
            piece_position--;
        }
    } else if (key == SDLK_RIGHT) {
        if (piece_position + getPieceWidth() < COLUMNS - 1)
            piece_position++;
    } else if (key == SDLK_RETURN) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {

            }
        }
    } else if (key == SDLK_ESCAPE)
        exit(1);
    getPieceInfo();
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

void pickNewPiece() {
    if (newpieceonscreen == false) {
        getBoardInfo();
        piece_shape = randomInt(0, 3);
        piece_variant = 0;
        getPieceInfo();
        newpieceonscreen = true;
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

void getBoardInfo() {
    int counter = -1;
    for (int i = 0; i < COLUMNS; i++) {
        counter = -1;
        for (int j = 0; j < ROWS; j++) {
            if (board[j][i])
                counter++;
            else
                boardinfo[i] = counter;
                break;
        }
    }
}

int getPieceWidth() {
    int piecewidth = -1, widthcounter = -1;
    for (int i = 0; i < 4; i++) {
        if (widthcounter > piecewidth)
            piecewidth = widthcounter;
        widthcounter = -1;
        for (int j = 0; j < 4; j++) {
            if (pieces[piece_shape][piece_variant][i][j])
                widthcounter++;
            else
                break;
        }
    }
    return piecewidth;
}

void getPieceInfo() {
    int pieceheight = -1;
    for (int i = 0; i < 4; i++) {
        pieceheight = -1;
        for (int j = 0; j < 4; j++) {
            if (pieces[piece_shape][piece_variant][j][i])
                pieceheight = j;
        }
        pieceinfo[i] = pieceheight;
    }
}

/*void getDistanceToDropInfo() {
    for (int i = 0; i < 4; i++) {
        distancetodropinfo[i] =
    }
}*/

void testPrintBoard() {
    printf("=====PRINTBOARD TEST=====");
    for (int i = 0; i < ROWS; i++) {
        printf("\n");
        for (int j = 0; j < COLUMNS; j++)
            printf("%d\t", board[i][j]);
    }
    printf("=====PRINTBOARD TEST=====\n");
}

void testPrintCurrentPiece() {
    printf("=====CURRENT PIECE TEST=====\n");
    printf("Current piece shape: %d\nCurrent piece variant (rotation): %d\nCurrent piece width: %d\nCurrent piece position: %d\n",
            piece_shape, piece_variant, getPieceWidth(), piece_position);
    printf("=====CURRENT PIECE TEST=====\n");
}

void testPrintBoardInfo() {
    printf("=====BOARDINFO TEST=====\n");
    for (int i = 0; i < COLUMNS; i++)
        printf("%d\t", boardinfo[i]);
    printf("=====BOARDINFO TEST=====\n");
}

void testPrintPieceInfo() {
    printf("=====PIECEINFO TEST=====\n");
    for (int i = 0; i < 4; i++)
        printf("%d\t", pieceinfo[i]);
    printf("\n=====PIECEINFO TEST=====\n");
}
