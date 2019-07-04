#include "gameBoard.h"

using namespace std;

/*
10 x 24 array of square structs representing the gameboard
the first 4 rows (y = 0 to y = 3) are not visible on the screen.
Those top 4 rows are used as a place to spawn the blocks
*/
gameBoard::gameBoard() {
  int count = 0;
  // Initializes the empty board array
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 24; j++) {
      this->board[i][j] = space(0, i, j);
      count++;
    }
  }
}

int gameBoard::checkLines(int (*lines)[2][4]) {
  bool filled;
  int redraw = 0;
  for (int i = 0; i < 4; i++) {
    filled = true;
    for (int j = 0; j < 10; j++) {
      if (this->board[j][(*lines)[1][i]].filled != 1) {
        (*lines)[0][i] = 0;
        //filled = false;
        break;
      }
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (((*lines)[1][i] == (*lines)[1][j]) && ((*lines)[0][j] == 1) &&
      ((*lines)[0][i] == 1) && (j != i)) {
        (*lines)[0][j] = 0;
      }
    }
  }
  for (int i = 0; i < 4; i++) {
    if ((*lines)[0][i]) {
      redraw++;
    }
  }

  return redraw;
}

void gameBoard::rePrintBoardAbove(int (*lines)[2][4], int redraw,
Adafruit_ILI9341 tft) {

  int ys[4];
  int ind;
  int l = 100;
  int lowestY = 0;
  bool done = false;
  for (int i = 0; i < 4; i++) {
    if ((*lines)[0][i]) {
      ys[i] = (*lines)[1][i];
    } else {
      // set value to 100 for that line so we dont check it
      ys[i] = 100;
    }
  }
  // find the lowest y level that has no blocks filled squares
  // we will only below this level to save time
  for (int i = 4; i < 24; i++) {
    for (int j = 0; j < 10; j++) {
      if (this->board[j][i].filled) {
        lowestY = i - 1;
        done = true;
        break;
      }
    }
    // quit when we've found any filled square
    if (done) {
      break;
    }
  }
  for (int i = 0; i < redraw; i++) {
    for (int j = 0; j < 4; j++) {
      if (ys[j] < l) {
        l = ys[j];
        ind = j;
      }
    }
    // set to 100 so we dont do that line again
    ys[ind] = 100;

    for (int i = l; i >= lowestY; i--) {
      for (int j = 0; j < 10; j++) {
        this->board[j][i] = space(this->board[j][i-1].filled, j, i,
        this->board[j][i-1].colour);
      }
    }
    // reset l so the next lowest line will be set to l
    // except dont reset it the last time so we dont mess up printing
    // all the squares
    if (i != (redraw-1)) {
      l = 100;
    }
    //this->displayBoard();
  }
  for (int i = 4; i <= l; i++) {
    for (int j = 0; j < 10; j++) {
      tft.fillRect(this->board[j][i].xD, this->board[j][i].yD, 21, 13,
      this->board[j][i].colour);
    }
  }
}
// Based on result of contour analysis, adds pieces to bag which are
// optimal for current board state, guarentees piece can be played.
// Running time: O(n^2) where n = number of potential pieces
int gameBoard::niceTris(int (*insertPieces)[7][2], int (*checks)[8]) {
  int count = 0;
  // resets the pieces that can go in the bag before we check again
  for (int i = 0; i < 7; i++) {
    (*insertPieces)[i][1] = 0;
  }
  if ((*checks)[0] == 1) {
    // inserts straight piece for 2 deep hole
    if ((*insertPieces)[0][1] != 1) {
      (*insertPieces)[0][1] = 1;
      count += 1;
    }
    if ((*checks)[5] == 1 ) {
      // inserts L piece for 2 deep hole with left edge being lower
      if ((*insertPieces)[2][1] != 1) {
        (*insertPieces)[2][1] = 1;
        count += 1;
      }
    } else if ((*checks)[6] == 1) {
      // insets backwards L piece for 2 deep hole with right edge being
      // lower
      if ((*insertPieces)[1][1] != 1) {
        (*insertPieces)[1][1] = 1;
        count += 1;
      }
    }
  }
  if ((*checks)[1] == 1) {
    // inserts straight piece for deep hole
    if ((*insertPieces)[0][1] != 1) {
      (*insertPieces)[0][1] = 1;
      count += 1;
    }
  }
  if ((*checks)[2] == 1) {
    // inserts square for 2 space flat stretch
    if ((*insertPieces)[3][1] != 1) {
      count += 1;
      (*insertPieces)[3][1] = 1;
    }
  }
  if (((*checks)[3] == 1) && ((*checks)[4] >= 3)) {
    // inserts T and Z pieces if we have flat space with many bumps
    if ((*insertPieces)[5][1] != 1) {
      count += 1;
      (*insertPieces)[5][1] = 1;
    }
    if ((*insertPieces)[4][1] != 1) {
      count += 1;
      (*insertPieces)[4][1] = 1;
    }
    if ((*insertPieces)[6][1] != 1) {
      count += 1;
      (*insertPieces)[6][1] = 1;
    }
  } else if (((*checks)[3] == 1) && ((*checks)[4] < 3)) {
    // inserts T and L pieces if we have flat space with fewer bumps
    (*insertPieces)[5][1] = 1;
    count += 1;
    if ((*insertPieces)[1][1] != 1) {
      (*insertPieces)[1][1] = 1;
      count += 1;
    }
    if ((*insertPieces)[2][1] != 1) {
      (*insertPieces)[2][1] = 1;
      count += 1;
    }
  }
  if ((*checks)[7] > 6) {
    // If there is a hole > 6 deep, guarentee a straight piece
    (*insertPieces)[0][1] = 1;
    count = 1;
    for (int i = 0; i < 7; i++) {
      if (i != 0) {
        (*insertPieces)[i][1] = 0;
      }
    }
  }
  return count;
}

