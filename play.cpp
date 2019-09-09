/*------------------------------------------------
#   Name: Justin Boileau, Ryan Kortbeek
#
#
#   Tetris: play.cpp
#
#   Main file for game to be run from, contains functions for interacting
#   with the device hardware as well as the main program flow.
#
#-----------------------------------------------*/

#include "play.h"

void setup() {
  // Initialize arduino
  init();
  // Initialize joystick
  pinMode(pins::JOY_BUTTON, INPUT_PULLUP);
  // Initialize the serial port for testing
  Serial.begin(9600);
  // Set up the screen
  Adafruit_ILI9341 tft = Adafruit_ILI9341(pins::TFT_CS, pins::TFT_DC);
  TouchScreen ts = TouchScreen(pins::XP, pins::YP, pins::XM, pins::YM, 300);
  // Initialize tft display
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
}

int readJoy(unsigned long startT, int maxT) {
  // max/min joystick values
  int highBound = ard_consts::JOY_CENTER + ard_consts::JOY_DEADZONE;
  int lowBound = ard_consts::JOY_CENTER - ard_consts::JOY_DEADZONE;
  // reads the position of the joystick
  int xVal = analogRead(pins::JOY_X);
  int yVal = analogRead(pins::JOY_Y);
  int buttonVal = digitalRead(pins::JOY_BUTTON);


  while ((millis() - startT) < maxT) {
    // read joystick vals
    xVal = analogRead(pins::JOY_X);
    yVal = analogRead(pins::JOY_Y);
    buttonVal = digitalRead(pins::JOY_BUTTON);
    if (buttonVal == LOW) {
      return 4;
    }
    if ((yVal > highBound) || (yVal < lowBound) || (xVal > highBound)
      || (xVal < lowBound)) {
      if (yVal < lowBound) {
        // down
        return 2;
      } else if (yVal > highBound) {
        // ROTATION
        return 3;
      }
      if (xVal > highBound) {
        // left
        return 0;
      } else if (xVal < lowBound) {
        // right
        return 1;
      }
    }
  }
  return 2;
}

int calcScore(int redraw) {
  int inc;
  // shift number of lines down 1 so the multiplier works right
  inc = redraw*(1000 + 250*(redraw - 1));
  return inc;
}
void redrawScore(unsigned long score, Adafruit_ILI9341 tft) {
  tft.setCursor(80, 0);
  tft.fillRect(80, 0, 100, 18, colours::BLACK);
  tft.setTextSize(2);
  tft.print(score);
}

