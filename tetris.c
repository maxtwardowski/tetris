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
    pieceboard[ROWS][COLUMNS],
    boardinfo[COLUMNS],
    distancetodropinfo[4],
    piece_shape,
    piece_variant,
    piece_position,
    piece_color,
    pieceinfo[4],
    piece_cords[4][4][2];
bool newpieceonscreen = false;

void cleanScreen();
void keyDetect();
void drawBoard();
int randomInt(int range_start, int range_end);
void pickNewPiece();
int checkCollision(void);
void movePieceDown();
void movePieceLeft();
void movePieceRight();
void drawBlocks();
void getBoardInfo();
int getPieceWidth();
void getPieceInfo();
void getDistanceToDropInfo();
int getShortestDistanceToDrop();
int getShortestDistanceToDropIndex();
void dropPiece();
void testPrintBoard();
void testPrintCurrentPiece();
void testPrintBoardInfo();
void testPrintPieceInfo();
void testPrintDistanceToDropInfo();
void testPrintPieceCords();

int main(int argc, char* argv[]) {

    if(initGraph()) {
        exit(3);
    }

    while(1) {


        getBoardInfo();
        pickNewPiece();
        cleanScreen();
        drawBoard();
        drawBlocks();
        updateScreen();
        //testPrintCurrentPiece();
        //testPrintPieceInfo();
        //testPrintDistanceToDropInfo();
        //testPrintBoardInfo();
        //testPrintPieceCords();
        keyDetect();
    }

    return 0;
}

void cleanScreen() {
    //Cleans the screen drawing a black background
	filledRect(0, 0, screenWidth(), screenHeight(), BLACK);
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
            movePieceLeft();

        }
    } else if (key == SDLK_RIGHT) {
        if (piece_position + getPieceWidth() < COLUMNS - 1) {
            piece_position++;
            testPrintBoard();
            movePieceRight();
            testPrintBoard();
        }
    } else if (key == SDLK_RETURN) {
        //printf("\n%d\n", get);
        movePieceDown();
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
        piece_shape = randomInt(0, 6);
        piece_color = randomInt(1, 7);
        piece_variant = 0;
        getPieceInfo();
        getDistanceToDropInfo();

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (pieces[piece_shape][piece_variant][i][j]) {
                    pieceboard[ROWS - i - 1][j] = piece_color;
                }
            }
        }

        newpieceonscreen = true;
    }
}

int checkCollision(void)
{
    int i, j; /* counters */
    for (i = ROWS - 1; i >= 0; i--)
        for (j = COLUMNS - 1; j >= 0; j--)
            if (pieceboard[i][j])
                if (board[i - 1][j])
                    return 1;
    return 0;
}

void movePieceDown() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (pieceboard[i][j]) {
                pieceboard[i - 1][j] = pieceboard[i][j];
                pieceboard[i][j] = 0;
            }
        }
    }
}

void movePieceLeft() {
    for (int i = COLUMNS - 1; i >= 0; i--) {
        for (int j = 0; j < ROWS; j++) {
            if (pieceboard[j][i]) {
                pieceboard[j][i + 1] = pieceboard[j][i];
                pieceboard[j][i] = 0;
            }
        }
    }
}

void movePieceRight() {
    for (int i = COLUMNS - 1; i >= 0; i--) {
        for (int j = ROWS - 1; j >= 0; j--) {
            if (pieceboard[j][i]) {
                pieceboard[j][i + 1] = pieceboard[j][i];
                pieceboard[j][i] = 0;
            }
        }
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
            if (pieceboard[i][j]) {
                filledRect(x_cord,
                           y_cord,
                           x_cord + BLOCKWIDTH,
                           y_cord - BLOCKWIDTH,
                           pieceboard[i][j]);
                x_cord += BLOCKWIDTH;
            }
        }
        y_cord -= BLOCKWIDTH;
    }
}

void getBoardInfo() {
    int counter = -1;
    for (int i = 0; i < COLUMNS; i++) {
        counter = -1;
        for (int j = 0; j < ROWS; j++) {
            if (board[j][i])
                counter = j;
        }
        boardinfo[i] = counter;
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

void getDistanceToDropInfo() {
    for (int i = 0; i < 4; i++) {
        distancetodropinfo[i] = ROWS - 2 - pieceinfo[piece_position + i];
    }
}

int getShortestDistanceToDrop() {
    int shortestdistance = 100000;
    for (int i = 0; i < 4; i ++) {
        if (shortestdistance > distancetodropinfo[i])
            shortestdistance = distancetodropinfo[i];
    }
    return shortestdistance;
}

int getShortestDistanceToDropIndex() {
    int shortestdistance = 100000, shortestindex;
    for (int i = 0; i < 4; i++) {
        if (shortestdistance > distancetodropinfo[i])
            shortestindex = i;
    }
    return shortestindex;
}

void dropPiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (pieces[piece_shape][piece_variant][i][j]) {
                board[boardinfo[piece_position + j] + i + 1][piece_position + j] = piece_color;
            }
        }
    }


    newpieceonscreen = false;
}

void testPrintBoard() {
    printf("=====PRINTBOARD TEST=====");
    for (int i = ROWS - 1; i >= 0; i--) {
        printf("\n");
        for (int j = 0; j < COLUMNS; j++)
            printf("%d\t", pieceboard[i][j]);
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

void testPrintDistanceToDropInfo() {
    printf("=====DISTANCETODROP TEST=====\n");
    for (int i = 0; i < 4; i++) {
        printf("Distance to drop column #%d: %d\n", i, distancetodropinfo[i]);
    }
    printf("\n=====DISTANCETODROP TEST=====\n");
}

void testPrintPieceCords() {
    printf("\n=====PIECE CORDS=====\n");
    for (int i = 0; i < 4; i++) {
        printf("\n");
        for (int j = 0; j < 4; j++) {
            printf("| %d, %d |\t", piece_cords[i][j][0], piece_cords[i][j][1]);
        }
    }
    printf("\n=====PIECE CORDS=====\n");
}
