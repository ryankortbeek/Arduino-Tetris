# MCU-Tetris

## Developers 
- Justin Boileau 
- Ryan Kortbeak

## Included Files:
  * README
  * gameBoard.h
  * gameBoard.cpp
  * nums.h
  * pieceGen.h
  * pieceGen.cpp
  * play.h
  * play.cpp
  * Makefile

## Wiring Instructions (for arduino)
### TFT Display

      The display pins are numbered left to right in this document when they
      are being viewed with their labels showing rightside up. All pins not
      listed are not used.

      Display Pin     Arduino Mega Pin
      1 GND           BB GND bus
      2 Vin           BB Vcc (+ive) bus

      4 CLK           Pin 52
      5 MISO          Pin 50
      6 MOSI          Pin 51
      7 CS            Pin 10
      8 D/C           Pin 9

      19 CCS          Pin 6

### Joystick Connections

      Joystick Pin    Arduino Mega Pin
      +5v             Vcc
      GND             GND
      VRx             Analog 1
      VRy             Analog 0
      SW              Pin 8

## Running Instructions
1. Navigate to the directory containing all the included files
2. Enter the command "make upload" to compile the code and upload it to the arduino
3. Orientate the arduino so that the game on the display is upright
4. Move the joystick left or right to change the selected difficulty and once on the desired difficulty press down on the joystick and the game will begin. The difference by difficulty is as follows:
- NOTE: On all 3 difficulties the fall rate increases by 100 ms for every three lines the player fills. The initial fall rate and piece selection algorithm vary based on difficulty.
- Easy : Uses the "NiceTris" algorithm to analyze the current board state for every new piece to actively try to give the player the best possible piece for the current situation. Start fall rate = 750 ms.
- Medium : Uses the "GrabBag" algorithm, this is what is used most often by Tetris games online. Basically simulates a bag full of all the possible pieces and each time we reach in the bag and pull out one piece. The previously picked pieces are not put bag in the bag until every piece has been pulled out. This guarantees a maximum of 12 other pieces between seeing two of the same. Start fall rate = 700 ms.
- Hard : Uses the "BustHead" algorithm which analyzes the current contour of the board and actively tries to give the player the worst piece for their current situation. Start fall rate = 600 ms.
5. The goal is to fill as many rows/lines as possible and more points are awarded when more rows/lines are filled at once
6. Controls are as follows (all actions refer to movements of the joystick):
    - UP: Rotate the block 90 degrees clockwise
    - DOWN: Shift the current piece down one square
    - RIGHT: Shift the current piece right one square
    - LEFT: Shift the current piece left one square
    - CLICK: Drop the piece to the lowest unfilled squares below it
7. The game ends once the blocks stack up past the top of the gameboard. A
  game over screen will be displayed for a few seconds and then you will be
  returned to the home screen

## Notes and Assumptions:
For a detailed walkthrough with regards to how rotating a piece is carried out, start at the function header for processRotation in gameBoard.cpp and navigate to the functions called inside processRotation and so on...
