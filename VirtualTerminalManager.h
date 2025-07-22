// Copyright (C)

#pragma once

// Declaration and Implementation of a virtual base class TerminalManager

class UserInput {
public:
  // Functions that check for particular keys.
  bool isEscape() const;
  bool isKeyLeft() const;
  bool isKeyRight() const;
  bool isKeyUp() const;
  bool isKeyDown() const;
  bool isA() const;
  bool isD() const;
  bool isSpace() const;
  bool isMouseclick() const;
  // The code of the key that was pressed.
  int keycode_;
  int mouseRow_ = -1;
  int mouseCol_ = -1;
};

class VirtualTerminalManager {
public: // Everybody needs to use this......
  // Needs a virtual destructor to ensure proper cleanup.????
  // Yes..
  virtual ~VirtualTerminalManager(){};

  // Draw a Pixel at row, col with color. Virtual
  virtual void drawPixel(int row, int col, int color) = 0;

  // Draw a String str at row, col with color. Virtual
  virtual void drawString(int row, int col, int color, const char *str) = 0;

  // Draw a Score score at row, col with color. Virtual
  virtual void drawScore(int row, int col, int color, int score) = 0;

  // Getters for the screen Dimensions. Virtual
  virtual int numRows() const = 0;
  virtual int numCols() const = 0;

  // Switch waiting for a user input. Virtual
  virtual void flipDelay(bool to) = 0;

  // Get user input. Virtual
  virtual UserInput getUserInput() = 0;

  // For testing.......
  virtual bool isCellPixel(int row, int col) const = 0;
  virtual bool isCellString(int row, int col, const char *str) const = 0;

protected:
  int numRows_;
  int numCols_;
};