int play(int diff) {
  Adafruit_ILI9341 tft = Adafruit_ILI9341(pins::TFT_CS, pins::TFT_DC);
  gameBoard gb = gameBoard();
  gb.printBoard(tft);
  // Initializes variables
  bool gameOver = false;
  int redraw = 0;
  int pID = 1;
  int move = -1;
  int res;
  int lines[2][4];
  unsigned long score = 0;
  int maxT, remaining;
  int totalLines = 0;
  int checks[8] = {0};
  int timeMult = 1;
  // 2d array holding pID and a 1 or 0 for if that piece is in the bag for
  // this iteration (for hard mode)
  int insertPieces[7][2];
  // instantiates varBag and bag objects from pieceGenerator class
  pieceGenerator::Bag bag;
  pieceGenerator::varBag vBag;


  // Difficulty Selection
  if (diff == 1) {
    // Calls varBag constructor
    vBag = pieceGenerator::varBag(7);
    // 750 ms starting fall timme for easy diff
    maxT = 750;
    // Initializes the the insertPieces array
    for (int i = 0; i < 7; i++) {
      insertPieces[i][0] = i + 1;
      insertPieces[i][1] = 0;
    }
  } else if (diff == 2) {
    // Calls Bag constructor, sets up bag
    bag = pieceGenerator::Bag();
    remaining = 7;
    bag.refillBag();
    // 700 ms starting fall time for medium diff
    maxT = 700;
    bag.refillBag();
  } else if (diff == 3) {
    // Calls varBag constructor
    vBag = pieceGenerator::varBag(7);
    // 600 ms starting fall time for hard diff
    maxT = 600;
    // Initializes the the insertPieces array
    for (int i = 0; i < 7; i++) {
      insertPieces[i][0] = i + 1;
      insertPieces[i][1] = 1;
    }
  }

  // TIMING
  // Made startT unsigned long so it the game can be played longer
  // int startT overflows after < 30 sec
  unsigned long startT;
  while (gameOver == false) {
    // Adds the lines filled to the total count
    totalLines += redraw;
    // Adjusts the fall speed based on total lines filled
    if ((totalLines - timeMult*3) > 0) {
      maxT -= 100;
      timeMult++;
    }
    // Adds and redraws the score if the player got any lines
    if (redraw > 0) {
      score += calcScore(redraw);
      redrawScore(score, tft);
    }
    startT = millis();
    // Resets the number of lines to redraw and the result
    redraw = 0;
    res = 1;

    // Resets the lines array for checking if player got any lines
    for (int i = 0; i < 4; i++) {
      lines[0][i] = 1;
      lines[1][i] = 0; // sets the line to be 0 be default so checlines
      // will not only return true for lines that the piece stopped at
    }

    // Generates piece based on difficulty

    // Easy piece gen using niceTris algorithm
    if (diff == 1) {
      gb.contour(&checks);
      vBag = pieceGenerator::varBag(gb.niceTris(&insertPieces, &checks));
      for (int i = 0; i < 7; i++) {
        if (insertPieces[i][1] == 1) {
          vBag.addPiece(insertPieces[i][0]);
        }
      }
      pID = vBag.grab();
      // Serial.print("Grabbed: "); Serial.print(pID); Serial.print(" New size: "); Serial.print(vBag.size); Serial.println(" New Bag:");
      // vBag.printBag();

    // Medium piece gen using grabBag algorithm
    } else if (diff == 2) {
      if  (remaining == 0) {
        bag.refillBag();
        remaining = 7;
      }
      pID = bag.grab(remaining);
      remaining--;

    // Hard piece gen using bustHead algorithm
    } else if (diff == 3) {
      gb.contour(&checks);
      vBag = pieceGenerator::varBag(gb.bustHead(&insertPieces, &checks, pID));
      for (int i = 0; i < 7; i++) {
        if (insertPieces[i][1] == 1) {
          vBag.addPiece(insertPieces[i][0]);
        }
      }
      pID = vBag.grab();
    }

    // Spawns the piece in the area above the board (invisible to player)
    gameBoard::piece p = gb.spawnPiece(pID, tft);
    while (res != -1) {
      // can mess around with the delay times more
      delay(200);
      // reads till user does something or the time limit passes
      move = readJoy(startT, maxT);
      if ((move != 3) && (move != 4)) {
        // shift returns a value from validateShift that tells us
        // what happened
        res = gb.shift(move, p, tft);
        if (res == -1) {
          // check for filled line
          for (int i = 0; i < 4; i++) {
            lines[1][i] = p.squares[i].yB;
          }
          // checks if the player filled any lines
          redraw = gb.checkLines(&lines);
          // redraws the lines above the one(s) the player got, if any
          if (redraw > 0) {
            gb.rePrintBoardAbove(&lines, redraw, tft);
          }
          // If a block stopped above the top level of the board, game over
          for (int i = 0; i < 4; i++) {
            if (p.squares[i].yB < 4) {
              gameOver = true;
            }
          }
        }
        if (move == 2) {
          // 2 = down
          // resets the timer
          startT = millis();
        }
      } else if (move == 3) {
        // 3 = rotation
        gb.processRotation(p, tft);
      } else if (move == 4) {
        // 4 = drop
        gb.drop(p, tft);
        // res = -1 means piece has hit the bottom
        res = -1;
        // check for filled line
        for (int i = 0; i < 4; i++) {
          lines[1][i] = p.squares[i].yB;
        }
        // checks for filled line in case of a drop
        redraw = gb.checkLines(&lines);
        // reprints above the line(s) filled, if any
        if (redraw > 0) {
          gb.rePrintBoardAbove(&lines, redraw, tft);
        }
        // checks for gameover
        for (int i = 0; i < 4; i++) {
          if (p.squares[i].yB < 4) {
            gameOver = true;
          }
        }
      }
    }
  }
  // returns score after game is over
  return score;
}

