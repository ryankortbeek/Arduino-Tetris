/*------------------------------------------------
#   Name: Justin Boileau, Ryan Kortbeek
#   ID: 1535268, 1535901
#   CMPUT 275, Winter 2019
#
#   Tetris: pieceGen.h
#
#-----------------------------------------------*/

#ifndef __PIECE_GEN_H
#define __PIECE_GEN_H

#include <Arduino.h>

using namespace std;

class pieceGenerator {

public:
  pieceGenerator();
  /* Implementation of the "grabBag" algorithm (most commonly used in Tetris).
  Basic idea is that all 7 blocks are placed in a "bag" and then one is
  randomly selected, added to the queue of pieces, and then tossed away. Once
  all 7 pieces have been picked that bag is refilled and the process repeats.
  This ensures that in the worst case there is 12 pieces between any two of the same pieces.
  */
  struct Bag {
    // constructor
    Bag();
    // the "bag" array
    int bag[7];
    // refill the bag with pieces 1 to 7
    void refillBag();
    // grab 1 pieces from the bag and re arrange the bag so that we can't
    // get the same one twice, adds the grabbed piece to the piece queue
    int grab(int remaining);
    //
    void printBag();
  };
  void printQ();

  /* Everything past here is for the BustHead algorithm based on contour
  analysis of the current board state
  */

  // A variable size bag --> elements < size will be set to null
  struct varBag {
    int vBag[7];
    int size;
    // constructors
    varBag(int size);
    varBag();
    // add a piece --> assumes bag isnt full or piece wont be added
    void addPiece(int pID);
    // print the contents of the bag
    void printBag();
    // grab an element from the bag and adjust the size and contents
    // accordingly, simulates "pulling out" one element
    int grab();
  };
};

#endif
