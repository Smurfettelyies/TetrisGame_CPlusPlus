// Copyright

#include "./TetrisGame.h"
#include <cstdio>
#include <stdexcept>

#include <iostream>

// Implementation of Row class

int ID = 0;

Row::Row() {
  num_ = ID % 20;
  ID++;
  clear();
}

void Row::clear() {
  for (auto &pair : row_) {
    pair = std::make_pair(0, 0);
  }
}

void Row::getUpper(const Row &row) {
  for (int i = 0; i < 10; ++i) {
    row_[i] = row.row_[i];
  }
}

bool Row::isFull() const {
  for (int i = 0; i < 10; ++i) {
    if (row_[i].second != 69) {
      return false;
    }
  }
  return true;
}

// Implementation of TetrisGame class

// Public

TetrisGame::TetrisGame(int argc, char **argv, bool mock) {
  const char *d = "default";
  if (argc > 1) {
    if (argv[1] != d) {
      try {
        startLevel_ = std::stoi(argv[1]);
      } catch (std::exception &e) {
        throw std::invalid_argument(
            "Usage: ./TetrisMain <level> <keycode a> <keycode d>\nTo get the "
            "default keycode for a but a different for d, do ./TetrisMain "
            "<level> default <keycode d>\n");
      }
    }
  }
  if (argc > 2) {
    if (argv[2] != d) {
      try {
        keycodeA_ = std::stoi(argv[2]);
      } catch (std::exception &e) {
        throw std::invalid_argument(
            "Usage: ./TetrisMain <level> <keycode a> <keycode d>\nTo get the "
            "default keycode for a but a different for d, do ./TetrisMain "
            "<level> default <keycode d>\n");
      }
    }
  }
  if (argc > 3) {
    if (argv[3] != d) {
      try {
        keycodeA_ = std::stoi(argv[3]);
      } catch (std::exception &e) {
        throw std::invalid_argument(
            "Usage: ./TetrisMain <level> <keycode a> <keycode d>\nTo get the "
            "default keycode for a but a different for d, do ./TetrisMain "
            "<level> default <keycode d>\n");
      }
    }
  }
  // Background Color
  Color Background{0, 0, 0};

  // Border Color
  Color Border{0.0, 1, 1};

  // Score Color
  Color TextFront{1, 0, 1};
  Color TextBack{0, 0, 0};

  // Default Colors
  Color ColorL{0.0, 1, 0.1};
  Color ColorJ{1, 0.0, 0.1};
  Color ColorZ{0.5, 0.5, 0.0};
  Color ColorS{0.0, 0.5, 0.5};
  Color ColorT{1, 0.0, 0.9};
  Color ColorI{0.5, 1, 0.5};
  Color ColorO{1, 1, 0.0};

  std::vector<std::pair<Color, Color>> colors_;

  colors_.push_back(std::make_pair(Background, Background));
  colors_.push_back(std::make_pair(Border, Border));
  colors_.push_back(std::make_pair(TextFront, TextBack));
  colors_.push_back(std::make_pair(ColorL, ColorL));
  colors_.push_back(std::make_pair(ColorJ, ColorJ));
  colors_.push_back(std::make_pair(ColorZ, ColorZ));
  colors_.push_back(std::make_pair(ColorS, ColorS));
  colors_.push_back(std::make_pair(ColorT, ColorT));
  colors_.push_back(std::make_pair(ColorI, ColorI));
  colors_.push_back(std::make_pair(ColorO, ColorO));
  if (!mock) {
    tm_ = std::make_unique<TerminalManager>(colors_);
  } else {
    tm_ = std::make_unique<MockTerminalManager>(100, 100);
  }
}

