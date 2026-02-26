#include <arduino.h>
#include "Snake.h"

// generate new food
void Snake::generateFood() {
  bool onSnake;
  do {
    onSnake = false;
    food.x = random(0, GAME_WIDTH);
    food.y = random(0, GAME_HEIGHT);
    
    // generate check, cannot be generated on own body
    for (int i = 0; i < length; i++) {
      if (body[i].x == food.x && body[i].y == food.y) {
        onSnake = true;
        break;
      }
    }
  } while (onSnake);
  //Serial.printf("gen food %d %d", food.x, food.y);
}

Snake::Snake() {
  pauseGame = true;
  moveInterval = 300;
  reset();
}

// reset the game
void Snake::reset() {
  length = 3;
  body[0] = Point(GAME_WIDTH/2, GAME_HEIGHT/2);
  body[1] = Point(GAME_WIDTH/2 - 1, GAME_HEIGHT/2);
  body[2] = Point(GAME_WIDTH/2 - 2, GAME_HEIGHT/2);
  dir = RIGHT;
  generateFood();
  score = 0;
}

// change new direction
void Snake::changeDirection(Direction newDir) {
  if ((dir == UP && newDir != DOWN) ||
      (dir == DOWN && newDir != UP) ||
      (dir == LEFT && newDir != RIGHT) ||
      (dir == RIGHT && newDir != LEFT)) {
    dir = newDir;
  }
}

// update the game, move body to a new position
bool Snake::move() {
  Point oldHead = body[0];
  Point oldTail = body[length -1];

  switch (dir) {
    case UP:    body[0].y--; break;
    case DOWN:  body[0].y++; break;
    case LEFT:  body[0].x--; break;
    case RIGHT: body[0].x++; break;
  }

  // rebuild body
  body[1]=oldHead;
  for (int i = length - 1; i > 1; i--) {
    body[i] = body[i - 1];
  }

  // boundary detection
  if (body[0].x < 0 || body[0].x >= GAME_WIDTH ||
      body[0].y < 0 || body[0].y >= GAME_HEIGHT) {
    return false;
  }

  // body detection
  for (int i = 1; i < length; i++) {
    if (body[0].x == body[i].x && body[0].y == body[i].y) {
      return false;
    }
  }

  // food detection
  if (body[0].x == food.x && body[0].y == food.y) {
    body[length].x = oldTail.x;
    body[length].y = oldTail.y;
    length++;
    generateFood();
    score++;
  } else {
    for (int i = length - 1; i > 0; i--) {
      body[i] = body[i - 1];
    }
  }

  return true;
}

// obtain the body position
Point Snake::getBody(int index) const {
  return body[index];
}

// obtain the length of the snake's body
int Snake::getLength() const {
  return length;
}

// obtain the food location
Point Snake::getFood() const {
  return food;
}

// update game data
void Snake::run(void)
{
  // moving snake
  unsigned long currentTime = millis();
  if (currentTime - lastMoveTime >= moveInterval) {
    if(!pauseGame)
    {
      if (!move()) {
        reset(); // game over, reset
        pauseGame = true;
        delay(1000);
      }
    }
    lastMoveTime = currentTime;
  }
}