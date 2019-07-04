/*------------------------------------------------
#   Name: Justin Boileau, Ryan Kortbeek
#   ID: 1535268, 1535901
#   CMPUT 275, Winter 2019
#
#   Tetris: pieceGen.cpp
#
#-----------------------------------------------*/

#include "pieceGen.h"

void pieceGenerator::Bag::refillBag() {
  // places pieces 1 to 7 in the bag
  for (int i = 0; i < 7; i++) {
    this->bag[i] = i+1;
  }
}
// Simulates grabbing and removing an element from the bag
int pieceGenerator::Bag::grab(int remaining) {
  int pull, piece;
  pull = random(0, remaining);
  piece = this->bag[pull];
  this->bag[pull] = this->bag[remaining-1];
  return piece;
}

void pieceGenerator::Bag::printBag() {
  for (int i = 0; i < 7; i++) {
    Serial.print(this->bag[i]); Serial.print(" ");
  }
  Serial.println("");
}

pieceGenerator::pieceGenerator() {}

pieceGenerator::Bag::Bag() {}

pieceGenerator::varBag::varBag(int size) {
  this->size = size;
  for (int i = 0; i < size; i++) {
    this->vBag[i] = 0;
  }

}
// add a piece to the bag, will not add if bag is full
void pieceGenerator::varBag::addPiece(int pID) {
  for (int i = 0; i < this->size; i++) {
    if (this->vBag[i] == 0) {
      this->vBag[i] = pID;
      break;
    }
  }
}

void pieceGenerator::varBag::printBag() {
  for (int i = 0; i < this->size; i++) {
    Serial.print(this->vBag[i]); Serial.print(" ");
  }
  Serial.println();
}

// Simulates grabbing and removing an element from the bag
int pieceGenerator::varBag::grab() {
  int pull, piece;
  pull = random(0, this->size);
  piece = this->vBag[pull];
  this->vBag[pull] = this->vBag[this->size-1];
  this->size--;
  return piece;
}

pieceGenerator::varBag::varBag() {}
