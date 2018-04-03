#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "tetris.h"
#include "primlib.h"

#define ROWS 20
#define COLUMNS 10
#define TICKRATE 20
#define FPSRATE 60

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
     piece_collision_right = false,
     rotation_possible = true;

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
void checkVerticalCollision();
int getPieceWidth(int direction);
void ExecuteKeyUP();
void ExecuteKeyDOWN();

int main(int argc, char* argv[]) {
    if(initGraph())
        exit(3);
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
        SDL_Delay(1000 / FPSRATE);
    }
    return 0;
}

void cleanScreen() {
    //Draws background
	filledRect(0, 0, screenWidth(), screenHeight(), BACKGROUNDCOLOR);
}

void keyDetect() {
    //Handles keyinput
    int key = pollkey();
    checkVerticalCollision();
    checkHorizontalCollision();
    if (key == SDLK_UP && !piece_collision)
        ExecuteKeyUP();
    else if (key == SDLK_DOWN && !piece_collision)
        ExecuteKeyDOWN();
    else if (key == SDLK_LEFT)
        movePiece(LEFT);
    else if (key == SDLK_RIGHT)
        movePiece(RIGHT);
    else if (key == SDLK_RETURN)
        dropThePiece();
    else if (key == SDLK_ESCAPE)
        exit(1);
}

void drawBoard() {
    //Draws the 'board' i.e. edges of the actual game area
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
    //Generated random integer from the [range_start; range_end] interval
    int random_int = rand() % (range_end - range_start + 1) + range_start; //+1 to avoid black
    return random_int;
}

void pickNewPiece() {
    //Picks a new piece if the old one got embedded
    int const SPAWNING_POSITION = COLUMNS / 2 - 1,
              RANGESTART = 0,
              RANGEEND = 6;
    if (newpieceonscreen == false) {
        piece_shape = randomInt(RANGESTART, RANGEEND);
        piece_color = BLOCKCOLOR;
        piece_variant = 0;
        piece_collision = false;
        piece_collision_left = false;
        piece_collision_right = false;
        for (int i = 0; i < PIECELENGTH; i++) {
            for (int j = 0; j < PIECELENGTH; j++) {
                if (pieces[piece_shape][piece_variant][i][j] == 1)
                    pieceboard[ROWS - i - 1][j + SPAWNING_POSITION] = piece_color;
                else if (pieces[piece_shape][piece_variant][i][j] == 2)
                    pieceboard[ROWS - i - 1][j + SPAWNING_POSITION] = PIVOTCOLOR;
            }
        }
        newpieceonscreen = true;
    }
}

void movePiece(int direction) {
    //Moves piece according to the chosen direction
    if (direction == DOWN) {
        checkVerticalCollision();
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
    //Draws graphical representation of an array filled with 'blocks'
    const int OFFSET = 10,
              BLOCKWIDTH = (screenHeight() - 2 * OFFSET) / ROWS,
              ADDITIONALSPACE = 2;
    int x_base = (screenWidth() - BLOCKWIDTH * COLUMNS) / 2,
        y_base = screenHeight() - OFFSET,
        x_cord,
        y_cord = y_base;
    for (int i = 0; i < ROWS; i++) {
        x_cord = x_base;
        for (int j = 0; j < COLUMNS; j++) {
            if (array[i][j]) {
                filledRect(x_cord + ADDITIONALSPACE,
                           y_cord - ADDITIONALSPACE,
                           x_cord + BLOCKWIDTH - ADDITIONALSPACE,
                           y_cord - BLOCKWIDTH + ADDITIONALSPACE,
                           array[i][j]);
            }
            x_cord += BLOCKWIDTH;
        }
        y_cord -= BLOCKWIDTH;
    }
}

void embedBlock() {
    //'Embeds' piece into the main board
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
    //Checks if it's not already over ;)
    const int DELAY = 2000,
              XPOSITION = 250,
              YPOSITION = 200;
    for (int i = 0; i < COLUMNS; i++) {
        if (board[ROWS - 1][i] != 0) {
            textout(XPOSITION, YPOSITION, "GAME OVER! TRY AGAIN.", RED);
            updateScreen();
            SDL_Delay(DELAY);
            exit(1);
        }
    }
}

void fallingPieces() {
    //The actual falling animation
    if (gameregulator == TICKRATE) {
        movePiece(DOWN);
        gameregulator = 0;
    }
    gameregulator++;
}

void dropThePiece() {
    //Drops the piece as low as possible
    while (piece_collision == false)
        movePiece(DOWN);
}

void checkCleanRow() {
    //Checks if there is a row to get rid of
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
    //Removes a row if it's full of blocks
    for (int i = height; i < ROWS - 1; i++)
        for (int j = 0; j < COLUMNS; j++)
            board[i][j] = board[i + 1][j];
}

void checkHorizontalCollision() {
    //Checks if collisions in horizontal axis occur
    //LEFT
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
    //RIGHT
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
    //Creates an illusion of rotating piece
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
    for (int k = 0; k < ROWS; k++)
        for (int l = 0; l < COLUMNS; l++)
            pieceboard[k][l] = 0;
    pieceboard[ycord][xcord] = PIVOTCOLOR; //making the pivot stay in the pieceboard
    //FOUR STAGE REDRAWING PROCESS
    //Bottom-right
    for (int i = 0; i < PIECELENGTH - centerpieceY; i++)
        for (int j = 0; j < PIECELENGTH - centerpieceX; j++)
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY + i][centerpieceX + j] && !pieceboard[ycord - i][xcord + j])
                    pieceboard[ycord - i][xcord + j] = piece_color;
    //Bottom-left
    for (int i = 0; i < PIECELENGTH - centerpieceY; i++)
        for (int j = 0; j < centerpieceX + 1; j++)
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY + i][centerpieceX - j] && !pieceboard[ycord - i][xcord - j])
                pieceboard[ycord - i][xcord - j] = piece_color;
    //Top-right
    for (int i = 0; i < centerpieceY + 1; i++)
        for (int j = 0; j < PIECELENGTH - centerpieceX; j++)
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY - i][centerpieceX + j] && !pieceboard[ycord + i][xcord + j])
                pieceboard[ycord + i][xcord + j] = piece_color;
    //Top-left
    for (int i = 0; i < centerpieceY + 1; i++)
        for (int j = 0; j < centerpieceX + 1; j++)
            if ((i != 0 || j != 0) && pieces[piece_shape][piece_variant][centerpieceY - i][centerpieceX - j] && !pieceboard[ycord + i][xcord - j])
                pieceboard[ycord + i][xcord - j] = piece_color;
}

