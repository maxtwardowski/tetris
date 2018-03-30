//DO OGARNIĘCIA
//obracanie
//kolizje boczne

#include "primlib.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "tetris.h"

#define ROWS 20
#define COLUMNS 10
#define THRESHOLD 1600

#define DOWN 1
#define LEFT 2
#define RIGHT 3

//the main structures of the game
int board[ROWS][COLUMNS],
    pieceboard[ROWS][COLUMNS],
    boardinfo[COLUMNS],
    piece_shape,
    piece_variant,
    piece_position,
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
int getPieceWidth();
void embedBlock();
void checkGameOver();
void fallingPieces();
void dropThePiece();
void checkCleanRow();
void CleanRow(int height);
void checkHorizontalCollision();
void rotatePiece();
void animateDrop();
void printboard();

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
        rotatePiece();
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
        rotatePiece();
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
    line(x1cord - DISPLACEMENT, y1cord - DISPLACEMENT, x2cord + DISPLACEMENT, y1cord - DISPLACEMENT, WHITE); //TOP
    line(x1cord - DISPLACEMENT, y2cord + DISPLACEMENT, x2cord + DISPLACEMENT, y2cord + DISPLACEMENT, WHITE); //BOTTOM
    line(x1cord - DISPLACEMENT, y1cord - DISPLACEMENT, x1cord - DISPLACEMENT, y2cord + DISPLACEMENT, WHITE); //LEFT
    line(x2cord + DISPLACEMENT, y1cord - DISPLACEMENT, x2cord + DISPLACEMENT, y2cord + DISPLACEMENT, WHITE); //RIGHT
}

int randomInt(int range_start, int range_end) {
    int random_int = rand() % (range_end - range_start + 1) + range_start; //+1 to avoid black
    return random_int;
}

void pickNewPiece() {
    int const spawning_position = COLUMNS / 2 - 1;
    if (newpieceonscreen == false) {
        piece_shape = randomInt(0, 6);
        piece_color = YELLOW;
        piece_variant = 0;
        piece_position = spawning_position;
        piece_collision = false;
        piece_collision_left = false;
        piece_collision_right = false;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (pieces[piece_shape][piece_variant][i][j] == 1)
                    pieceboard[ROWS - i - 1][j + spawning_position] = piece_color;
                else if (pieces[piece_shape][piece_variant][i][j] == 2)
                    pieceboard[ROWS - i - 1][j + spawning_position] = RED;
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

void checkGameOver() {
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

void dropThePiece() {
    while (piece_collision == false) {
        movePiece(DOWN);
    }
}

void checkCleanRow() {
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
                SDL_Delay(400);
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
            } else if (pieceboard[i][j] && j && !board[i][j - 1])
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
            } else if (pieceboard[i][j] && j < COLUMNS && !board[i][j + 1])
                piece_collision_right = false;
        }
    }
}

void rotatePiece() {
    int ycord, xcord;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            if (pieceboard[i][j] == RED) {
                ycord = i;
                xcord = j;
                i = ROWS;
                j = COLUMNS;
            }
        }
    }

    //cleaning the pieceboard for the rotated piece
    for (int k = 0; k < ROWS; k++) {
        for (int l = 0; l < COLUMNS; l++) {
            pieceboard[k][l] = 0;
        }
    }

    int centerpieceX, centerpieceY;

    for (int k = 0; k < 4; k++) {
        for (int l = 0; l < 4; l++) {
            if (pieces[piece_shape][piece_variant][k][l] == 2) {
                centerpieceY = k;
                centerpieceX = l;
                k = 4;
                l = 4;
            }
        }
    }

    pieceboard[ycord][xcord] = RED;

    //rightbot
    for (int i = 0; i < 4 - centerpieceY; i++) {
        for (int j = 0; j < 4 - centerpieceX; j++) {
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY + i][centerpieceX + j] && !pieceboard[ycord - i][xcord + j])
                    pieceboard[ycord - i][xcord + j] = piece_color;
        }
    }

    //leftbot
    for (int i = 0; i < 4 - centerpieceY; i++) {
        for (int j = 0; j < centerpieceX + 1; j++) {
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY + i][centerpieceX - j] && !pieceboard[ycord - i][xcord - j])
                pieceboard[ycord - i][xcord - j] = piece_color;
        }
    }

    //righttop
    for (int i = 0; i < centerpieceY + 1; i++) {
        for (int j = 0; j < 4 - centerpieceX; j++) {
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY - i][centerpieceX + j] && !pieceboard[ycord + i][xcord + j])
                pieceboard[ycord + i][xcord + j] = piece_color;
        }
    }

    //lefttop
    for (int i = 0; i < centerpieceY + 1; i++) {
        for (int j = 0; j < centerpieceX + 1; j++) {
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY - i][centerpieceX - j] && !pieceboard[ycord + i][xcord - j])
                pieceboard[ycord + i][xcord - j] = piece_color;
        }
    }

    printf("%d, %d\n", piece_shape, piece_variant);
    printboard();
    //SDL_Delay(10000000);
    piece_position = xcord;
}

void printboard() {
    printf("==============\n");
    for (int i = ROWS - 1; i >= 0; i--) {
        printf("\n");
        for (int j = 0; j < COLUMNS; j++) {
            if (!pieceboard[i][j])
                printf("-\t");
            else
                printf("%d\t", pieceboard[i][j]);
        }
    }
    printf("\n==============\n");
}
