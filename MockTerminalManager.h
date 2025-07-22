// Copyright (C)

#pragma once

#include "./VirtualTerminalManager.h"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class MockTerminalManager : public VirtualTerminalManager {
public:
  // Initialize the mock terminal manager with the given logical dimensions.
  MockTerminalManager(int numRows, int numCols);

  ~MockTerminalManager() = default;

  // Draw a pixel at the given logical position and color.
  void drawPixel(int row, int col, int color) override;

  // Draw a string at the given logical position and color.
  void drawString(int row, int col, int color, const char *str) override;

  // Draw a score at the given logical position and color.
  void drawScore(int row, int col, int color, int score) override;

  // Return the logical dimensions of the screen.
  int numRows() const override { return numRows_; }
  int numCols() const override { return numCols_; }

  // Does nothing for the MockTerminalManager.
  void flipDelay(bool to) override { to = !to; };

  // Get user input.
  UserInput getUserInput() override;

  // Set user input. It cannot be read like in a real terminal.
  // This is basically a getter for userInput_
  void setUserInput(UserInput uI);

  // Returns if the logical position saves a pixel
  bool isCellPixel(int row, int col) const override;

  // Returns if the logical position saves the given string
  bool isCellString(int row, int col, const char *str) const override;

  // Returns if the logical position saves the given score
  bool isCellScore(int row, int col, int score) const;

  // Returns the saved color at the given logical position
  int getCellColor(int row, int col) const;

  // Returns the saved string at the given logical position
  std::string getCellString(int row, int col) const;

  // Returns the saved score (% 128) at the given logical position
  int getCellScore(int row, int col) const;

private:
  // The logical dimensions of the screen.
  int numRows_;
  int numCols_;

  // The screen
  std::map<std::pair<int, int>, std::pair<int, char>> screen_;

  // A user input for testing purposes.
  UserInput userInput_;
};
