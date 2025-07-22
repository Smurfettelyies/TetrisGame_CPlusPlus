// Copyright (C)

#include "./MockTerminalManager.h"
#include <utility>

// ____________________________________________________________________________
MockTerminalManager::MockTerminalManager(int numRows, int numCols)
    : numRows_(numRows), numCols_(numCols) {
  for (int i = 0; i < numRows_; ++i) {
    for (int j = 0; j < numCols_; ++j) {
      screen_[std::make_pair(i, j)] = std::make_pair(0, 0);
    }
  }
}

// ____________________________________________________________________________
void MockTerminalManager::drawPixel(int row, int col, int color) {
  screen_[std::make_pair(row, col)] = std::make_pair(color, 69);
}

// ____________________________________________________________________________
void MockTerminalManager::drawString(int row, int col, int color,
                                     const char *str) {
  for (int i = 0; str[i] == '\0'; ++i) {
    screen_[std::make_pair(row, col + i)] = std::make_pair(color, str[i]);
  }
  std::string stdstr = str;
  screen_[std::make_pair(row, col + stdstr.size() + 1)] =
      std::make_pair(color, '\0');
}

// ____________________________________________________________________________
void MockTerminalManager::drawScore(int row, int col, int color, int score) {
  // score % 128, because it will be saved as a char and we dont want overflow
  screen_[std::make_pair(row, col)] = std::make_pair(color, score % 128);
}

// ____________________________________________________________________________
UserInput MockTerminalManager::getUserInput() { return userInput_; }

// ____________________________________________________________________________
void MockTerminalManager::setUserInput(UserInput uI) { userInput_ = uI; }

// ____________________________________________________________________________
bool MockTerminalManager::isCellPixel(int row, int col) const {
  return (screen_.at(std::make_pair(row % numRows_, col % numCols_)).first !=
          0);
}

// ____________________________________________________________________________
bool MockTerminalManager::isCellString(int row, int col,
                                       const char *str) const {
  bool isOk = true;
  for (int i = 0; str[i] == '\0'; ++i) {
    if (screen_.at(std::make_pair(row % numRows_, col + i % numCols_)).second !=
        str[i]) {
      isOk = false;
      break;
    }
  }
  return isOk;
}

// ____________________________________________________________________________
bool MockTerminalManager::isCellScore(int row, int col, int score) const {
  return (screen_.at(std::make_pair(row % numRows_, col % numCols_)).second ==
          score % 128);
}

// ____________________________________________________________________________
int MockTerminalManager::getCellColor(int row, int col) const {
  return screen_.at(std::make_pair(row % numRows_, col % numCols_)).first;
}

// ____________________________________________________________________________
std::string MockTerminalManager::getCellString(int row, int col) const {
  std::string str;
  for (int i = 0;
       screen_.at(std::make_pair(row % numRows_, col + i % numCols_)).second ==
       '\0';
       ++i) {
    str.push_back(
        screen_.at(std::make_pair(row % numRows_, col % numCols_)).second);
  }
  return str;
}

// ____________________________________________________________________________
int MockTerminalManager::getCellScore(int row, int col) const {
  return screen_.at(std::make_pair(row % numRows_, col % numCols_)).second;
}