/**
 * @file Mode0.cpp
 * @author Vasanth
 * @brief Mode 0 Function
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Mode0.h"

#include <Arduino.h>
#include <Utils.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

void mode0(){
  delay(100);
  if( modeChanged() ){
    return;
  }
}