void checkVerticalCollision() {
    //Checks if vertical collisions occur
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLUMNS; j++)
            if ((i && pieceboard[i][j] && board[i - 1][j]) || (pieceboard[i][j] && i == 0))
                piece_collision = true;
}

int getPieceWidth(int direction) {
    //Returns 'side-width' of a piece according to the direction given
    int piecewidth = 0;
    bool colchecked;
    if (direction == LEFT) {
        for (int j = 0; j < PIECELENGTH; j++) {
            colchecked = false;
            for (int i = 0; i < PIECELENGTH; i++) {
                if (!colchecked && pieces[piece_shape][piece_variant][i][j]) {
                    piecewidth++;
                    colchecked = true;
                }
                if (pieces[piece_shape][piece_variant][i][j] == 2) {
                    i = PIECELENGTH;
                    j = PIECELENGTH;
                }
            }
        }
    } else if (direction == RIGHT) {
        for (int j = PIECELENGTH - 1; j >= 0; j--) {
            colchecked = false;
            for (int i = PIECELENGTH - 1; i >= 0; i--) {
                if (!colchecked && pieces[piece_shape][piece_variant][i][j]) {
                    piecewidth++;
                    colchecked = true;
                }
                if (pieces[piece_shape][piece_variant][i][j] == 2) {
                    i = -1;
                    j = -1;
                }
            }
        }
    }
    return piecewidth - 1;
}

void ExecuteKeyUP() {
    //Executes SDLK_UP action triggered by keyinput
    int oldwidthleft = getPieceWidth(LEFT),
        oldwidthright = getPieceWidth(RIGHT),
        oldvariant;
    if (piece_variant == 0) {
        oldvariant = piece_variant;
        piece_variant = PIECELENGTH - 1;
    }
    else {
        oldvariant = piece_variant;
        piece_variant--;
    }
    if (piece_collision_left && (oldwidthleft >= getPieceWidth(LEFT)))
        rotatePiece();
    else if (piece_collision_right && (oldwidthright >= getPieceWidth(RIGHT)))
        rotatePiece();
    else if (!piece_collision_right && !piece_collision_left) {
        //locating the pivot
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
        if (!board[ycord][xcord - getPieceWidth(LEFT)] &&
            !board[ycord][xcord + getPieceWidth(RIGHT)] &&
            xcord + getPieceWidth(RIGHT) <= COLUMNS - 1 &&
            xcord - getPieceWidth(LEFT) >= 0)
            rotatePiece();
        else
            piece_variant = oldvariant;
    } else
        piece_variant = oldvariant;
}

void ExecuteKeyDOWN() {
    //Executes SDLK_DOWN action triggered by keyinput
    int oldwidthleft = getPieceWidth(LEFT),
        oldwidthright = getPieceWidth(RIGHT),
        oldvariant;
    if (piece_variant == PIECELENGTH - 1) {
        oldvariant = piece_variant;
        piece_variant = 0;
    } else {
        oldvariant = piece_variant;
        piece_variant++;
    }
    if (piece_collision_left && (oldwidthleft >= getPieceWidth(LEFT)))
        rotatePiece();
    else if (piece_collision_right && (oldwidthright >= getPieceWidth(RIGHT)))
        rotatePiece();
    else if (!piece_collision_right && !piece_collision_left) {
        //locating the pivot
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
        if (!board[ycord][xcord - getPieceWidth(LEFT)] &&
            !board[ycord][xcord + getPieceWidth(RIGHT)] &&
            xcord + getPieceWidth(RIGHT) <= COLUMNS - 1 &&
            xcord - getPieceWidth(LEFT) >= 0)
            rotatePiece();
        else
            piece_variant = oldvariant;
    }
    else
        piece_variant = oldvariant;
}