int chgDiff(int diff, int change) {
  // change = -1 for down, 1 for up
  diff += change;
  if (diff == 4) {
    diff = 1;
  } else if (diff == 0) {
    diff = 3;
  }
  return diff;
}

// Deletes old score and prints new one
void printDiff(int diff, Adafruit_ILI9341 tft) {
  tft.fillRect(140, 148, 100, 25, colours::BLACK);
  tft.setCursor(140, 150);
  if (diff == 1) {
    tft.drawLine(138, 156, 138, 157, colours::WHITE);
    tft.drawLine(139, 154, 139, 158, colours::WHITE);
    tft.drawLine(140, 152, 140, 160, colours::WHITE);
    tft.drawLine(141, 150, 141, 162, colours::WHITE);
    tft.drawLine(142, 148, 142, 164, colours::WHITE);
    tft.setCursor(148, 150);
    tft.print("Easy");
    tft.drawLine(200, 148, 200, 164, colours::WHITE);
    tft.drawLine(201, 150, 201, 162, colours::WHITE);
    tft.drawLine(202, 152, 202, 160, colours::WHITE);
    tft.drawLine(203, 154, 203, 158, colours::WHITE);
    tft.drawLine(204, 156, 204, 157, colours::WHITE);

  } else if (diff == 2) {
    tft.drawLine(138, 156, 138, 157, colours::WHITE);
    tft.drawLine(139, 154, 139, 158, colours::WHITE);
    tft.drawLine(140, 152, 140, 160, colours::WHITE);
    tft.drawLine(141, 150, 141, 162, colours::WHITE);
    tft.drawLine(142, 148, 142, 164, colours::WHITE);
    tft.setCursor(148, 150);
    tft.print("Medium");
    tft.drawLine(224, 148, 224, 164, colours::WHITE);
    tft.drawLine(225, 150, 225, 162, colours::WHITE);
    tft.drawLine(226, 152, 226, 160, colours::WHITE);
    tft.drawLine(227, 154, 227, 158, colours::WHITE);
    tft.drawLine(228, 156, 228, 157, colours::WHITE);
  } else if (diff == 3) {
    tft.drawLine(138, 156, 138, 157, colours::WHITE);
    tft.drawLine(139, 154, 139, 158, colours::WHITE);
    tft.drawLine(140, 152, 140, 160, colours::WHITE);
    tft.drawLine(141, 150, 141, 162, colours::WHITE);
    tft.drawLine(142, 148, 142, 164, colours::WHITE);
    tft.setCursor(148, 150);
    tft.print("Hard");
    tft.drawLine(200, 148, 200, 164, colours::WHITE);
    tft.drawLine(201, 150, 201, 162, colours::WHITE);
    tft.drawLine(202, 152, 202, 160, colours::WHITE);
    tft.drawLine(203, 154, 203, 158, colours::WHITE);
    tft.drawLine(204, 156, 204, 157, colours::WHITE);
  }
}