// Based on result of the contour analysis, puts suboptimal pieces
// in the bag
// Running time: O(n^2) where n = number of potential pieces
int gameBoard::bustHead(int (*insertPieces)[7][2], int (*checks)[8],
int prevP) {

  int count = 7;
  // resets the pieces that can go in the bag before we check again
  for (int i = 0; i < 7; i++) {
    (*insertPieces)[i][1] = 1;
  }
  if ((*checks)[0] == 1) {
    // Removes L pieces if we have a 2 deep hole
    (*insertPieces)[1][1] = 0;
    (*insertPieces)[2][1] = 0;
    count -= 2;
  }
  if ((*checks)[1] == 1) {
    // Removes straight pieces if we have a >2 deep hole
    (*insertPieces)[0][1] = 0;
    count -= 1;
  }
  if (((*checks)[2] == 1) && ((*checks)[3] == 1)) {
    // Removes square blocks if we have a 2 block flat stretch
    (*insertPieces)[3][1] = 0;
    count -= 1;
  }
  if ((*checks)[3] == 1) {
    // Removes T blocks if we have a 3 block flat stretch
    (*insertPieces)[5][1] = 0;
    count -= 1;
  }
  if (((*checks)[2] == 0) && (checks[3] == 0)) {
    // Removes both Z blocks if we have a bumpy board
    (*insertPieces)[4][1] = 0;
    (*insertPieces)[6][1] = 0;
    count -= 2;
  }
  for (int i = 0; i < 7; i++) {
    if (((*insertPieces)[i][1] == 1) && ((i+1) == prevP)) {
      (*insertPieces)[i][1] = 0;
      count -= 1;
    }
  }
  return count;
}

/*
Description: Analyzes the contour of the board and sets an array of flags
Running time: O(n*m) where n = width of board and m = height of board
*/
void gameBoard::contour(int (*checks)[8]) {
  /*
  checks[0]: shallow hole (2 deep)
  checks[1]: deep hole (> 2 deep)
  checks[2]: semi flat space (2 blocks on same level)
  checks[3]: flat space (> 2 blocks on same level)
  checks[4]: number of bumps
  checks[5]: 2 deep hole left lower
  checks[6]: 2 deep hole right lower
  checks[7]: deepest hole
  */
  int cLvl, pLvl, heightDiff; cLvl = 0; pLvl = 0;
  bool flat = false;
  // Gets the initial previous Lvl and resets the checks array
  for (int i = 0; i < 8; i++) {
    (*checks)[i] = 0;
  }
  for (int j = 4; j < 24; j++) {
    if ((this->board[0][j].filled == 1) || (j == 23)) {
      pLvl = j;
      break;
    }
  }
  // Gets the height of each column and compares it to the previous one
  for (int i = 1; i < 10; i++) {
    for (int j = 4; j < 24; j++) {
      if ((this->board[i][j].filled == 1) || (j == 23)) {
        cLvl = j;
        break;
      }
    }
    // Do checks inbetween here
    heightDiff = abs(pLvl - cLvl);
    // checks for a 2 deep hole
    if (heightDiff == 2) {
      (*checks)[0] = 1;
      if (pLvl < cLvl) {
        (*checks)[5] = 1;
      } else if (pLvl > cLvl) {
        (*checks)[6] = 1;
      } else {
        // Do nothing
      }
    // checks for a 3 or greater depth hole
    } else if (heightDiff > 2) {
      (*checks)[1] = 1;
      if (heightDiff > (*checks)[7]) {
        (*checks)[7] = heightDiff;
      }
    }
    // checks for a flat space
    if (heightDiff == 0) {
      // checks for a 2 block flat space
      if (!flat) {
        (*checks)[2] = 1;
        flat = 1;
      // checks for a 3+ block flat space
      } else {
        (*checks)[3] = 1;
      }
    // resets the flat indicator if we dont have heightDiff of 0
    } else if (heightDiff == 1) {
      (*checks)[4] += 1;
    } else {
      flat = 0;
    }
    // Now set/reset cLvl and pLvl
    pLvl = cLvl;
    cLvl = 0;
  }
}

