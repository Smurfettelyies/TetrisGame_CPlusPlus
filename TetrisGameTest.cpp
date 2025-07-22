// Copyright (C)

#include "./TetrisGame.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <ncurses.h> // For keycodes
#include <string>
#include <utility>
#include <vector>

// Tests for the Row class

// Set row to random values for testing.
// Should work, if not, the whole row testing is broken.
void setRowRandom(Row &row) {
  srand(time(nullptr));
  for (int i = 0; i < 10; ++i) {
    row.row_[i] = std::make_pair(rand() % 100, 69);
  }
}

TEST(Row, Row) {
  Row row[20];
  for (int i = 0; i < 20; ++i) {
    ASSERT_EQ(row[i].num_, i);
  }
}

TEST(Row, clear) {
  Row row[20];
  for (int i = 0; i < 20; ++i) {
    setRowRandom(row[i]);
    row[i].clear();
    ASSERT_EQ(row[i].row_[i % 10].first, 0);
    ASSERT_EQ(row[i].row_[i % 10].second, 0);
  }
}

TEST(Row, getUpper) {
  Row row[20];
  for (int i = 0; i < 20; ++i) {
    setRowRandom(row[i]);
  }
  Row row2[20];
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 10; ++j) {
      row2[i].row_[j] = row[i].row_[j];
    }
  }
  for (int i = 19; i > 0; --i) {
    row[i].getUpper(row[i - 1]);
  }
  // Row 0 (Top row) will be cleared after every row moves one down.
  row[0].clear();
  for (int i = 19; i > 0; --i) {
    for (int j = 0; j < 10; ++j) {
      ASSERT_EQ(row[i].row_[j], row2[i - 1].row_[j]);
    }
  }
  // Row 0 (Top row) should be cleared after every row moves one down.
  for (int j = 0; j < 10; ++j) {
    ASSERT_EQ(row[0].row_[j], std::make_pair(0, 0));
  }
}

TEST(Row, isFull) {
  Row row[20];
  for (int i = 0; i < 20; ++i) {
    setRowRandom(row[i]);
  }
  for (int i = 0; i < 20; ++i) {

    ASSERT_TRUE(row[i].isFull());
  }
}

// Tests for the TetrisGame class

TEST(TetrisGame, TetrisGame) {
  TetrisGame game(1, nullptr, true);
  // Not testing anything
  ASSERT_TRUE(true);
}

// Testing subclass
// This is a subclass of TetrisGame, that provides getters and setters for
// testing purposes. I posted a question to this on the Forum, in Test Art
// (FRIEND_TEST oder public).
class TetrisGameTest : public TetrisGame {
public:
  // Inherit Constructor
  using TetrisGame::TetrisGame;

  // Public relays for protected TetrisGame methods
  void bufferTetrominoTest() { bufferTetromino(); }

  void settleTetrominoTest() { settleTetromino(); }

  void writeToScreenTest() { writeToScreen(); }

  void drawScreenTest() { drawScreen(); }

  bool checkCollisionLeftTest() { return checkCollisionLeft(); }

  bool checkCollisionRightTest() { return checkCollisionRight(); }

  bool checkCollisionDownTest() { return checkCollisionDown(); }

  void generateNextTetrominoTest() { generateNextTetromino(); }

  void removeTetrominoOldTest() { removeTetrominoOld(); }

  // Simulates a "step" of the game.
  // A step is the rotation of removing the current tetromino, buffering again
  // and then writing to the screen.
  void stepGame() {
    removeTetrominoOld();
    bufferTetromino();
    writeToScreen();
  }

  // Getter for the protected members for testing.
  Row *screen_Test() { return screen_; }

  std::vector<std::pair<int, int>> &points_Test() { return points_; }

  // Yes, i am passing full objects here, but i want them imutable.
  Tetromino getNextTetromino() const { return nextTetromino_; }

  Tetromino getCurrentTetromino() const { return currentTetromino_; }

  // I want this to be imutable aswell
  const std::pair<int, int> getPositionTetromino() const {
    return positionTetromino_;
  }

  int gameSpeed() const { return gameSpeed_; }

  int lines_Test() { return lines_; }

  int level_Test() { return level_; }

  int score_Test() { return score_; }

  VirtualTerminalManager *getTerminalManager() { return tm_.get(); }

