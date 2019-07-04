# Arduino-Tetris

Included Files:
  * README
  * gameBoard.h
  * gameBoard.cpp
  * nums.h
  * pieceGen.h
  * pieceGen.cpp
  * play.h
  * play.cpp
  * Makefile
NOTE: DOUBLE CHECK THATS ALL THEM ^

Wiring Instructions:
  * Connect wires as follows:

  TFT Display:

      The display pins are numbered left to right in this document when they
      are being viewed with their labels showing rightside up. Note you do not
      require the touchscreen for this assignment, so it is probably a lot
      easier to not wire the Y+/X+/Y-/X- pins.

      Display Pin     Arduino Mega Pin
      1 GND           BB GND bus
      2 Vin           BB Vcc (+ive) bus

      4 CLK           Pin 52
      5 MISO          Pin 50
      6 MOSI          Pin 51
      7 CS            Pin 10
      8 D/C           Pin 9

      19 CCS          Pin 6

  Joystick connections:

      Joystick Pin    Arduino Mega Pin
      +5v             Vcc
      GND             GND
      VRx             Analog 1
      VRy             Analog 0
      SW              Pin 8
NOTE: MUST UPDATE THIS ^

Running Instructions:
  1) Navigate to the directory containing all the included files
  2) Enter the command "make upload" to compile the code and upload it to
  the arduino
  3) Orientate the arduino so that the game on the display is upright
  4) Move the joystick left or right to change the selected difficulty and once
  on the desired difficulty press down on the joystick and the game will begin
  5) The goal is to fill as many rows/lines as possible and more points are
  awarded when more rows/lines are filled at once
  6) The game ends once the blocks stack up past the top of the gameboard. A
  game over screen will be displayed for a few seconds and then you will be
  returned to the home screen
NOTE: MAYBE MORE ON HOW POINTS ARE AWARDED? ^

Notes and Assumptions:
  * For a detailed walkthrough with regards to how rotating a piece is carried
  out, start at the function header for processRotation in gameBoard.cpp and
  navigate to the functions called inside processRotation and so on...
NOTE: EXPLAIN THE DIFFERENT DIFFICULTY LEVELS ^

NOTE: ADD ANYTHING ELSE YOU THINK WE NEED