void gameBoard::printBoard(Adafruit_ILI9341 tft) {
  // Prints the empty board
  int x0, y0, x1, y1;
  x0 = 5; x1 = 5;
  y0 = 18; y1 = ard_consts::tft_height - 2;
  tft.drawLine(x0, y0, x1, y1, colours::WHITE);
  for (int i = 0; i < 10; i++) {
    x0 += 23;
    x1 = x0;
    tft.drawLine(x0, y0, x1, y1, colours::WHITE);
  }
  y0 = 18; y1 = 18;
  x0 = 5; x1 = ard_consts::tft_width - 5;
  tft.drawLine(x0, y0, x1, y1, colours::WHITE);
  for (int i = 0; i < 20; i++) {
    y0 += 15;
    y1 = y0;
    tft.drawLine(x0, y0, x1, y1, colours::WHITE);
  }
  tft.setCursor(0,0);
  tft.setTextColor(colours::WHITE);
  tft.setTextSize(2);
  tft.print("Score:");

}

int gameBoard::shift(int shift, piece &currentPiece, Adafruit_ILI9341 tft) {
  // 1 means can move, 0 for hitting side, -1 for hitting another
  // block or the bottom
  int ok = validateShift(shift, currentPiece);
  if (ok == 1) {
    // updates the board and piece then draws it
    updateBoard(shift, currentPiece);
    currentPiece.updatePiece(shift, tft);
    drawPiece(currentPiece, tft);
  }
  return ok;
}

void gameBoard::drop(piece &p, Adafruit_ILI9341 tft) {
  bool bottom = false;
  int shift = 0;
  for (int i = 0; i < 4; i++) {
    this->board[p.squares[i].xB]
    [p.squares[i].yB].filled = 0;
  }
  while (!bottom) {
    shift++;
    for (int i = 0; i < 4; i++) {
      if ((this->board[p.squares[i].xB]
      [p.squares[i].yB + shift].filled == 1) ||
      (p.squares[i].yB + shift > 23)) {
        bottom = true;
        shift -= 1;
        break;
      }
    }
  }
  if ((shift) != 0) {
    for (int i = 0; i < 4; i++) {
      this->board[p.squares[i].xB][p.squares[i].yB + shift].filled = 1;
      this->board[p.squares[i].xB][p.squares[i].yB + shift].colour =
      p.colour;
      if (p.squares[i].yB >= 4) {
        tft.fillRect(p.squares[i].xD, p.squares[i].yD, 21, 13, colours::BLACK);
      }
    }
    for (int i = 0; i < 4; i++) {
      if ((p.squares[i].yB + shift) >= 4) {
        tft.fillRect(this->board[p.squares[i].xB][p.squares[i].yB].xD,
          this->board[p.squares[i].xB][p.squares[i].yB + shift].yD, 21, 13,
          this->board[p.squares[i].xB][p.squares[i].yB + shift].colour);
        p.squares[i].yB += shift;
      }
    }
  } else {
    for (int i = 0; i < 4; i++) {
      this->board[p.squares[i].xB]
      [p.squares[i].yB].filled = 1;
    }
  }
}

// draws a piece at its location
// should probably move this into update piece
void gameBoard::drawPiece(piece p, Adafruit_ILI9341 tft) {
  for (int i = 0; i < 4; i++) {
    if (p.squares[i].yB >= 4) {
      tft.fillRect(p.squares[i].xD, p.squares[i].yD, 21, 13, p.colour);
    }
  }
}

