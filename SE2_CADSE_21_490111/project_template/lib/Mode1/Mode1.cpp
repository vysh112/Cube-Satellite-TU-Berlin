/*This requirement describes the operation in mode 1.
 -A symbol representing the satellite is displayed on the OLED and will move or "fall" into the direction of the main gravitational force.
 -Additionally, the satellite has a physically plausible movement like acceleration and is shown as ASCII-Art or as an bitmap image of a satellite instead of a symbol.*/

/**
 * @file Mode1.cpp
 * @author Vasanth
 * @brief Mode 1 Function This requirement describes the operation in mode 1.
 -A symbol representing the satellite is displayed on the OLED and will move or "fall" into the direction of the main gravitational force.
 -Additionally, the satellite has a physically plausible movement like acceleration and is shown as ASCII-Art or as an bitmap image of a satellite instead of a symbol.s
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Mode1.h"

#include <Arduino.h>
#include <Utils.h>

void mode1(){
  delay(100);
  if( modeChanged() ){ 
    return;
  }

  // Simulated accelerometer values (replace with actual readings)

  // Map accelerometer values to OLED display coordinates
  int x = map(accelX, -10, 10, 5, SCREEN_WIDTH - 6);
  int y = map(accelY, -10, 10, 5, SCREEN_HEIGHT - 6);

  // Clear previous display
  display.clearDisplay();

  // Draw satellite symbol (circle) at mapped coordinates
   display.drawCircle(x, y, 6, SSD1306_WHITE);
   display.fillCircle(x, y, 4, SSD1306_WHITE);

  // Draw parallelogram
   display.drawLine(x - 9, y, x - 3, y - 6, SSD1306_WHITE);
   display.drawLine(x - 3, y - 6, x + 3, y - 6, SSD1306_WHITE);
   display.drawLine(x + 3, y - 6, x + 9, y, SSD1306_WHITE);
   display.drawLine(x + 9, y, x - 9, y, SSD1306_WHITE);

  // Draw triangle
   display.drawLine(x - 6, y + 5, x, y - 6, SSD1306_WHITE);
   display.drawLine(x, y - 6, x + 6, y + 5, SSD1306_WHITE);
   display.drawLine(x + 6, y + 5, x - 6, y + 5, SSD1306_WHITE);

  // Draw antennas
   display.drawLine(x - 9, y + 6, x - 12, y + 12, SSD1306_WHITE);
   display.drawLine(x + 9, y + 6, x + 12, y + 12, SSD1306_WHITE);

    // Display the drawing
    display.display();
}

//End of M1:)