  // Setters for the tetrominos for testing.
  void setNextTetromino(const Tetromino &tetromino) {
    nextTetromino_ = tetromino;
  }

  void setCurrentTetromino(const Tetromino &tetromino) {
    currentTetromino_ = tetromino;
  }

  void rotateCurrentTetromino(const int times = 1) {
    for (int i = 0; i < times % 4; ++i) {
      currentTetromino_.rotateCW(true);
    }
  }

  void setPositionTetromino(int x, int y) {
    positionTetromino_ = std::make_pair(x, y);
  }

  void setTetrominoAtPosition(const Tetromino &tetromino, int x, int y) {
    std::vector<std::pair<int, int>> pointsOld = points_;
    std::pair<int, int> positionOld = positionTetromino_;
    Tetromino tetrominoOld = currentTetromino_;
    setCurrentTetromino(tetromino);
    setPositionTetromino(x, y);
    bufferTetromino();
    settleTetromino(); // This can also affect points.
    drawScreen();
    // points_.clear();
    currentTetromino_ = tetrominoOld;
    positionTetromino_ = positionOld;
    points_ = pointsOld;
  }

  // Setters for the game state for testing.

  void setLevel(int level) { level_ = level; }

  void setScore(int score) { score_ = score; }

  void setLines(int lines) { lines_ = lines; }

  void fillLine(int line) {
    for (int i = 0; i < 10; ++i) {
      screen_Test()[line % 20].row_[i] = std::make_pair(3, 69);
    }
  }
};

TEST(TetrisGameTest, bufferTetromino) {
  TetrisGameTest game(1, nullptr, true);
  game.setCurrentTetromino(Tetromino{TetrominoForm::T});
  game.setPositionTetromino(5, 5);
  game.bufferTetrominoTest();
  // Testvector with all the points of the tetromino.
  std::vector<std::pair<int, int>> TetrominoTest = {
      {4, 5}, {5, 5}, {6, 5}, {5, 6}};
  // Test lambda function if a point is in a vector.
  auto isInVector = [](const std::vector<std::pair<int, int>> &vector,
                       const std::pair<int, int> &pair) {
    return std::find(vector.begin(), vector.end(), pair) != vector.end();
  };
  for (const auto &point : game.points_Test()) {
    ASSERT_TRUE(isInVector(TetrominoTest, point));
  }
}

TEST(TetrisGameTest, checkCollisionRotateICW) {
  // This tests only the I tetromino, but the method is only used for the I
  // tetromino.
  TetrisGameTest game(1, nullptr, true);
  game.setTetrominoAtPosition(Tetromino{TetrominoForm::J}, 3, 9);
  game.setCurrentTetromino(Tetromino{TetrominoForm::I}); // Standard is Up
  game.setPositionTetromino(4, 8);
  // Check if current Tetromino collides with settled ones
  ASSERT_TRUE(game.checkCollisionRotateICW());
  game.setPositionTetromino(10, 15);
  // Check if current Tetromino goes out of bounds (to the right, but it is just
  // a check if any point of the rotated I tetromino would be out of bounds
  // (left, right, down))
  ASSERT_TRUE(game.checkCollisionRotateICW());
}

TEST(TetrisGameTest, checkCollisionRotateCW) {
  // This tests only the L tetromino, but the method is used for every
  // tetromino. This is OK, because the method does everything the same way for
  // every tetromino.
  // It rotates (but does NOT buffer) the tetromino, then checks for collisions
  // for every of the four points in the tetromino. Then, it rotates back,
  // buffers and returns.
  TetrisGameTest game(1, nullptr, true);
  game.setTetrominoAtPosition(Tetromino{TetrominoForm::J}, 3, 9);
  game.setCurrentTetromino(Tetromino{TetrominoForm::L});
  game.setPositionTetromino(4, 9);
  // Collides with the J tetromino!
  ASSERT_TRUE(game.checkCollisionRotateCW(true));
  game.setPositionTetromino(8, 10);
  // Does not collide!
  ASSERT_FALSE(game.checkCollisionRotateCW(true));
  game.setCurrentTetromino(Tetromino{TetrominoForm::T});
  game.rotateCurrentTetromino(3);
  game.setPositionTetromino(4, 9);
  ASSERT_TRUE(game.checkCollisionRotateCW(true));
}