gameBoard::piece gameBoard::spawnPiece(int pID, Adafruit_ILI9341 tft) {
  // spawns piece at 4,0 (non visible area of board)
  // different pID's determine which one spawns
  int x1, x2, x3, x4, y1, y2, y3, y4, colour;
  if (pID == 1) {
    x1 = x2 = x3 = x4 = 4;
    y1 = 0; y2 = 1; y3 = 2; y4 = 3;
    colour = colours::CYAN;
  } else if (pID == 2) {
    x1 = x2 = x3 = 5; x4 = 4;
    y1 = 0; y2 = 1; y3 = 2; y4 = 2;
    colour = colours::VIOLET;
  } else if (pID == 3) {
    x1 = x2 = x3 = 4; x4 = 5;
    y1 = 0; y2 = 1; y3 = 2; y4 = 2;
    colour = colours::ORANGE;
  } else if (pID == 4) {
    x1 = x2 = 3; x3 = x4 = 4;
    y2 = y4 = 0; y1 = y3 = 1;
    colour = colours::YELLOW;
  } else if (pID == 5) {
    x1 = 4; x2 = 4; x3 = 5; x4 = 5;
    y1 = 0; y2 = y3 = 1; y4 = 2;
    colour = colours::LIME;
  } else if (pID == 6) {
    x1 = 3; x2 = x3 = 4; x4 = 5;
    y1 = y2 = y4 = 1; y3 = 0;
    colour = colours::PINKK;
  } else if (pID == 7) {
    x1 = x2 = 4; x3 = x4 = 5;
    y1 = 2; y2 = y3 = 1; y4 = 0;
    colour = colours::RED;
  }
  // generates the piece
  piece p = piece(pID, this->board[x1][y1], this->board[x2][y2],
    this->board[x3][y3], this->board[x4][y4], colour);
  return p;
}

void gameBoard::piece::updatePiece(int shift, Adafruit_ILI9341 tft) {
  int xS = 0; int yS = 0;
  int oldX[4]; int oldY[4]; int newX[4]; int newY[4];
  if (shift == 0) {
    xS = -1; yS = 0;
  } else if (shift == 1) {
    xS = 1; yS = 0;
  } else { // shift = 2
    xS = 0; yS = 1;
  }
  for (int i = 0; i < 4; i++) {
    // stores all the old and new locations
    oldX[i] = this->squares[i].xB;
    oldY[i] = this->squares[i].yB;
    this->squares[i].xB += xS;
    this->squares[i].yB += yS;
    newX[i] = this->squares[i].xB;
    newY[i] = this->squares[i].yB;

    // sets the pieces new xD and yD values (location on display)
    this->squares[i].xD = (5 + ((this->squares[i].xB + 1)*1) +
    ((this->squares[i].xB)*22));

    this->squares[i].yD = (18 + (((this->squares[i].yB - 4) + 1)*1) +
    ((this->squares[i].yB - 4)*14));
  }

  bool fillIn = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      // checks if any of the old squares are still in the new one, in which
      // case we dont have to erase and redraw the whole block
      // NOTE: move draw piece in here and only draw the squares which
      // werent in the old place
      if ((oldX[i] == newX[j]) && (oldY[i] == newY[j])) {
        break;
      } else {
        fillIn = true;
      }
    }
    // blacks out the old squares
    if ((fillIn == true) && (oldY[i] >= 4)) {
      tft.fillRect( 5 + ((oldX[i] + 1)*1) + ((oldX[i])*22), 18 + (((oldY[i]
        - 4) + 1)*1) + (((oldY[i] - 4))*14), 21, 13, colours::BLACK);
    }
  }
}

// This functions assumes validate board returned true
void gameBoard::updateBoard(int shift, piece currentPiece) {
  int xS = 0;
  int yS = 0;
  if (shift == 0) {
    xS = -1; yS = 0;
  } else if (shift == 1) {
    xS = 1; yS = 0;
  } else { // shift = 2
    xS = 0; yS = 1;
  }

  for (int i = 0; i < 4; i++) {
    // sets old spots to be empty
    this->board[currentPiece.squares[i].xB]
    [currentPiece.squares[i].yB].filled = 0;
  }
  // loops through each space occupied by this piece
  for (int i = 0; i < 4; i++) {
    // sets the new spots to be filled
    this->board[currentPiece.squares[i].xB + xS]
    [currentPiece.squares[i].yB + yS].filled = 1;
  }
}

void gameBoard::displayBoard() {
  // for testing, prints board to serial mon
  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 10; j++) {
      Serial.print(this->board[j][i].filled); Serial.print(" ");
    }
    Serial.print("    ");
    for (int j = 0; j < 10; j++) {
      Serial.print(this->board[j][i].xB); Serial.print(" ");
      Serial.print(this->board[j][i].yB); Serial.print(" ");
      //Serial.print(this->board[j][i].xD); Serial.print(" ");
      //Serial.print(this->board[j][i].yD); Serial.println(" ");
      //Serial.print(this->board[j][i].colour); Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("");
}

