/* This requirement describes the operation in mode 3.
 -A timer have been implemented. The remaining time will be displayed with the servo (+90° equals 100% time left, -90° equals time is over). 
 -The remaining time is also displayed on the OLED screen. When the time is over, a "MARIO" melody will be played. 
 -The time in seconds can be set via the up and down capacitive touch buttons. To start the timer the “x” button can be used.*/
 /**
 * @file Mode3.cpp
 * @author Vasanth
 * @brief Mode 3 Function -A timer have been implemented. The remaining time will be displayed with the servo (+90° equals 100% time left, -90° equals time is over). 
 -The remaining time is also displayed on the OLED screen. When the time is over, a "MARIO" melody will be played. 
 -The time in seconds can be set via the up and down capacitive touch buttons. To start the timer the “x” button can be used.
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Mode3.h"

#include <Arduino.h>
#include <Utils.h>

void mode3(){
  delay(100);
  if( modeChanged() ){
    return;
  }
  if (touchRead(touchUpPin) > touchThreshold) {
    setTime += 10; // Increase time by 10 seconds
    displayTimeSet();
    delay(500); // Debounce delay
  }

  if (touchRead(touchDownPin) > touchThreshold && setTime > 0) {
    setTime -= 10; // Decrease time by 10 seconds
    displayTimeSet();
    delay(500); // Debounce delay
  }

  if (touchRead(touchXPin) > touchThreshold) {
    display.clearDisplay();
    display.display();
    myservo.attach(7);
    moveServo(setTime);
    delay(200);
  }
}

//End of M3:)