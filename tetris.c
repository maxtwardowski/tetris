#include "primlib.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "tetris.h"

#define ROWS 20
#define COLUMNS 10
#define TICKRATE 20

#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define PIVOT 2

#define BLOCKCOLOR YELLOW
#define PIVOTCOLOR RED
#define BACKGROUNDCOLOR BLACK
#define BOARDCOLOR WHITE

//the main structures of the game
int board[ROWS][COLUMNS],
    pieceboard[ROWS][COLUMNS],
    boardinfo[COLUMNS],
    piece_shape,
    piece_variant,
    piece_color,
    gameregulator;
bool newpieceonscreen = false,
     piece_collision = false,
     piece_collision_left = false,
     piece_collision_right = false;

void cleanScreen();
void keyDetect();
void drawBoard();
int randomInt(int range_start, int range_end);
void pickNewPiece();
void movePiece(int direction);
void drawBlocks(int array[ROWS][COLUMNS]);
void embedBlock();
void checkGameOver();
void fallingPieces();
void dropThePiece();
void checkCleanRow();
void CleanRow(int height);
void checkHorizontalCollision();
void rotatePiece();

int main(int argc, char* argv[]) {

    if(initGraph()) {
        exit(3);
    }

    srand(time(NULL));

    while(true) {
        checkHorizontalCollision();
        pickNewPiece();
        cleanScreen();
        drawBoard();
        drawBlocks(board);
        drawBlocks(pieceboard);
        keyDetect();
        updateScreen();
        checkCleanRow();
        checkGameOver();
        fallingPieces();
        SDL_Delay(1000 / 60);
    }
    return 0;
}

void cleanScreen() {
	filledRect(0, 0, screenWidth(), screenHeight(), BACKGROUNDCOLOR);
}

void keyDetect() {
    int key = pollkey();
    if (key == SDLK_UP) {
        if (piece_variant == 0) {
            piece_variant = PIECELENGTH - 1;
        } else {
            piece_variant--;
        }
        rotatePiece();
    } else if (key == SDLK_DOWN) {
        if (piece_variant == PIECELENGTH - 1) {
            piece_variant = 0;
        } else {
            piece_variant++;
        }
        rotatePiece();
    } else if (key == SDLK_LEFT) {
        movePiece(LEFT);
    } else if (key == SDLK_RIGHT) {
        movePiece(RIGHT);
    } else if (key == SDLK_RETURN) {
        dropThePiece();
    } else if (key == SDLK_ESCAPE)
        exit(1);
}

void drawBoard() {
    const int OFFSET = 10, DISPLACEMENT = 2;
    int y1cord = OFFSET,
        y2cord = screenHeight() - OFFSET,
        blockwidth = (y2cord - y1cord) / ROWS,
        x1cord = (screenWidth() - blockwidth * COLUMNS) / 2,
        x2cord = screenWidth() - x1cord;
    line(x1cord - DISPLACEMENT, y1cord - DISPLACEMENT, x2cord + DISPLACEMENT, y1cord - DISPLACEMENT, BOARDCOLOR); //TOP
    line(x1cord - DISPLACEMENT, y2cord + DISPLACEMENT, x2cord + DISPLACEMENT, y2cord + DISPLACEMENT, BOARDCOLOR); //BOTTOM
    line(x1cord - DISPLACEMENT, y1cord - DISPLACEMENT, x1cord - DISPLACEMENT, y2cord + DISPLACEMENT, BOARDCOLOR); //LEFT
    line(x2cord + DISPLACEMENT, y1cord - DISPLACEMENT, x2cord + DISPLACEMENT, y2cord + DISPLACEMENT, BOARDCOLOR); //RIGHT
}

int randomInt(int range_start, int range_end) {
    int random_int = rand() % (range_end - range_start + 1) + range_start; //+1 to avoid black
    return random_int;
}

void pickNewPiece() {
    int const spawning_position = COLUMNS / 2 - 1,
              rangestart = 0,
              rangeend = 6;
    if (newpieceonscreen == false) {
        piece_shape = randomInt(rangestart, rangeend);
        piece_color = BLOCKCOLOR;
        piece_variant = 0;
        piece_collision = false;
        piece_collision_left = false;
        piece_collision_right = false;
        for (int i = 0; i < PIECELENGTH; i++) {
            for (int j = 0; j < PIECELENGTH; j++) {
                if (pieces[piece_shape][piece_variant][i][j] == 1)
                    pieceboard[ROWS - i - 1][j + spawning_position] = piece_color;
                else if (pieces[piece_shape][piece_variant][i][j] == 2)
                    pieceboard[ROWS - i - 1][j + spawning_position] = PIVOTCOLOR;
            }
        }
        newpieceonscreen = true;
    }
}

void movePiece(int direction) {
    if (direction == DOWN) {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLUMNS; j++) {
                if ((i && pieceboard[i][j] && board[i - 1][j]) || (pieceboard[i][j] && i == 0)) {
                    piece_collision = true;
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
    } else if (direction == LEFT && !piece_collision_left) {
        for (int i = 0; i < COLUMNS; i++) {
            for (int j = 0; j < ROWS; j++) {
                if (pieceboard[j][i]) {
                    pieceboard[j][i - 1] = pieceboard[j][i];
                    pieceboard[j][i] = 0;
                }
            }
        }
    } else if (direction == RIGHT && !piece_collision_right) {
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

void embedBlock() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (pieceboard[i][j]) {
                board[i][j] = piece_color;
                pieceboard[i][j] = 0;
            }
        }
    }
}