int gameBoard::validateShift(int shift, piece currentPiece) {
  // checks if the shift destination is open or not
  // returns 1 for open, 0 for hitting a side edge, -1 for hitting another
  // block vertically or the floor
  int xS, yS;
  if (shift == 0) {
    xS = -1; yS = 0;
  } else if (shift == 1) {
    xS = 1; yS = 0;
  } else { // shift = 2
    xS = 0; yS = 1;
  }
  // set all the old spots to unfilled
  for (int i = 0; i < 4; i++) {
    this->board[currentPiece.squares[i].xB]
    [currentPiece.squares[i].yB].filled = 0;
  }

  for (int i = 0; i < 4; i++) {
    // only shift if the destination is in the board
    if ((currentPiece.squares[i].xB + xS < 0) ||
    (currentPiece.squares[i].xB + xS > 9) ||
    (currentPiece.squares[i].yB + yS < 0) ||
    (currentPiece.squares[i].yB + yS > 23)) {
      for (int i = 0; i < 4; i++) {
        // reset the old spots to filled as we didnt move
        this->board[currentPiece.squares[i].xB]
        [currentPiece.squares[i].yB].filled = 1;
      }
      if (yS != 0) {
        for (int i = 0; i < 4; i++) {
          this->board[currentPiece.squares[i].xB]
          [currentPiece.squares[i].yB].colour = currentPiece.colour;
        }
        return -1;
      } else {
        return 0;
      }
    }
    // only shift if destination also doesnt have other blocks at it
    if (this->board[currentPiece.squares[i].xB + xS]
    [currentPiece.squares[i].yB + yS].filled == 1) {
      for (int i = 0; i < 4; i++) {
        // reset the old spots to filled as we didnt move
        this->board[currentPiece.squares[i].xB]
        [currentPiece.squares[i].yB].filled = 1;
      }
      if (yS != 0) {
        for (int i = 0; i < 4; i++) {
          this->board[currentPiece.squares[i].xB]
          [currentPiece.squares[i].yB].colour = currentPiece.colour;
        }
        return -1;
      } else {
        return 0;
      }
    }
  }
  for (int i = 0; i < 4; i++) {
    // i forget what the point of this was
    this->board[currentPiece.squares[i].xB]
    [currentPiece.squares[i].yB].filled = 1;
  }
  return 1;
}

gameBoard::space::space(bool filled, int xB, int yB, int colour) {
  this->filled = filled;
  this->xB = xB;
  this->yB = yB;
  this->colour = colour;
  // Calculates the x and y location of the top left corner of
  // the black rectangle on the screen
  this->xD = 5 + ((xB + 1)*1) + (xB*22);
  this->yD = 18 + (((yB - 4) + 1)*1) + ((yB - 4)*14);
}

gameBoard::space::space() {}

gameBoard::piece::piece(int pID, space s0, space s1, space s2, space s3,
int colour, int currentRotation) {
  this->pID = pID;
  this->squares[0] = s0;
  this->squares[1] = s1;
  this->squares[2] = s2;
  this->squares[3] = s3;
  this->colour = colour;
  this->currentRotation = currentRotation;
}

