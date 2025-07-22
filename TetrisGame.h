// Copyright (C)

#pragma once

#include "./MockTerminalManager.h"
#include "./TerminalManager.h"
#include "./Tetromino.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>
#include <vector>

// Declaration of Row class

// Rows do only save settled blocks, not falling Tetrominos

extern int ID;

class Row {
public:
  // Constructor of a Row
  Row();

  // Rows will not be destructed manually so no need for destructor or something

  // Empty the Row
  void clear();

  // Get the upper row (for when the row drops down)
  void getUpper(const Row &row);

  // Check if the row is full
  bool isFull() const;

  // Number (ID) of the row.
  int num_;

  // Array holding 10 cells of a pair of Color and a bool if the cell is to be
  // collided with.
  std::pair<int, int> row_[10];
};

// Declaration of TetrisGame class

class TetrisGame {
public:
  TetrisGame(int argc, char **argv, bool mock = false);

  void play(int cycles = -1);

  void restartHandler();

  std::chrono::system_clock::time_point start_;
  std::chrono::system_clock::time_point end_;
  std::vector<std::chrono::duration<float>> times_;

protected:
  // A function for the timed falling of the Tetromino
  void gameFalling();
  // Trivial. Only calls other functions (has an if statement.....)

  // Hanldes inputs from the user
  // Handles Rightarrow, Leftarrow, Downarrow, A-Key and D-Key
  // Does not rotate the O-Tetromino
  void handleInput(UserInput uI);
  FRIEND_TEST(TetrisGameTest, inputhandling);

  // Checks Collision for every cell that the rotated I-Tetromino would have
  bool checkCollisionRotateICW() const;
  FRIEND_TEST(TetrisGameTest, checkCollisionRotateICW);

  // Checks Collision for every cell that the rotated Tetromino would have
  bool checkCollisionRotateCW(bool CW);
  FRIEND_TEST(TetrisGameTest, checkCollisionRotateCW);

  // Checks Collision to the left of every cell in the current Tetromino
  bool checkCollisionLeft() const;
  FRIEND_TEST(TetrisGameTest, checkCollisions);

  // Checks Collision to the right of every cell in the current Tetromino
  bool checkCollisionRight() const;
  FRIEND_TEST(TetrisGameTest, checkCollisions);

  // Checks Collision under every cell in the current Tetromino
  bool checkCollisionDown() const;
  FRIEND_TEST(TetrisGameTest, checkCollisions);

  // Calculate game speed
  void calculateGameSpeed();
  // Trivial. It's literally a bunch of if else statements.
  // No calculations whatsoever happens inside.

  // Removes the old Tetromino from the screen
  void removeTetrominoOld();
  FRIEND_TEST(TetrisGameTest, removeTetrominoOld);

  // Buffers the current Tetromino (Rotates it and adjusts the position)
  void bufferTetromino();
  // Tested in TetrisGameTest::bufferTetromino

  // Settle a Tetromino to the screem
  void settleTetromino();
  FRIEND_TEST(TetrisGameTest, settleTetromino);

  // Generates a new Tetromino and handles the NEXT screen
  void generateNextTetromino();
  FRIEND_TEST(TetrisGameTest, generateNextTetromino);

  // Checks if a Line is full
  void checkLineFull();
  // Is tested in TEST(Row, isFull) and used in settleTetrominoTest.
  // It is also tested in settleTetrominoTest in the tests for point adjustment.

  // Grants points based on the number of lines cleared
  void lineScore(int lines);
  // Tested in settleTetrominoTest.
  // Trivial. No real calculations happen inside.

  // Calculate number of digits in score
  int calculateLengthOfScore(int number) const;
  // Trivial. Found this on the first google how to get the number of digits in
  // a number.

  // Writes the current Tetromino to the screen
  void writeToScreen();
  FRIEND_TEST(TetrisGame, writeToScreen);

  // Draws the game screen and the scores
  void drawScreen();
  // Tested in many Test suites

  // Initializes a standard game
  void initGame();
  FRIEND_TEST(TetrisGame, initGame);

  // Initialize the Screen
  void initScreen() const;
  // Only draws on the TM...
  // You can see how it works. Just play a game and look at the screen.

  // A screen where the game happens
  // Highest Row is 0.
  Row screen_[20];

  // The current onscreen Tetromino
  Tetromino currentTetromino_;

  // The upcoming Tetromino
  Tetromino nextTetromino_;

  // A buffer for the Tetromino
  std::vector<std::pair<int, int>> points_;

  // The position of the current Tetromino
  std::pair<int, int> positionTetromino_;

  // A terminalmanager to display the game
  std::unique_ptr<VirtualTerminalManager> tm_;

  // Bool if the I is up
  bool iIsUp{true};

  // Array of lines that are to be erased
  std::vector<std::pair<int, bool>> linesToErase_;

  // Starting level
  int startLevel_{0};

  // Values
  int score_{0};
  int level_{0};
  int lines_{0};
  int high_{0};
  int gameSpeed_{48};

  bool gameOver_{false};

  // Keycodes for A and D
  int keycodeA_{97};
  int keycodeD_{100};

  // bools for game settings
  bool hardDropOn{false};
  bool wallKickOn{false};
};
