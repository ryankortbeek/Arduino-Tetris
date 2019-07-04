#ifndef __GAME_BOARD_H
#define __GAME_BOARD_H

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
#include "nums.h"

using namespace std;

class gameBoard {

public:

  struct space {
    /*
    xB and yB are the column and row location on the gameboard of the
    space where as xD and yD are the x and y cordinates of the top left corner
    of this space on the display
    */
    bool filled;
    int xB, yB, xD, yD, colour;
    // Constructors
    space(bool filled, int xB, int yB, int colour = colours::BLACK);
    space();
  };

  struct piece {
    // pID corresponds to which tetris block this is
    int pID;
    // Array of each space in this tetris block
    space squares[4];
    // Colour of this peice
    int colour;
    // Current rotation of piece: = 1 if original position, = 2 if rotated 90
    // degrees clockwise, = 3 if rotated 180 degrees, = 4 if rotated 90 degrees
    // counterclockwise
    int currentRotation;
    // Constructors
    piece(int pID, space s0, space s1, space s2, space s3, int colour,
      int currentRotation = 1);
    piece();

    void updatePiece(int shift, Adafruit_ILI9341 tft);
  };

  gameBoard();

  void printBoard(Adafruit_ILI9341 tft);

  // 10x24 Gameboard --> each 1x1 unit is a instance of the "space" class
  // Creates an array of "squares" representing the gameboard
  space board[10][24];

  // Moves made by the player, validate shift called inside
  // shift = 0 for left, 1 for right, 2 for down
  int shift(int shift, piece &currentPiece, Adafruit_ILI9341 tft);

  // Checks if the move is ok
  int validateShift(int shift, piece currentPiece);

  // Updates the gameBoard, assumes validate board was true
  void updateBoard(int shift, piece currentPiece);

  // Draw a single piece at its location, called from shift functions
  void drawPiece(piece p, Adafruit_ILI9341 tft);

  gameBoard::piece spawnPiece(int pID, Adafruit_ILI9341 tft);

  // Analyzes the countour of the board to determine what piece to give
  // next
  void contour(int (*checks)[8]);

  int niceTris(int (*insertPieces)[7][2], int (*checks)[8]);

  int bustHead(int (*insertPieces)[7][2], int (*checks)[8], int prevP);

  // Checks if lines given in array were filled
  // makes line a [2][4] array where [0][y] is a true false for wether not
  // this line is to be checked (initially), then checlines changes it
  // to 1 if that line is filled; [1][y] is the line number
  int checkLines(int (*lines)[2][4]);

  // Cuts out the lines which were filled and shifts all the above lines down
  // should check how many lines are to be erased, then erase all those lines
  // and fix all the board space variables, then re draw the blocks shifted
  // down
  void rePrintBoardAbove(int (*lines)[2][4], int redraw, Adafruit_ILI9341 tft);

  void displayBoard();

  /*
  Carries out the whole rotation process, maps the piece to rotate to a
  matrix depending on its pID and current rotation and then rotates this
  matrix (via rotate3 or rotate4) and changes the pieces location on the
  gameboard if successful

  Arguments:
    - &currentPiece: pass by reference of the piece we are rotating
    - tft: the display we are using (so we can update it from within rotation
    process)
  */
  void processRotation(piece &currentPiece, Adafruit_ILI9341 tft);

  /*
  Iterates through a matrix that consists of 0's and 1's, where the 1's
  are the representation of the current orientation of the piece we are
  rotating, through each row (left to right). Every time a 1 is encountered
  rot90Clockwise is called with the row and column location of that 1 and
  then executeRotation is called to verify that the new location of the
  corresponding square is valid. Deals with 3x3 matrix rotation

  Arguments:
    - matrix[4][4]: matrix that consists of 0's and 1's, where the 1's
    are the representation of the current orientation of the piece we are
    rotating
    - rowSize: row size of the matrix (always = 4 in this function)
    - colSize: column size of the matrix (always = 4 in this function)
    - &currentPiece: pass by reference of the piece we are rotating
    - tft: the display we are using (so we can update it from within rotation
    process)
    - (&newFilledX)[4]: pass by reference of an array which stores the new
    column locations of each cube/part of the piece we are rotating whose
    rotation is valid
    - (&newFilledY)[4]: pass by reference of an array which stores the new
    row locations of each cube/part of the piece we are rotating whose rotation
    is valid
    NOTE: the arrays newFilledX and newFilledY allow us to reset the filled
    property of spaces on the gameboard if a rotation fails on a part/cube of
    the piece we are rotating after one or more parts/cubes of the piece have
    already been successfully rotated

  Returns true or false with regards to whether the rotation is allowed or not
  */
  bool rotate4(int matrix[4][4], int rowSize, int colSize, piece &currentPiece,
    Adafruit_ILI9341 tft, int (&newFilledX)[4], int (&newFilledY)[4]);

