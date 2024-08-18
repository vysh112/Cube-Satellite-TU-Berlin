/*This requirement describes the operation in mode 2.
 -The satellite have been integrated with a simple alarm system for critical errors: 
 -The temperature of the environmental sensor above or below a certain threshold (<15°C & >45°C) triggers the buzzer as well as the OLED blinking Morse-coded “SOS”, 
  stopping the alarm within the thresholds again. The thresholds can be changed by the controller via telecommand. Thresholds and level is also displayed shown on the OLED screen.
 -Additionally, an encoder has been Implemented for the Morse-alphabet to transmit data visually.*/
/**
 * @file Mode2.cpp
 * @author Vasanth
 * @brief Mode 2 Function This requirement describes the operation in mode 2.
 -The satellite have been integrated with a simple alarm system for critical errors: 
 -The temperature of the environmental sensor above or below a certain threshold (<15°C & >45°C) triggers the buzzer as well as the OLED blinking Morse-coded “SOS”, 
  stopping the alarm within the thresholds again. The thresholds can be changed by the controller via telecommand. Thresholds and level is also displayed shown on the OLED screen.
 -Additionally, an encoder has been Implemented for the Morse-alphabet to transmit data visually.
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Mode2.h"

#include <Arduino.h>
#include <Utils.h>

void mode2(){
  delay(100);
  if( modeChanged() ){
    return;
  }
  temperature = bme.readTemperature();

  // Check if temperature is above or below thresholds
  if (temperature > TEMPERATURE_THRESHOLD_HIGH || temperature < TEMPERATURE_THRESHOLD_LOW) {
    // Activate alarm
    activateAlarm();
  } else {
    // Deactivate alarm
    deactivateAlarm();
  }

  // Display temperature and alarm status on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.println(temperature, 1);
  display.print("Alarm: ");
  display.println(alarmActive ? "Active " : "Inactive");
  display.println("Thresholds: " + String(TEMPERATURE_THRESHOLD_LOW) + " to " + String(TEMPERATURE_THRESHOLD_HIGH));

  display.display();

  delay(1000);
} 

//End of M2:)