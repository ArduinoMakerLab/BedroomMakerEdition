#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <IRremote.hpp>

#include "snake.h"
#include "logo.h"

// OLED prameter
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 
#define SCREEN_ADDRESS 0x3C 

// game parameter
#define LED_PIN    48
#define LED_COUNT  1

#define GRID_SIZE     4
#define ORIGIN_X  0
#define ORIGIN_Y  16
#define BORDER_SIZE (GRID_SIZE/2)

// object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
Snake snake;

// variable
volatile bool up,down,left,right,ok,pound;

void setup() {
  // init serial log
  Serial.begin(115200);
  Serial.println("ESP32-S3 Is Ready");
  Serial.println("******************");
  Serial.println("Bedroom Maker Edition - 01");
  Serial.println("wireless game controler");

  // init IIC
  Wire.begin(6, 7);  // SDA=GPIO6, SCL=GPIO7

  // init SSD1306
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;)
    {
      Serial.println(F("SSD1306 error"));
      delay(500);
    }
  }
  Serial.println("OLED init cplt");
  display.clearDisplay();
  display.drawBitmap(32, 0 ,(const uint8_t *)logo, 64, 64, WHITE);
  display.display();

  // turn off the on-board LED
  strip.begin();           
  strip.show();            
  strip.setBrightness(50);

  // init HX1838
  IrReceiver.begin(8, DISABLE_LED_FEEDBACK);

  // init game
  snake.reset();

  delay(5000);
}

// draw game pixel
void drawGame(Snake& snake) {
  display.clearDisplay();
  
  // update score
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.printf("Score:%04d", snake.score);

  // draw body
  for (int i = 0; i < snake.getLength(); i++) {
    Point p = snake.getBody(i);
    int x = p.x * GRID_SIZE;
    int y = p.y * GRID_SIZE;
    display.fillRect(x + ORIGIN_X + BORDER_SIZE, y + ORIGIN_Y + BORDER_SIZE, GRID_SIZE, GRID_SIZE, WHITE);
  }

  // draw food
  Point food = snake.getFood();
  int foodX = food.x * GRID_SIZE;
  int foodY = food.y * GRID_SIZE;
  display.fillRect(foodX + ORIGIN_X + BORDER_SIZE, foodY + ORIGIN_Y + BORDER_SIZE, GRID_SIZE, GRID_SIZE, WHITE);  //
  display.drawPixel(foodX + ORIGIN_X + BORDER_SIZE, foodY + ORIGIN_Y + BORDER_SIZE, BLACK);
  display.drawPixel(foodX + ORIGIN_X + BORDER_SIZE + (GRID_SIZE - 1), foodY + ORIGIN_Y + BORDER_SIZE, BLACK);
  display.drawPixel(foodX + ORIGIN_X + BORDER_SIZE, foodY + ORIGIN_Y + GRID_SIZE + (GRID_SIZE - 1), BLACK);
  display.drawPixel(foodX + ORIGIN_X + BORDER_SIZE + (GRID_SIZE - 1), foodY + ORIGIN_Y + BORDER_SIZE + (GRID_SIZE - 1), BLACK);

  // draw border
  display.drawRect(ORIGIN_X, ORIGIN_Y, ((GAME_WIDTH+1) * GRID_SIZE), ((GAME_HEIGHT+1) * GRID_SIZE), WHITE);

  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
            // We have an unknown protocol here, print extended info
            //IrReceiver.printIRResultRawFormatted(&Serial, true);
            IrReceiver.resume(); // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
        } else {
            IrReceiver.resume(); // Early enable receiving of the next IR frame
            //IrReceiver.printIRResultShort(&Serial);
            //IrReceiver.printIRSendUsage(&Serial);
            if (IrReceiver.decodedIRData.command == 0x18) {
                up = true;
            } else if (IrReceiver.decodedIRData.command == 0x52) {
                down = true;
            } else if (IrReceiver.decodedIRData.command == 0x08) {
                left = true;
            } else if (IrReceiver.decodedIRData.command == 0x5A) {
                right = true;
            } else if (IrReceiver.decodedIRData.command == 0x1C) {
                ok = true;
            } else if (IrReceiver.decodedIRData.command == 0x0D) {
                pound = true;
            }
        }
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
            //Serial.println(F("Repeat received. Here you can repeat the same action as before."));
        } else {
            if (IrReceiver.decodedIRData.command == 0x18) {
                up = true;
            } else if (IrReceiver.decodedIRData.command == 0x52) {
                down = true;
            } else if (IrReceiver.decodedIRData.command == 0x08) {
                left = true;
            } else if (IrReceiver.decodedIRData.command == 0x5A) {
                right = true;
            } else if (IrReceiver.decodedIRData.command == 0x1C) {
                ok = true;
            } else if (IrReceiver.decodedIRData.command == 0x0D) {
                pound = true;
            }
        }
        //Serial.printf("up %d, down %d, left %d, right %d, ok %d, pound %d", up, down, left, right, ok, pound);
    }

  // handle the pressed button
  if (up) {
    snake.changeDirection(UP);
    snake.pause(false);
    //delay(100);
  } else if (down) {
    snake.changeDirection(DOWN);
    snake.pause(false);
    //delay(100);
  } else if (left) {
    snake.changeDirection(LEFT);
    snake.pause(false);
    //delay(100);
  } else if (right) {
    snake.changeDirection(RIGHT);
    snake.pause(false);
    //delay(100);
  }
  up = false;
  down = false;
  left = false;
  right = false;
  ok = false;
  pound = false;

  // update game data
  snake.run();

  // draw the new frame
  drawGame(snake);
}