  /*
  Iterates through a matrix that consists of 0's and 1's, where the 1's
  are the representation of the current orientation of the piece we are
  rotating, through each row (left to right). Every time a 1 is encountered
  rot90Clockwise is called with the row and column location of that 1 and
  then executeRotation is called to verify that the new location of the
  corresponding square is valid. Deals with 3x3 matrix rotation

  Arguments:
    - matrix[3][3]: matrix that consists of 0's and 1's, where the 1's
    are the representation of the current orientation of the piece we are
    rotating
    - rowSize: row size of the matrix (always = 3 in this function)
    - colSize: column size of the matrix (always = 3 in this function)
    - &currentPiece: pass by reference of the piece we are rotating
    - tft: the display we are using (so we can update it from within rotation
    process)
    - (&newFilledX)[4]: pass by reference of an array which stores the new
    column locations of each cube/part of the piece we are rotating whose
    rotation is valid
    - (&newFilledY)[4]: pass by reference of an array which stores the new
    row locations of each cube/part of the piece we are rotating whose rotation
    is valid
    NOTE: the arrays newFilledX and newFilledY allow us to reset the filled
    property of spaces on the gameboard if a rotation fails on a part/cube of
    the piece we are rotating after one or more parts/cubes of the piece have
    already been successfully rotated

  Returns true or false with regards to whether the rotation is allowed or not
  */
  bool rotate3(int matrix[3][3], int rowSize, int colSize, piece &currentPiece,
    Adafruit_ILI9341 tft, int (&newFilledX)[4], int (&newFilledY)[4]);

  /*
  Handles the 90 degree rotation (clockwise) of an m by n matrix that is all
  zeros except for one 1 that is in the initial row and column location,
  rowOfSquare and columnOfSquare. Transposes the matrix and then reverses
  each row (results in a matrix rotated 90 degrees). By reference, returns
  the final row and column location of the one 1 in the matrix

  Arguments:
    - m: row size of matrix we are rotating
    - n: column size of matrix we are rotating
    - rowOfSquare: row of the part/cube of the piece we are currently
    rotating (in the matrix representation of the piece)
    - columnOfSquare:
    - &finalRow: pass by reference of the final row of the part/cube of the
    piece we are currently rotating (in the matrix representation of the piece)
    after the rotation is performed
    - &finalColumn: pass by reference of final column of the part/cube of the
    piece we are currently rotating (in the matrix representation of the piece)
    after the rotation is performed
    NOTE: rowOfSquare and columnOfSquare allow us to create a matrix of size
    m x n filled with all 0's except for one 1 in with location
    (columnOfSquare, rowOfSquare) so that we can rotate this matrix and then
    find the finalRow and finalColumn location of the 1, allowing us to get the
    change in position on the gameboard for the part/cube of the piece we are
    rotating
  */
  void rot90Clockwise(int m, int n, int rowOfSquare, int columnOfSquare,
    int &finalRow, int &finalColumn);

  /*
  Finds the change in position of the part/cube of the piece we are currently
  rotating and then checks to see if its a valid rotation.

  Arguments:
    - initialRow: initial row of the part/cube of the piece we are currently
    rotating (in the matrix representation of the piece)
    - initialColumn: initial column of the part/cube of the piece we are
    currently rotating (in the matrix representation of the piece)
    - finalRow: final row of the part/cube of the piece we are currently
    rotating (in the matrix representation of the piece) after the rotation
    performed in rot90Clockwise
    - finalColumn: final column of the part/cube of the piece we are
    currently rotating (in the matrix representation of the piece) after the
    rotation performed in rot90Clockwise
    - &currentPiece: pass by reference of the piece we are rotating
    - i: current iteration of rotate3 or rotate4 which also corresponds to the
    part/cube of the piece we are in the process of rotating
    - tft: the display we are using (so we can update it from within rotation
    process)
    - xOrigB[4]: array containing the original column locations (on the
    gameboard) of each cube that makes up the tetris piece we are currently
    rotating
    - yOrigB[4]: array containing the original row locations (on the gameboard)
    of each cube that makes up the tetris piece we are currently rotating
    - (&newFilledX)[4]: pass by reference of an array which stores the new
    column locations of each cube/part of the piece we are rotating whose
    rotation is valid
    - (&newFilledY)[4]: pass by reference of an array which stores the new
    row locations of each cube/part of the piece we are rotating whose rotation
    is valid
    NOTE: the arrays newFilledX and newFilledY allow us to reset the filled
    property of spaces on the gameboard if a rotation fails on a part/cube of
    the piece we are rotating after one or more parts/cubes of the piece have
    already been successfully rotated

  Returns false if invalid, otherwise returns true and stores new location
  of the part of the piece that was rotated (and updates the corresponding
  squares member of currentPiece)
  */
  bool executeRotation(int initialRow, int initialColumn, int finalRow,
    int finalColumn, piece &currentPiece, int i, Adafruit_ILI9341 tft,
    int xOrigB[4], int yOrigB[4], int (&newFilledX)[4], int (&newFilledY)[4]);

  /*
  Based on the current iteration of rotate3 or rotate4 (squareToShift is called
  from both functions), finds the correct part/cube of the piece on the
  gameboard to rotate. I.e. for the first iteration it finds the cube (from the
  piece) with the smallest y and then x location on the gameboard, for the
  second iteration it finds the cube with the second smallest y and then x
  location and so on...
  NOTE: origin on the gameboard is the top left corner

  Arguments:
    - i: current iteration of rotate3 or rotate4 (determines which cube to look
    for)
    - currentPiece: copy of type class piece that corresponds to the tetris
    piece we are currently rotating
    - x[4]: array containing the original column locations (on the gameboard)
    of each cube that makes up the tetris piece we are currently rotating
    - y[4]: array containing the original row locations (on the gameboard) of
    each cube that makes up the tetris piece we are currently rotating

  Returns the index of the correct cube to shift in the array squares, which is
  a member of currentPiece. This is possible since the array squares is ordered
  exactly how x[4] and y[4] are ordered
  */
  int squareToShift(int i, piece currentPiece, int x[4], int y[4]);

  void drop(piece &p, Adafruit_ILI9341 tft);
};

#endif