void checkGameOver() {
    const int delay = 2000,
              xposition = 250,
              yposition = 200;
    for (int i = 0; i < COLUMNS; i++) {
        if (board[ROWS - 1][i] != 0) {
            textout(xposition, yposition, "GAME OVER! TRY AGAIN.", RED);
            updateScreen();
            SDL_Delay(delay);
            exit(1);
        }
    }
}

void fallingPieces() {
    if (gameregulator == TICKRATE) {
        movePiece(DOWN);
        gameregulator = 0;
    }
    gameregulator++;
}

void dropThePiece() {
    while (piece_collision == false) {
        movePiece(DOWN);
    }
}

void checkCleanRow() {
    const int delay = 300;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (!board[i][j])
                break;
            if (j == COLUMNS - 1 && board[i][j]) {
                cleanScreen();
                drawBoard();
                drawBlocks(board);
                drawBlocks(pieceboard);
                updateScreen();
                SDL_Delay(delay);
                CleanRow(i);
            }
        }
    }
}

void CleanRow(int height) {
    for (int i = height; i < ROWS - 1; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            board[i][j] = board[i + 1][j];
        }
    }
}

void checkHorizontalCollision() {
    //checking left side
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (pieceboard[i][j] && j && board[i][j - 1]) {
                piece_collision_left = true;
                i = ROWS;
                j = COLUMNS;
            } else if (pieceboard[i][j] && j == 0) {
                piece_collision_left = true;
                i = ROWS;
                j = COLUMNS;
            }
            else if (pieceboard[i][j] && j && !board[i][j - 1])
                piece_collision_left = false;
        }
    }
    //checking right side
    for (int i = 0; i < ROWS; i++) {
        for (int j = COLUMNS - 1; j >= 0; j--) {
            if (pieceboard[i][j] && j < COLUMNS && board[i][j + 1]) {
                piece_collision_right = true;
                i = ROWS;
                j = -1;
            } else if (pieceboard[i][j] && j == COLUMNS - 1) {
                piece_collision_right = true;
                i = ROWS;
                j = -1;
            }
            else if (pieceboard[i][j] && j < COLUMNS && !board[i][j + 1])
                piece_collision_right = false;
        }
    }
}

void rotatePiece() {
    //locating the pivot before the rotation
    int ycord, xcord;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (pieceboard[i][j] == PIVOTCOLOR) {
                ycord = i;
                xcord = j;
                i = ROWS;
                j = COLUMNS;
            }
        }
    }

    //locating the new pivot in the already rotated piece
    int centerpieceX, centerpieceY;
    for (int k = 0; k < PIECELENGTH; k++) {
        for (int l = 0; l < PIECELENGTH; l++) {
            if (pieces[piece_shape][piece_variant][k][l] == PIVOT) {
                centerpieceY = k;
                centerpieceX = l;
                k = PIECELENGTH;
                l = PIECELENGTH;
            }
        }
    }

    //cleaning the pieceboard for the rotated piece
    for (int k = 0; k < ROWS; k++) {
        for (int l = 0; l < COLUMNS; l++) {
            pieceboard[k][l] = 0;
        }
    }

    //Preventing bugs if the rotated figure is wider than the original
    if (!xcord && centerpieceX) {
        for (int i = 0; i < centerpieceX; i++)
            xcord++;
    } else if (xcord == COLUMNS - 1 && centerpieceX) {
        for (int i = 0; i < centerpieceX; i++)
            xcord--;
    }

    pieceboard[ycord][xcord] = PIVOTCOLOR; //making the pivot stay in the pieceboard

    //Bottom-right
    for (int i = 0; i < PIECELENGTH - centerpieceY; i++) {
        for (int j = 0; j < PIECELENGTH - centerpieceX; j++) {
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY + i][centerpieceX + j] && !pieceboard[ycord - i][xcord + j])
                    pieceboard[ycord - i][xcord + j] = piece_color;
        }
    }
    //Bottom-left
    for (int i = 0; i < PIECELENGTH - centerpieceY; i++) {
        for (int j = 0; j < centerpieceX + 1; j++) {
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY + i][centerpieceX - j] && !pieceboard[ycord - i][xcord - j])
                pieceboard[ycord - i][xcord - j] = piece_color;
        }
    }
    //Top-right
    for (int i = 0; i < centerpieceY + 1; i++) {
        for (int j = 0; j < PIECELENGTH - centerpieceX; j++) {
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY - i][centerpieceX + j] && !pieceboard[ycord + i][xcord + j])
                pieceboard[ycord + i][xcord + j] = piece_color;
        }
    }
    //Top-left
    for (int i = 0; i < centerpieceY + 1; i++) {
        for (int j = 0; j < centerpieceX + 1; j++) {
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY - i][centerpieceX - j] && !pieceboard[ycord + i][xcord - j])
                pieceboard[ycord + i][xcord - j] = piece_color;
        }
    }
}