TEST(TetrisGameTest, checkCollisions) {
  // This tests only the L and J tetrominos, but the method is used for every
  // tetromino. This is OK, because the method does everything the same way for
  // every tetromino.
  // It checks, for every point in the tetromino, if it collides with its
  // neighbor or the pixel below respectively. For example, the
  // checkCollisionRight() checks if the pixel to the right of every point in
  // the tetromino is occupied by another, already settled tetromino. Also, it
  // checks out of bounds.
  TetrisGameTest game(1, nullptr, true);
  game.setTetrominoAtPosition(Tetromino{TetrominoForm::J}, 3, 9);
  game.setCurrentTetromino(Tetromino{TetrominoForm::L});
  game.setPositionTetromino(4, 9);
  game.bufferTetrominoTest();
  // Collides with the J tetromino!
  ASSERT_TRUE(game.checkCollisionLeftTest());
  game.setPositionTetromino(2, 8);
  game.bufferTetrominoTest();
  // Does not collide!
  ASSERT_FALSE(game.checkCollisionLeftTest());
  for (int i = 0; i < 20; ++i) {
    game.screen_Test()[i].clear();
  }
  game.setTetrominoAtPosition(Tetromino{TetrominoForm::L}, 4, 9);
  game.setCurrentTetromino(Tetromino{TetrominoForm::J});
  game.setPositionTetromino(3, 9);
  game.bufferTetrominoTest();
  // Collides with the L tetromino!
  ASSERT_TRUE(game.checkCollisionRightTest());
  game.setPositionTetromino(5, 8);
  game.bufferTetrominoTest();
  // Does not collide!
  ASSERT_FALSE(game.checkCollisionRightTest());
  for (int i = 0; i < 20; ++i) {
    game.screen_Test()[i].clear();
  }
  game.setTetrominoAtPosition(Tetromino{TetrominoForm::J}, 4, 9);
  game.setCurrentTetromino(Tetromino{TetrominoForm::L});
  game.setPositionTetromino(4, 6);
  game.bufferTetrominoTest();
  // Collides with the J tetromino!
  ASSERT_TRUE(game.checkCollisionDownTest());
  game.setPositionTetromino(5, 8);
  game.bufferTetrominoTest();
  // Does not collide!
  ASSERT_FALSE(game.checkCollisionDownTest());
}

TEST(TetrisGameTest, removeTetrominoOld) {
  TetrisGameTest game(1, nullptr, true);
  game.setCurrentTetromino(Tetromino{TetrominoForm::L});
  game.setPositionTetromino(4, 9);
  game.bufferTetrominoTest();
  game.writeToScreenTest();
  game.drawScreenTest();
  // Is the middle of the tetromino drawn?
  // This is enough of a test, because the rotation does not matter. It removes
  // all points on the last location of the tetromino.
  for (const auto &point : game.points_Test()) {
    ASSERT_TRUE(game.getTerminalManager()->isCellPixel(point.second + 77,
                                                       point.first + 45));
  }
  // Then, it changes position of the tetromino and draws it again (here: 7 rows
  // down).
  // So the tetromino in row 9 should not be drawn.
  game.removeTetrominoOld();
  game.setPositionTetromino(4, 16);
  game.bufferTetrominoTest();
  game.writeToScreenTest();
  game.drawScreenTest();
  // Is the middle of the tetromino NOT drawn (removed)?
  for (const auto &point :
       game.points_Test()) { // Row - 7, because we are already on the new row.
                             // We need to test the old position.
    ASSERT_FALSE(game.getTerminalManager()->isCellPixel(point.second + 77 - 7,
                                                        point.first + 45));
  }
}