void TetrisGame::play(int cycles) {
  int cycle{0};
  srand(time(0)); // Seed random number generator
  initGame();
  UserInput uI_;
  times_.clear();
  start_ = std::chrono::system_clock::now();
  while (!((uI_ = tm_->getUserInput()).isEscape()) && !gameOver_ &&
         (cycles < 0 || cycle < cycles)) {
    removeTetrominoOld();
    std::chrono::milliseconds threshold((gameSpeed_ * 16));
    end_ = std::chrono::system_clock::now();
    if (end_ - start_ >= threshold) {
      if (cycles > 0) {
        cycle++;
      }
      if (cycles != -1) {
        times_.emplace_back((end_ - start_) * 1000);
      }
      start_ = end_;
      gameFalling();
    } else {
      handleInput(uI_);
    }
    bufferTetromino();
    writeToScreen();
    drawScreen();
    usleep(16'667);
  }
}

void TetrisGame::restartHandler() {
  while (1) {
    tm_->flipDelay(false);
    if (gameOver_) {
      tm_->drawString(tm_->numRows() - 31, tm_->numCols() / 2 - 3, 2,
                      "Game Over!");
      tm_->drawString(tm_->numRows() - 30, tm_->numCols() / 2 - 7, 2,
                      ("Score: " + std::to_string(score_)).c_str());
    }
    tm_->drawString(tm_->numRows() - 28, tm_->numCols() / 2 - 5, 2,
                    "Press Space to Play");
    tm_->drawString(tm_->numRows() - 27, tm_->numCols() / 2 - 5, 2,
                    "Press ESC to Exit");
    UserInput uI = tm_->getUserInput();
    if (uI.isSpace()) {
      for (int i = 0; i < tm_->numRows(); ++i) {
        for (int j = 0; j < tm_->numCols(); ++j) {
          tm_->drawPixel(i, j, 0);
        }
      }
      tm_->flipDelay(true);
      gameOver_ = false;
      score_ = 0;
      lines_ = 0;
      level_ = startLevel_;
      play();
    } else if (uI.isEscape()) {
      return;
    }
  }
}

// Private

void TetrisGame::gameFalling() {
  if (!checkCollisionDown()) {
    positionTetromino_.second++;
  } else {
    settleTetromino();
    generateNextTetromino();
  }
}

void TetrisGame::handleInput(UserInput uI) {
  if (uI.keycode_ != -1) {
    if (uI.isKeyLeft()) {
      if (!checkCollisionLeft()) {
        positionTetromino_.first--;
      }
    } else if (uI.isKeyRight()) {
      if (!checkCollisionRight()) {
        positionTetromino_.first++;
      }
    } else if (uI.isKeyDown()) {
      if (!checkCollisionDown()) {
        positionTetromino_.second++;
        score_++;
      } else {
        settleTetromino();
        generateNextTetromino();
      }
    } else if (uI.keycode_ == keycodeD_) {
      if (currentTetromino_.form() == TetrominoForm::O) {
        return;
      }
      if (currentTetromino_.form() == TetrominoForm::I) {
        if (!checkCollisionRotateICW()) {
          iIsUp = !iIsUp;
        }
      } else {
        if (!checkCollisionRotateCW(false)) {
          currentTetromino_.rotateCW(false);
        }
      }
    } else if (uI.keycode_ == keycodeA_) {
      if (currentTetromino_.form() == TetrominoForm::O) {
        return;
      }
      if (currentTetromino_.form() == TetrominoForm::I) {
        if (!checkCollisionRotateICW()) {
          iIsUp = !iIsUp;
        }
      } else {
        if (!checkCollisionRotateCW(true)) {
          currentTetromino_.rotateCW(true);
        }
      }
    }
  } else {
    return;
  }
}

bool TetrisGame::checkCollisionRotateICW() const {
  if (iIsUp) {
    for (int i = 0; i < 4; ++i) {
      if (screen_[positionTetromino_.second]
                  .row_[i - 2 + positionTetromino_.first]
                  .second == 69 ||
          i - 2 + positionTetromino_.first > 9 ||
          i - 2 + positionTetromino_.first < 1) {
        return true;
      }
    }
    return false;
  } else {
    for (int i = 0; i < 4; ++i) {
      if (screen_[i - 2 + positionTetromino_.second]
                  .row_[positionTetromino_.first]
                  .second == 69 ||
          i - 2 + positionTetromino_.second > 18) {
        return true;
      }
    }
    return false;
  }
}

bool TetrisGame::checkCollisionRotateCW(bool CW) { // Theoretically modifies
                                                   // the object, but
                                                   // technically doesn't.
  currentTetromino_.rotateCW(CW);
  bufferTetromino();
  bool ret_{false};
  for (const auto &point : points_) {
    if (screen_[point.second].row_[point.first].second == 69 ||
        point.first < 0 || point.first > 9 || point.second > 18) {
      ret_ = true;
    }
  }
  currentTetromino_.rotateCW(!CW);
  bufferTetromino();
  return ret_;
}

bool TetrisGame::checkCollisionLeft() const {
  for (const auto &point : points_) {
    // Returns true if any point goes out of bounds.
    if (point.first < 1) {
      return true;
    }
    if (screen_[point.second].row_[point.first - 1].second == 69) {
      return true;
    }
  }
  return false;
}

bool TetrisGame::checkCollisionRight() const {
  for (const auto &point : points_) {
    // Returns true if any point goes out of bounds.
    if (point.first > 8) {
      return true;
    }
    if (screen_[point.second].row_[point.first + 1].second == 69) {
      return true;
    }
  }
  return false;
}

bool TetrisGame::checkCollisionDown() const {
  for (const auto &point : points_) {
    // Returns true if any point goes out of bounds.
    // Retrurns True if any point hits another that is already onscreen.
    if (screen_[point.second + 1].row_[point.first].second == 69 ||
        point.second > 18) {
      return true;
    }
  }
  return false;
}

void TetrisGame::calculateGameSpeed() {
  if (level_ == 0) {
    gameSpeed_ = 48;
  } else if (level_ == 1) {
    gameSpeed_ = 43;
  } else if (level_ == 2) {
    gameSpeed_ = 38;
  } else if (level_ == 3) {
    gameSpeed_ = 33;
  } else if (level_ == 4) {
    gameSpeed_ = 28;
  } else if (level_ == 5) {
    gameSpeed_ = 23;
  } else if (level_ == 6) {
    gameSpeed_ = 18;
  } else if (level_ == 7) {
    gameSpeed_ = 13;
  } else if (level_ == 8) {
    gameSpeed_ = 8;
  } else if (level_ == 9) {
    gameSpeed_ = 6;
  } else if (level_ >= 29) {
    gameSpeed_ = 1;
  } else if (level_ >= 19) {
    gameSpeed_ = 2;
  } else if (level_ >= 16) {
    gameSpeed_ = 3;
  } else if (level_ >= 13) {
    gameSpeed_ = 4;
  } else if (level_ >= 10) {
    gameSpeed_ = 5;
  } else {
    throw std::runtime_error("Invalid level");
  }
}

void TetrisGame::removeTetrominoOld() {
  for (const auto &point : points_) {
    screen_[point.second].row_[point.first] = std::make_pair(0, 0);
  }
}

void TetrisGame::bufferTetromino() {
  if (currentTetromino_.form() != TetrominoForm::O) {
    if (currentTetromino_.form() == TetrominoForm::I) {
      points_ = currentTetromino_.getIRotation(iIsUp);
    } else {
      points_ = currentTetromino_.getDefaultForm();
      for (auto &point : points_) {
        for (int i = 0; i < 4 - static_cast<int>(currentTetromino_.rotation());
             ++i) {
          point = currentTetromino_.getRotation(point);
        }
      }
      if (points_.size() != 4) {
        throw std::runtime_error("Buffering Tetromino went wrong");
      }
    }
  } else {
    points_ = currentTetromino_.getDefaultForm();
  }
  for (auto &point : points_) {
    point.first += positionTetromino_.first;
    point.second += positionTetromino_.second;
    if (screen_[point.second].row_[point.first].second == 69) {
      gameOver_ = true;
    }
  }
}

void TetrisGame::settleTetromino() {
  for (const auto &point : points_) {
    screen_[point.second].row_[point.first] =
        std::make_pair(static_cast<int>(currentTetromino_.form()) + 3, 69);
  }
  checkLineFull();
  for (const auto &line : linesToErase_) {
    lines_++;
    if (lines_ % 10 == 0) {
      level_++;
      calculateGameSpeed();
    }
    if (line.second) {
      screen_[line.first].clear();
    }
    for (int j = line.first; j > -1; --j) {
      if (j != 0) {
        screen_[j].getUpper(screen_[j - 1]);
      } else {
        screen_[0].clear();
      }
    }
  }
  lineScore(linesToErase_.size());
  drawScreen();
}

void TetrisGame::generateNextTetromino() {
  // Calculating next Tetromino
  currentTetromino_ = nextTetromino_;
  nextTetromino_ = Tetromino{static_cast<TetrominoForm>(rand() % 7)};
  if (nextTetromino_.form() == currentTetromino_.form()) {
    nextTetromino_ = Tetromino{static_cast<TetrominoForm>(rand() % 7)};
  }
  if (currentTetromino_.form() == TetrominoForm::I) {
    positionTetromino_ = std::make_pair(5, 2);
    iIsUp = true;
  } else {
    positionTetromino_ = std::make_pair(5, 1);
  }
  // Clearing NEXT screen
  for (int i = tm_->numRows() - 18; i < tm_->numRows() - 11; ++i) {
    for (int j = tm_->numCols() / 2 + 8; j < tm_->numCols() / 2 + 14; ++j) {
      tm_->drawPixel(i, j, 0);
    }
    tm_->drawString(tm_->numRows() - 18, tm_->numCols() / 2 + 10, 2, "NEXT");
  }
  // Placing next Tetromino into NEXT screen
  std::vector<std::pair<int, int>> nextTetromino =
      nextTetromino_.getDefaultForm();
  if (nextTetromino_.form() == TetrominoForm::I) {
    nextTetromino = nextTetromino_.getIRotation(true);
    for (auto &point : nextTetromino) {
      point.second++;
    }
  }
  for (const auto &point : nextTetromino) {
    tm_->drawPixel(point.second + tm_->numRows() - 15,
                   nextTetromino_.form() == TetrominoForm::J
                       ? point.first + tm_->numCols() / 2 + 11
                       : point.first + tm_->numCols() / 2 + 10,
                   static_cast<int>(nextTetromino_.form()) + 3);
  }
}

void TetrisGame::checkLineFull() {
  linesToErase_.clear();
  for (const auto &row : screen_) {
    if (row.isFull()) {
      linesToErase_.emplace_back(std::make_pair(row.num_, true));
    }
  }
}

void TetrisGame::lineScore(int lines) {
  switch (lines) {
  case 0:
    break;
  case 1:
    score_ += 40 * (level_ + 1);
    break;
  case 2:
    score_ += 100 * (level_ + 1);
    break;
  case 3:
    score_ += 300 * (level_ + 1);
    break;
  case 4:
    score_ += 1200 * (level_ + 1);
  }
}

int TetrisGame::calculateLengthOfScore(int number) const {
  int length = 1;
  while (number / 10 >= 1) {
    number /= 10;
    length++;
  }
  return length;
}

void TetrisGame::writeToScreen() {
  for (const auto &point : points_) {
    screen_[point.second].row_[point.first] =
        std::make_pair(static_cast<int>(currentTetromino_.form()) + 3, 1);
  }
}

void TetrisGame::drawScreen() {
  for (const auto &row : screen_) {
    for (int i = 0; i < 10; ++i) {
      tm_->drawPixel(row.num_ + tm_->numRows() - 23, i + tm_->numCols() / 2 - 5,
                     row.row_[i].first);
    }
  }
  if (score_ > high_) {
    high_ = score_;
  }
  tm_->drawScore(tm_->numRows() - 24,
                 tm_->numCols() + 27 - calculateLengthOfScore(high_), 2, high_);
  tm_->drawScore(tm_->numRows() - 22,
                 tm_->numCols() + 27 - calculateLengthOfScore(score_), 2,
                 score_);
  tm_->drawScore(tm_->numRows() - 7,
                 tm_->numCols() + 27 - calculateLengthOfScore(level_), 2,
                 level_);
  tm_->drawScore(tm_->numRows() - 25, tm_->numCols() - 1, 2, lines_);
}

void TetrisGame::initGame() {
  nextTetromino_ = Tetromino{static_cast<TetrominoForm>(rand() % 7)};
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 10; ++j) {
      screen_[i].row_[j] = std::make_pair(0, 0);
    }
  }
  calculateGameSpeed();
  initScreen();
  generateNextTetromino();
  bufferTetromino();
  writeToScreen();
  drawScreen();
}

