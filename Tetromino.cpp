// Copyright (C)

#include "./Tetromino.h"
#include <regex>
#include <utility>

// Implementation of Tetromino class

Tetromino::Tetromino(TetrominoForm form) : form_(form) {
  // Initialize rotation map for clockwise rotation
  rotationMapCW_[std::make_pair(-1, -1)] = std::make_pair(1, -1);
  rotationMapCW_[std::make_pair(0, -1)] = std::make_pair(1, 0);
  rotationMapCW_[std::make_pair(1, -1)] = std::make_pair(1, 1);
  rotationMapCW_[std::make_pair(-1, 0)] = std::make_pair(0, -1);
  rotationMapCW_[std::make_pair(1, 0)] = std::make_pair(0, 1);
  rotationMapCW_[std::make_pair(-1, 1)] = std::make_pair(-1, -1);
  rotationMapCW_[std::make_pair(0, 1)] = std::make_pair(-1, 0);
  rotationMapCW_[std::make_pair(1, 1)] = std::make_pair(-1, 1);
  rotationMapCW_[std::make_pair(0, 0)] = std::make_pair(0, 0);

  rotationI_.first = {std::make_pair(0, -2), std::make_pair(0, 1),
                      std::make_pair(0, 0), std::make_pair(0, -1)};
  rotationI_.second = {std::make_pair(-2, 0), std::make_pair(-1, 0),
                       std::make_pair(0, 0), std::make_pair(1, 0)};

  // Initialize default forms
  defaultForms_[TetrominoForm::I] = rotationI_.first;
  defaultForms_[TetrominoForm::J] = {{-1, 1}, {0, 0}, {0, 1}, {0, -1}};
  defaultForms_[TetrominoForm::L] = {{1, 1}, {0, 0}, {0, 1}, {0, -1}};
  defaultForms_[TetrominoForm::O] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  defaultForms_[TetrominoForm::S] = {{-1, 1}, {0, 0}, {0, 1}, {1, 0}};
  defaultForms_[TetrominoForm::T] = {{0, 1}, {0, 0}, {-1, 0}, {1, 0}};
  defaultForms_[TetrominoForm::Z] = {{1, 1}, {0, 0}, {0, 1}, {-1, 0}};
}

void Tetromino::rotateCW(bool direction) {
  if (!direction) {
    rotation_ =
        static_cast<TetrominoRotation>((static_cast<int>(rotation_) + 1) % 4);
  } else {
    rotation_ =
        static_cast<TetrominoRotation>((static_cast<int>(rotation_) + 3) % 4);
  }
}