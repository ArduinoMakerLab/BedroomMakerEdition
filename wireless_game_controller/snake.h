#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>

// game area
#define GAME_WIDTH    31
#define GAME_HEIGHT   11

enum Direction { UP, DOWN, LEFT, RIGHT };

// position strcuture
struct Point {
  int x;
  int y;
  Point(int x_ = 0, int y_ = 0) : x(x_), y(y_) {}
};

class Snake {
private:
  Point body[500];
  int length;
  Direction dir;
  Point food;
  bool pauseGame;
  unsigned long lastMoveTime = 0;
  int moveInterval; // moving interval

  // function
  void generateFood();
  bool move();

public:
  Snake();

  uint32_t score;

  Point getFood() const;
  Point getBody(int index) const;
  int getLength() const;

  void reset();
  void changeDirection(Direction newDir);
  void pause(bool state)
  {
    pauseGame = state;
  }

  void run(void);
};

#endif // SNAKE_H