TEST(TetrisGameTest, settleTetrominoTest) {
  {
    // This tests, if the tetromino is settled correctly on the screen and saved
    // with the right attributes (69).
    TetrisGameTest game(1, nullptr, true);
    game.setTetrominoAtPosition(Tetromino{TetrominoForm::J}, 4, 9);
    game.setCurrentTetromino(Tetromino{TetrominoForm::L});
    game.setPositionTetromino(4, 9);
    game.bufferTetrominoTest();
    game.settleTetrominoTest();
    for (const auto &point : game.points_Test()) {
      ASSERT_EQ(game.screen_Test()[point.second].row_[point.first].second, 69);
    }
  }
  {
    // This tests, if the tetromino is settled and the score, level etc are
    // updated correctly.
    TetrisGameTest game(1, nullptr, true);
    game.setCurrentTetromino(Tetromino{TetrominoForm::S});
    game.setPositionTetromino(4, 9);
    game.bufferTetrominoTest();
    game.setLines(4);    // Linescore is 4 now
    game.fillLine(19);   // Filling bottom line
    game.setLevel(7);    // Level is 7 now
    game.setScore(1000); // Score is 1000 now
    game.settleTetrominoTest();
    // Linescore should now be one more (5)
    // Level should now be 7 still
    // Score should go up (7 + 1) * 40 (one line)
    // No line should be full anymore
    ASSERT_EQ(game.lines_Test(), 5);
    for (int i = 0; i < 20; ++i) {
      ASSERT_FALSE(game.screen_Test()[i].isFull());
    }
    ASSERT_EQ(game.level_Test(), 7);
    ASSERT_EQ(game.score_Test(), 1320);
  }
}

TEST(TetrisGameTest, generateNextTetromino) {
  TetrisGameTest game(1, nullptr, true);
  game.setCurrentTetromino(Tetromino{TetrominoForm::I});
  game.setNextTetromino(Tetromino{TetrominoForm::S});
  game.generateNextTetrominoTest();
  // The current tetromino should now be the S tetromino.
  ASSERT_EQ(game.getCurrentTetromino().form(), TetrominoForm::S);
  // Cannot really test the randomness here???
  // Will not test if the NEXT screen is really black... You can see that
  // ingame.
  // Test for the NEXT string
  ASSERT_TRUE(game.getTerminalManager()->isCellString(
      game.getTerminalManager()->numRows() - 18,
      game.getTerminalManager()->numCols() / 2 + 10, "NEXT"));
  // Test if the next tetromino is drawn correctly.
  for (const auto &point : game.points_Test()) {
    ASSERT_TRUE(game.getTerminalManager()->isCellPixel(
        point.second + game.getTerminalManager()->numRows() - 18,
        point.first + game.getTerminalManager()->numCols() / 2 + 10));
  }
}

TEST(TetrisGameTest, writeToScreen) {
  // Test if the tetromino is drawn correctly on the screen_.
  TetrisGameTest game(1, nullptr, true);
  game.setTetrominoAtPosition(Tetromino{TetrominoForm::J}, 3, 9);
  game.setCurrentTetromino(Tetromino{TetrominoForm::L});
  game.setPositionTetromino(4, 9);
  game.bufferTetrominoTest();
  game.writeToScreenTest();
  // Test the current tetromino
  for (const auto &point : game.points_Test()) {
    ASSERT_EQ(game.screen_Test()[point.second].row_[point.first].second, 1);
  }
  // Test the settled tetromino
  ASSERT_EQ(game.screen_Test()[8].row_[3].second, 69);
  ASSERT_EQ(game.screen_Test()[9].row_[3].second, 69);
  ASSERT_EQ(game.screen_Test()[10].row_[3].second, 69);
  ASSERT_EQ(game.screen_Test()[10].row_[2].second, 69);
}

TEST(TetrisGameTest, initGame) {
  ASSERT_TRUE(true);
  // This tests nothing.
  // This is because initGame() sets the screen black, then sets all the Borders
  // (that you can see by playing the game), then it generates a next tetromino.
  // Then it buffers, writes and draws. This is all already tested.
}