void TetrisGame::initScreen() const {
  // Draw playscreen
  for (int i = tm_->numRows() - 24; i < tm_->numRows() - 2; ++i) {
    tm_->drawPixel(i, tm_->numCols() / 2 - 6, 1);
    tm_->drawPixel(i, tm_->numCols() / 2 + 5, 1);
  }
  for (int j = tm_->numCols() / 2 - 5; j < tm_->numCols() / 2 + 5; ++j) {
    tm_->drawPixel(tm_->numRows() - 24, j, 1);
    tm_->drawPixel(tm_->numRows() - 3, j, 1);
  }
  // Draw linebar
  for (int i = tm_->numRows() - 26; i < tm_->numRows() - 24; ++i) {
    tm_->drawPixel(i, tm_->numCols() / 2 - 6, 1);
    tm_->drawPixel(i, tm_->numCols() / 2 + 5, 1);
  }
  for (int j = tm_->numCols() / 2 - 5; j < tm_->numCols() / 2 + 5; ++j) {
    tm_->drawPixel(tm_->numRows() - 26, j, 1);
  }
  tm_->drawString(tm_->numRows() - 25, tm_->numCols() / 2 - 4, 2, "LINES-");
  // Draw scorescreen
  for (int i = tm_->numRows() - 26; i < tm_->numRows() - 20; ++i) {
    tm_->drawPixel(i, tm_->numCols() / 2 + 7, 1);
    tm_->drawPixel(i, tm_->numCols() / 2 + 14, 1);
  }
  for (int j = tm_->numCols() / 2 + 7; j < tm_->numCols() / 2 + 15; ++j) {
    tm_->drawPixel(tm_->numRows() - 26, j, 1);
    tm_->drawPixel(tm_->numRows() - 21, j, 1);
  }
  tm_->drawString(tm_->numRows() - 25, tm_->numCols() / 2 + 8, 2, "TOP");
  tm_->drawString(tm_->numRows() - 23, tm_->numCols() / 2 + 8, 2, "SCORE");
  // Draw nextscreen
  for (int i = tm_->numRows() - 19; i < tm_->numRows() - 10; ++i) {
    tm_->drawPixel(i, tm_->numCols() / 2 + 7, 1);
    tm_->drawPixel(i, tm_->numCols() / 2 + 14, 1);
  }
  for (int j = tm_->numCols() / 2 + 7; j < tm_->numCols() / 2 + 15; ++j) {
    tm_->drawPixel(tm_->numRows() - 19, j, 1);
    tm_->drawPixel(tm_->numRows() - 11, j, 1);
  }
  for (int i = tm_->numRows() - 18; i < tm_->numRows() - 11; ++i) {
    for (int j = tm_->numCols() / 2 + 8; j < tm_->numCols() / 2 + 14; ++j) {
      tm_->drawPixel(i, j, 0);
    }
  }
  tm_->drawString(tm_->numRows() - 18, tm_->numCols() / 2 + 10, 2, "NEXT");
  // Draw level
  for (int i = tm_->numRows() - 9; i < tm_->numRows() - 5; ++i) {
    tm_->drawPixel(i, tm_->numCols() / 2 + 7, 1);
    tm_->drawPixel(i, tm_->numCols() / 2 + 14, 1);
  }
  for (int j = tm_->numCols() / 2 + 7; j < tm_->numCols() / 2 + 15; ++j) {
    tm_->drawPixel(tm_->numRows() - 9, j, 1);
    tm_->drawPixel(tm_->numRows() - 6, j, 1);
  }
  tm_->drawString(tm_->numRows() - 8, tm_->numCols() / 2 + 8, 2, "LEVEL");
}