// Prints main menu display, waits for player to select difficulty and then
// returns it
int mainMenu(Adafruit_ILI9341 tft) {
  // max/min joystick values
  int highBound = ard_consts::JOY_CENTER + ard_consts::JOY_DEADZONE;
  int lowBound = ard_consts::JOY_CENTER - ard_consts::JOY_DEADZONE;
  // reads the position of the joystick
  int xVal = analogRead(pins::JOY_X);
  int yVal = analogRead(pins::JOY_Y);
  int buttonVal = digitalRead(pins::JOY_BUTTON);
  // Diff : 1 = Easy, 2 = Medium, 3 = Hard
  int diff = 1;
  // T
  tft.fillRect(5, 30, 40, 10, colours::RED);
  tft.fillRect(20, 30, 10, 60, colours::RED);
  // E
  tft.fillRect(50, 30, 10, 60, colours::ORANGE);
  tft.fillRect(50, 30, 35, 10, colours::ORANGE);
  tft.fillRect(50, 55, 35, 10, colours::ORANGE);
  tft.fillRect(50, 80, 35, 10, colours::ORANGE);
  // T
  tft.fillRect(90, 30, 40, 10, colours::YELLOW);
  tft.fillRect(105, 30, 10, 60, colours::YELLOW);
  // R ..... this one is retarded
  tft.fillRect(135, 30, 10, 60, colours::LIME);
  tft.fillRect(135, 30, 35, 10, colours::LIME);
  tft.fillRect(160, 30, 5, 30, colours::LIME);
  tft.fillRect(165, 30, 5, 25, colours::LIME);
  tft.fillRect(135, 55, 25, 10, colours::LIME);
  tft.fillRect(160, 65, 5, 25, colours::LIME);
  tft.fillRect(165, 70, 5, 20, colours::LIME);
  // I
  tft.fillRect(175, 30, 10, 10, colours::CYAN);
  tft.fillRect(175, 45, 10, 45, colours::CYAN);
  // S
  tft.fillRect(190, 30, 40, 10, colours::VIOLET);
  tft.fillRect(190, 30, 10, 30, colours::VIOLET);
  tft.fillRect(190, 55, 40, 10, colours::VIOLET);
  tft.fillRect(220, 55, 10, 30, colours::VIOLET);
  tft.fillRect(190, 80, 40, 10, colours::VIOLET);

  tft.setTextSize(2);
  tft.setCursor(5, 150);
  tft.print("Difficulty:");
  printDiff(diff, tft);
  tft.setCursor(5, 200);
  tft.print("Click to play!");


  while (buttonVal == HIGH) {
    delay(150);
    // reads the position of the joystick
    xVal = analogRead(pins::JOY_X);
    yVal = analogRead(pins::JOY_Y);
    buttonVal = digitalRead(pins::JOY_BUTTON);
    if (xVal > highBound) {
      // left
      diff = chgDiff(diff, -1);
      printDiff(diff, tft);
    } else if (xVal < lowBound) {
      // right
      diff = chgDiff(diff, 1);
      printDiff(diff, tft);
    }
  }
  tft.fillScreen(colours::BLACK);
  return diff;
}
// Prints "Game Over" one letter at a time
void gameOver(Adafruit_ILI9341 tft, unsigned long score) {
  tft.fillScreen(colours::BLACK);
  uint16_t cols[6] = {colours::RED, colours::ORANGE, colours::YELLOW,
    colours::LIME, colours::CYAN, colours::VIOLET};
  char gameOverMessage[8] = {'G', 'A', 'M', 'E', 'O', 'V', 'E', 'R'};
  tft.setTextSize(9);
  for (int i = 0; i < 8; i++) {
    tft.setTextColor(cols[i % 6]);
    if (i <= 3) {
      tft.setCursor(23 + i*48, 50);
    } else {
      tft.setCursor(23 + (i - 4)*48, 140);
    }
    tft.print(gameOverMessage[i]);
    delay(250);
  }
  tft.setTextSize(2);
  tft.setTextColor(colours::WHITE);
  tft.setCursor(23, 240);
  tft.print("Score: ");
  tft.setCursor(97, 240);
  tft.print(score);

  delay(4000);
  tft.fillScreen(colours::BLACK);
}

int main() {
  // Calls setup to initialize arduino
  setup();
  // Initializes tft display
  Adafruit_ILI9341 tft = Adafruit_ILI9341(pins::TFT_CS, pins::TFT_DC);
  while (true) {
    // Calls main menu function which will print and wait for the user to
    // select a difficulty
    int diff = mainMenu(tft);
    // Calls play function to play a tetris game at selected difficulty.
    // Returns the players score
    unsigned long score = play(diff);
    // Prints game over when the game ends and displays score.
    gameOver(tft, score);
  }
}