gameBoard::piece::piece() {}

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
void gameBoard::rot90Clockwise(int m, int n, int rowOfSquare,
  int columnOfSquare, int &finalRow, int &finalColumn) {
  // Creates matrix to be rotated
  // Consists of all zeros except for one 1 in the location given by
  // rowOfSquare and columnOfSquare
  int transpose[m][n];
  for (int row = 0; row < m; row++) {
    int rowToTranspose[m] = {0};
    // Puts a 1 in the location given by rowOfSquare and columnOfSquare
    if (row == rowOfSquare) {
      for (int col = 0; col < n; col++) {
        if (col == columnOfSquare) {
          rowToTranspose[col] = 1;
        }
      }
    }
    // Transposes the matrix and reverses each row
    for (int j = 0; j < m; j++) {
      // Reverses each row of the transposed matrix
      transpose[j][(n-1)-row] = rowToTranspose[j];
    }
  }

  // Iterates through the transposed matrix until a 1 is encountered and then
  // assigns finalRow and finalColumn to the corresponding row and column where
  // the 1 is located
  bool flag = false;
  for (int row = 0; row < m; row++) {
    for (int col = 0; col < n; col++) {
      if (transpose[row][col] == 1) {
        finalRow = row;
        finalColumn = col;
        flag = true;
        break;
      }
    }
    if (flag) {
      break;
    }
  }
}

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
int gameBoard::squareToShift(int i, piece currentPiece, int x[4], int y[4]) {
  // Case 1 - for first iteration
  // Finds the cube from the piece currentPiece that is upmost and leftmost
  // on the gameboard and gets the index of it in the arrays x and y
  int min = y[0];
  int b0 = 0;
  // Finds the cube with the minimum row and in the case of a tie, finds the
  // leftmost cube
  for (int j = 0; j < 4; j++) {
    if (y[j] < min) {
      b0 = j;
      min = y[j];
    } else if (y[j] == min) {
      if (x[j] < x[b0]) {
        b0 = j;
        min = y[j];
      }
    }
  }

  // Case 4 - for fourth/last iteration
  // Finds the cube from the piece currentPiece that is downmost and
  // rightmost on the gameboard and gets the index of it in the array x and y
  int max = y[0];
  int b3 = 0;
  // Finds the cube with the largest row and in the case of a tie, finds the
  // rightmost cube
  for (int j = 0; j < 4; j++) {
    if (y[j] > max) {
      b3 = j;
      max = y[j];
    } else if (y[j] == max) {
      if (x[j] > x[b3]) {
        b3 = j;
        max = y[j];
      }
    }
  }

  // Case 2 - for second iteration
  // Finds the cube from the piece currentPiece that is second upmost and
  // leftmost on the gameboard and gets the index of it in the array x and y
  int b1 = b3;
  int secondMin = max;
  for (int j = 0; j < 4; j++) {
    if (j != b0 && j != b3) {
      if (y[j] < secondMin) {
        secondMin = y[j];
        b1 = j;
      } else if (y[j] == secondMin) {
        if (x[j] < x[b1]) {
          secondMin = y[j];
          b1 = j;
        }
      }
    }
  }

  // If iteration corresponds to case 1, returns the corresponding index (min)
  if (i == 0) {
    return b0;
  // If iteration corresponds to case 4, returns the corresponding index (max)
  } else if (i == 3) {
    return b3;
  // If iteration corresponds to case 2, returns the corresponding index
  // (second min)
  } else if (i == 1) {
    return b1;
  // Otherwise it must be the third iteration and therefore returns the index
  // that has not been found by any of the above cases - this will always
  // correspond to the second max value
  } else {
    for (int j = 0; j < 4; j++) {
      if (j != b0 && j != b1 && j != b3) {
        return j;
      }
    }
  }
}

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
bool gameBoard::executeRotation(int initialRow, int initialColumn,
int finalRow, int finalColumn, piece &currentPiece, int i,
Adafruit_ILI9341 tft, int xOrigB[4], int yOrigB[4], int (&newFilledX)[4],
int (&newFilledY)[4]) {
  // Gets the change in position of the cube from before rotation to after
  // rotation
  int changeRow = finalRow - initialRow;
  int changeColumn = finalColumn - initialColumn;
  // Finds correct square in piece to shift
  int s_to_shift = squareToShift(i, currentPiece, xOrigB, yOrigB);

  int m = currentPiece.squares[s_to_shift].xB;
  int n = currentPiece.squares[s_to_shift].yB;

  // Checks if the new position of the cube is in a valid location - does
  // nothing and returns false if invalid
  if ((m + changeColumn > 9) || (m + changeColumn < 0) || (n + changeRow > 23)
  || (n + changeRow < 0)) {
    return false;
  } else if (!board[m + changeColumn][n + changeRow].filled) {
    // If rotation is valid then sets the filled member of the space on the
    // gameboard where cube will be after rotating to true (as that space will
    // now be filled)
    int newXB = currentPiece.squares[s_to_shift].xB + changeColumn;
    int newYB = currentPiece.squares[s_to_shift].yB + changeRow;
    this->board[newXB][newYB].filled = true;
    // Stores the new row and column location of the cube (after rotating it)
    newFilledX[i] = newXB;
    newFilledY[i] = newYB;
    // Changes the properties of the piece we are rotating by updating the
    // location of the cube we just rotated to where it now is on the gameboard
    currentPiece.squares[s_to_shift] = space(true, newXB, newYB);
    return true;
  } else {
    return false;
  }
}

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
bool gameBoard::rotate4(int matrix[4][4], int rowSize, int colSize,
  piece &currentPiece, Adafruit_ILI9341 tft, int (&newFilledX)[4],
  int (&newFilledY)[4]) {
  int finalRow, finalColumn;
  int blockNum = 0;
  bool success = true;
  // Stores the original column and row location of each cube in the piece we
  // are rotating
  int xOrigB[4] = {currentPiece.squares[0].xB, currentPiece.squares[1].xB,
    currentPiece.squares[2].xB, currentPiece.squares[3].xB};
  int yOrigB[4] = {currentPiece.squares[0].yB, currentPiece.squares[1].yB,
    currentPiece.squares[2].yB, currentPiece.squares[3].yB};
  // Checks each row in matrix
  for (int row = 0; row < rowSize; row++) {
    // Checks each column in matrix
    for (int col = 0; col < colSize; col++) {
      // If a one is found calls rot90Clockwise to rotate the 1 and then calls
      // executeRotation to ensure that the rotation is valid and if so carry
      // it out
      if (matrix[row][col] == 1) {
        matrix[row][col] = 0;
        rot90Clockwise(rowSize, colSize, row, col, finalRow, finalColumn);
        success = executeRotation(row, col, finalRow, finalColumn,
          currentPiece, blockNum, tft, xOrigB, yOrigB, newFilledX, newFilledY);
        // blockNum tells us how many cubes we have rotated (i.e. what
        // iteration we are on)
        blockNum++;
        // Ends the rotation process if rotating one of the cubes in the piece
        // is invalid
        if (!success) {
          break;
        }
      }
    }
    // Ends the rotation process if rotating one of the cubes in the piece
    // is invalid
    if (!success) {
      break;
    }
  }
  return success;
}

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
bool gameBoard::rotate3(int matrix[3][3], int rowSize, int colSize,
  piece &currentPiece, Adafruit_ILI9341 tft, int (&newFilledX)[4],
  int (&newFilledY)[4]) {
  int finalRow, finalColumn;
  int blockNum = 0;
  bool success = true;
  // Stores the original column and row location of each cube in the piece we
  // are rotating
  int xOrigB[4] = {currentPiece.squares[0].xB, currentPiece.squares[1].xB,
    currentPiece.squares[2].xB, currentPiece.squares[3].xB};
  int yOrigB[4] = {currentPiece.squares[0].yB, currentPiece.squares[1].yB,
    currentPiece.squares[2].yB, currentPiece.squares[3].yB};
  // Checks each row in matrix
  for (int row = 0; row < rowSize; row++) {
    // Checks each column in matrix
    for (int col = 0; col < colSize; col++) {
      // If a one is found calls rot90Clockwise to rotate the 1 and then calls
      // executeRotation to ensure that the rotation is valid and if so carry
      // it out
      if (matrix[row][col] == 1) {
        matrix[row][col] = 0;
        rot90Clockwise(rowSize, colSize, row, col, finalRow, finalColumn);
        success = executeRotation(row, col, finalRow, finalColumn,
          currentPiece, blockNum, tft, xOrigB, yOrigB, newFilledX, newFilledY);
        // blockNum tells us how many cubes we have rotated (i.e. what
        // iteration we are on)
        blockNum++;
        // Ends the rotation process if rotating one of the cubes in the piece
        // is invalid
        if (!success) {
          break;
        }
      }
    }
    // Ends the rotation process if rotating one of the cubes in the piece
    // is invalid
    if (!success) {
      break;
    }
  }
  return success;
}

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
void gameBoard::processRotation(piece &currentPiece, Adafruit_ILI9341 tft) {
  bool res;
  int cID = currentPiece.pID;
  int x, y;
  // Sets the original column and row location, for each cube in the piece, on
  // the gameboard to false so it doesn't mistakingly rotate into itself
  for (int i = 0; i < 4; i++) {
    x = currentPiece.squares[i].xB;
    y = currentPiece.squares[i].yB;
    this->board[x][y].filled = false;
  }
  // Stores the original column and row location of each cube in the piece we
  // are rotating (in arrays xOrigB and yOrigB)
  // Stores the original x and y coordinates on the gameboard of cube in the
  // piece we are rotating (in arrays xOrigD and yOrigD)
  int xOrigB[4], yOrigB[4], xOrigD[4], yOrigD[4];
  for (int i = 0; i < 4; i++) {
    xOrigB[i] = currentPiece.squares[i].xB;
    yOrigB[i] = currentPiece.squares[i].yB;
    xOrigD[i] = currentPiece.squares[i].xD;
    yOrigD[i] = currentPiece.squares[i].yD;
  }

  // Initializes arrays for the column and row location of each cube in the
  // piece after they have been rotated
  int newFilledX[4] = {-1};
  int newFilledY[4] = {-1};

  // Maps the piece to rotate to a matrix depending on its pID and current
  // rotation and then rotates this matrix (via rotate3 or rotate4)
  // First switch is regards to the pID and then second switch is regards to
  // the current rotation
  switch(cID) {
    case 1:
    {
    int rot = currentPiece.currentRotation;
    switch(rot) {
      case 1:
      {
      int matrix[4][4] = {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0},
      {0, 0, 1, 0}};
      res = rotate4(matrix, 4, 4, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 2:
      {
      int matrix[4][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1},
      {0, 0, 0, 0}};
      res = rotate4(matrix, 4, 4, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 3:
      {
      int matrix[4][4] = {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0},
      {0, 1, 0, 0}};
      res = rotate4(matrix, 4, 4, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 4:
      {
      int matrix[4][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0},
      {0, 0, 0, 0}};
      res = rotate4(matrix, 4, 4, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
    }
    break;
    }
    case 2:
    {
    int rot = currentPiece.currentRotation;
    switch(rot) {
      case 1:
      {
      int matrix[3][3] = {{0, 1, 0}, {0, 1, 0}, {1, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 2:
      {
      int matrix[3][3] = {{1, 0, 0}, {1, 1, 1}, {0, 0, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 3:
      {
      int matrix[3][3] = {{0, 1, 1}, {0, 1, 0}, {0, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 4:
      {
      int matrix[3][3] = {{0, 0, 0}, {1, 1, 1}, {0, 0, 1}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
    }
    break;
    }
    case 3:
    {
    int rot = currentPiece.currentRotation;
    switch(rot) {
      case 1:
      {
      int matrix[3][3] = {{0, 1, 0}, {0, 1, 0}, {0, 1, 1}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 2:
      {
      int matrix[3][3] = {{0, 0, 0}, {1, 1, 1}, {1, 0, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 3:
      {
      int matrix[3][3] = {{1, 1, 0}, {0, 1, 0}, {0, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 4:
      {
      int matrix[3][3] = {{0, 0, 1}, {1, 1, 1}, {0, 0, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
    }
    break;
    }
    case 4:
    {
    break;
    }
    case 5:
    {
    int rot = currentPiece.currentRotation;
    switch(rot) {
      case 1:
      {
      int matrix[3][3] = {{0, 1, 0}, {0, 1, 1}, {0, 0, 1}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 2:
      {
      int matrix[3][3] = {{0, 0, 0}, {0, 1, 1}, {1, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 3:
      {
      int matrix[3][3] = {{1, 0, 0}, {1, 1, 0}, {0, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 4:
      {
      int matrix[3][3] = {{0, 1, 1}, {1, 1, 0}, {0, 0, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
    }
    break;
    }
    case 6:
    {
    int rot = currentPiece.currentRotation;
    switch(rot) {
      case 1:
      {
      int matrix[3][3] = {{0, 1, 0}, {1, 1, 1}, {0, 0, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 2:
      {
      int matrix[3][3] = {{0, 1, 0}, {0, 1, 1}, {0, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 3:
      {
      int matrix[3][3] = {{0, 0, 0}, {1, 1, 1}, {0, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 4:
      {
      int matrix[3][3] = {{0, 1, 0}, {1, 1, 0}, {0, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
    }
    break;
    }
    case 7:
    {
    int rot = currentPiece.currentRotation;
    switch(rot) {
      case 1:
      {
      int matrix[3][3] = {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 2:
      {
      int matrix[3][3] = {{0, 0, 0}, {1, 1, 0}, {0, 1, 1}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 3:
      {
      int matrix[3][3] = {{0, 1, 0}, {1, 1, 0}, {1, 0, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
      case 4:
      {
      int matrix[3][3] = {{1, 1, 0}, {0, 1, 1}, {0, 0, 0}};
      res = rotate3(matrix, 3, 3, currentPiece, tft, newFilledX, newFilledY);
      break;
      }
    }
    break;
    }
  }

  // If rotation was successful erases the old location of the piece and draws
  // the new rotated piece
  if (res) {
    // Erases the old location of the piece
    for (int i = 0; i < 4; i++) {
      if (yOrigB[i] >= 4) {
        tft.fillRect(xOrigD[i], yOrigD[i], 21, 13, colours::BLACK);
      }
    }
    // Draws the new rotated piece and changes the current rotation member of
    // the current piece
    drawPiece(currentPiece, tft);
    if (currentPiece.currentRotation == 4) {
      currentPiece.currentRotation = 1;
    } else {
      currentPiece.currentRotation += 1;
    }
  // If the rotation was unsuccessful, restores all the properties of the
  // current piece and gameboard prior to trying to rotate the piece
  } else {
    int i = 0;
    // Restores the state of the gameboard before trying to rotate the piece
    while (newFilledX[i] != -1 and newFilledY[i] != -1) {
      this->board[newFilledX[i]][newFilledY[i]].filled = false;
      i++;
    }
    // Restores the properties of the current piece to how they were prior to
    // trying to rotate the piece
    for (int i = 0; i < 4; i++) {
      currentPiece.squares[i] = space(true, xOrigB[i], yOrigB[i]);
      this->board[xOrigB[i]][yOrigB[i]].filled = true;
    }
  }
}
