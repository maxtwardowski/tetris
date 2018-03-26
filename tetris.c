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
#define THRESHOLD 1600

#define DOWN 1
#define LEFT 2
#define RIGHT 3

//the main structures of the game  newpieceonscreen
int board[ROWS][COLUMNS],
    pieceboard[ROWS][COLUMNS],
    boardinfo[COLUMNS],
    piece_shape,
    piece_variant,
    piece_position,
    piece_color,
    gameregulator;
bool newpieceonscreen = false,
     piece_collision = false;

void cleanScreen();
void keyDetect();
void drawBoard();
int randomInt(int range_start, int range_end);
void pickNewPiece();
void movePiece(int direction);
void drawBlocks(int array[ROWS][COLUMNS]);
int getPieceWidth();
void embedBlock();
void checkWin();
void fallingPieces();

int main(int argc, char* argv[]) {

    if(initGraph()) {
        exit(3);
    }

    while(true) {
        pickNewPiece();
        cleanScreen();
        drawBoard();
        drawBlocks(board);
        drawBlocks(pieceboard);
        keyDetect();
        updateScreen();
        checkWin();
        fallingPieces();
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
            movePiece(LEFT);
        }
    } else if (key == SDLK_RIGHT) {
        if (piece_position + getPieceWidth() < COLUMNS - 1) {
            piece_position++;
            movePiece(RIGHT);
        }
    } else if (key == SDLK_RETURN) {
        movePiece(DOWN);
    } else if (key == SDLK_ESCAPE)
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

void pickNewPiece() {
    if (newpieceonscreen == false) {
        piece_shape = randomInt(0, 6);
        piece_color = randomInt(1, 7);
        piece_variant = 0;
        piece_position = 0;
        piece_collision = false;
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

void movePiece(int direction) {
    if (direction == DOWN) {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLUMNS; j++) {
                if ((pieceboard[i][j] && board[i - 1][j]) || (pieceboard[i][j] && i == 0)) {
                    piece_collision = true;

                    /*if (board[i - 1][j] || board[i][j + 1] || i == 0) {
                        i = ROWS + 1;
                        j = COLUMNS + 1;
                        embedBlock();
                        newpieceonscreen = false;
                    } else {
                        pieceboard[i - 1][j] = pieceboard[i][j];
                        pieceboard[i][j] = 0;
                    }*/
                }
            }
        }
        if (piece_collision == false) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLUMNS; j++) {
                    pieceboard[i - 1][j] = pieceboard[i][j];
                    pieceboard[i][j] = 0;
                }
            }
        } else if (piece_collision == true) {
            embedBlock();
            newpieceonscreen = false;
        }
    } else if (direction == LEFT) {
        for (int i = 0; i < COLUMNS; i++) {
            for (int j = 0; j < ROWS; j++) {
                if (pieceboard[j][i]) {
                    pieceboard[j][i - 1] = pieceboard[j][i];
                    pieceboard[j][i] = 0;
                }
            }
        }
    } else if (direction == RIGHT) {
        for (int i = COLUMNS - 1; i >= 0; i--) {
            for (int j = ROWS - 1; j >= 0; j--) {
                if (pieceboard[j][i]) {
                    pieceboard[j][i + 1] = pieceboard[j][i];
                    pieceboard[j][i] = 0;
                }
            }
        }
    }
}

void drawBlocks(int array[ROWS][COLUMNS]) {
    const int OFFSET = 10, BLOCKWIDTH = (screenHeight() - 2 * OFFSET) / ROWS;
    int x_base = (screenWidth() - BLOCKWIDTH * COLUMNS) / 2,
        y_base = screenHeight() - OFFSET,
        x_cord,
        y_cord = y_base;

    for (int i = 0; i < ROWS; i++) {
        x_cord = x_base;
        for (int j = 0; j < COLUMNS; j++) {
            if (array[i][j]) {
                filledRect(x_cord,
                           y_cord,
                           x_cord + BLOCKWIDTH,
                           y_cord - BLOCKWIDTH,
                           array[i][j]);
            }
            x_cord += BLOCKWIDTH;
        }
        y_cord -= BLOCKWIDTH;
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

void embedBlock() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (pieceboard[i][j]) {
                board[i][j] = pieceboard[i][j];
                pieceboard[i][j] = 0;
            }
        }
    }
}

void checkWin() {
    for (int i = 0; i < COLUMNS; i++) {
        if (board[ROWS - 1][i] != 0) {
            textout(200, 200, "GAME OVER! TRY AGAIN.", RED);
            updateScreen();
            SDL_Delay(2000);
            exit(1);
        }
    }
}

void fallingPieces() {
    if (gameregulator == THRESHOLD) {
        movePiece(DOWN);
        gameregulator = 0;
    }
    gameregulator++;
}