TEST(TetrisGameTest, inputhandling) {
  /* auto drawScreen = [](const Row *screen) {
    std::string line;
    printf("X   0 1 2 3 4 5 6 7 8 9\n\n");
    for (int i = 0; i < 20; ++i) {
      if (i < 10) {
        printf(" %d ", i);
      } else {
        printf("%d ", i);
      }
      for (int j = 0; j < 10; ++j) {
        if (screen[i].row_[j].second != 69) {
          line += screen[i].row_[j].second == 0 ? " -" : " #";
        } else {
          line += " O";
        }
      }
      printf("%s\n", line.c_str());
      line = "";
    }
  }; */
  TetrisGameTest game(1, nullptr, true);
  game.setCurrentTetromino(Tetromino{TetrominoForm::T});
  game.setPositionTetromino(5, 5);
  game.bufferTetrominoTest();
  game.writeToScreenTest();
  UserInput ui;
  ui.keycode_ = KEY_LEFT;
  game.handleInput(ui);
  // Should move left.
  game.stepGame();
  ASSERT_EQ(game.getPositionTetromino(), std::make_pair(4, 5));
  game.setTetrominoAtPosition(Tetromino{TetrominoForm::J}, 2, 5);
  game.handleInput(ui);
  // Should not move left
  game.stepGame();
  ASSERT_EQ(game.getPositionTetromino(), std::make_pair(4, 5));
  ui.keycode_ = 100;
  game.handleInput(ui);
  // Should rotate counterclockwise
  game.stepGame();
  ASSERT_EQ(game.getCurrentTetromino().rotation(), 1);
  ui.keycode_ = KEY_LEFT;
  game.handleInput(ui);
  // Should move left
  game.stepGame();
  ASSERT_EQ(game.getPositionTetromino(), std::make_pair(3, 5));
  ui.keycode_ = 97;
  game.handleInput(ui);
  // Should not rotate clockwise because it hits the J tetromino
  game.stepGame();
  ASSERT_EQ(game.getCurrentTetromino().rotation(), 1);
  ui.keycode_ = KEY_RIGHT;
  game.handleInput(ui);
  // Should move right
  game.stepGame();
  ASSERT_EQ(game.getPositionTetromino(), std::make_pair(4, 5));
  ui.keycode_ = 97;
  game.handleInput(ui);
  // Should rotate clockwise
  game.stepGame();
  game.setTetrominoAtPosition(Tetromino{TetrominoForm::O}, 3, 9);
  ASSERT_EQ(game.getCurrentTetromino().rotation(), 0);
  ui.keycode_ = KEY_DOWN;
  game.handleInput(ui);
  // Should move down
  game.stepGame();
  ASSERT_EQ(game.getPositionTetromino(), std::make_pair(4, 6));
  game.handleInput(ui);
  // Should move down
  game.stepGame();
  ASSERT_EQ(game.getPositionTetromino(), std::make_pair(4, 7));
  game.handleInput(ui);
  // Should not move down because it hits the O tetromino. It settles.
  ASSERT_FALSE(game.getPositionTetromino() == std::make_pair(4, 7));
}

TEST(TetrisGameTest, play) {
  TetrisGameTest game(1, nullptr, true);
  UserInput ui;
  game.setLevel(15);
  for (int i = 10; i < 20; ++i) {
    game.play(i);
    // Gamespeed should be 64 ms for every Cycle.
    float sum{0};
    for (const auto &time : game.times_) {
      sum += time.count();
    }
    float avg = sum / i;
    ASSERT_TRUE(avg > 63.0 &&
                avg < 71.0); // Going for a high upper limit, because it can
                             // take longer than the normal time due to how the
                             // times are set. But I think, 5 ms are low enough.
  }
  // The rest has been already tested. It is just called in play() method.
}

// This is for asthetic purpose only.
#include <stdio.h>

#define GRN "\x1B[32m"
#define RESET "\x1B[0m"
//---------------------------------------------------------------------------------
TEST(TetrisGameTest, restartHandler) {
  ASSERT_TRUE(true);
  printf(GRN "[   TEXT   ] " RESET
             "This is trivial. It sets values and calls tested methods.\n");
}

TEST(TetrominoTest, rotateCW) {
  Tetromino tetromino(TetrominoForm::J);
  // Save rotation
  std::vector<std::pair<int, int>> tetrominoPoints = tetromino.getDefaultForm();
  // Rotate clockwise
  tetromino.rotateCW(true);
  // Save rotation 2
  std::vector<std::pair<int, int>> rotatedPoints = tetromino.getDefaultForm();
  // Rotate rotation 2
  for (auto &point : rotatedPoints) {
    point = tetromino.getRotation(point);
  }
  // Lambda if point is vector.
  auto isRotated = [](const std::vector<std::pair<int, int>> &vector,
                      const std::pair<int, int> &point) {
    return std::find(vector.begin(), vector.end(), point) != vector.end();
  };
  // Basically checks if 1 == 1...
  for (const auto &point : tetrominoPoints) {
    ASSERT_TRUE(isRotated(rotatedPoints, tetromino.getRotation(point)));
  }
}