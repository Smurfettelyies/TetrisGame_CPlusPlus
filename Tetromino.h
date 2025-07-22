// Copyright (C)

#pragma once

#include <algorithm>
#include <map>
#include <stdexcept>
#include <vector>

// Declaration of Tetromino class

enum class TetrominoForm {
  L = 0,
  J = 1,
  Z = 2,
  S = 3,
  T = 4,
  I = 5,
  O = 6,
  N = 7
};

enum TetrominoRotation { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

class Tetromino {
public:
  // Constructor
  Tetromino(TetrominoForm form = TetrominoForm::N);

  void operator=(TetrominoForm form) { form_ = form; }

  // Rotate the Tetromino Clockwise if direction == TRUE, otherwise
  // Counterclockwise
  void rotateCW(bool direction);

  // Getters
  TetrominoForm form() const { return form_; }
  TetrominoRotation rotation() const { return rotation_; }

  std::pair<int, int> getRotation(std::pair<int, int> coordinate) {
    return rotationMapCW_[coordinate];
  }

  std::vector<std::pair<int, int>> getDefaultForm() {
    return defaultForms_[form_];
  }

  std::vector<std::pair<int, int>> getIRotation(bool up) {
    if (up) {
      return rotationI_.first;
    } else {
      return rotationI_.second;
    }
  }

private:
  // Form of the Tetromino
  TetrominoForm form_;

  // Rotation of the Tetromino
  TetrominoRotation rotation_ = NORTH;

  // Map for the rotations in clockwise direction
  std::map<std::pair<int, int>, std::pair<int, int>> rotationMapCW_;
  std::pair<std::vector<std::pair<int, int>>, std::vector<std::pair<int, int>>>
      rotationI_;

  // Map for the forms
  std::map<TetrominoForm, std::vector<std::pair<int, int>>> defaultForms_;
};
