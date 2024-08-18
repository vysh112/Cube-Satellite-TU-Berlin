/*This requirement describes the operation in mode 4.
 The light intensity will be displayed with the LEDs in a bar graph representation. Thereby, non illuminated LEDs represent a dark environment 
and all LEDs powered represent a lit environment. The thresholds for the transitions between the different LED states can be changed by the controller 
via telecommand and are stored in persistant memory.*/
/**
 * @file Mode4.cpp
 * @author Vasanth
 * @brief Mode 4 Function This requirement describes the operation in mode 4.
 The light intensity will be displayed with the LEDs in a bar graph representation. Thereby, non illuminated LEDs represent a dark environment 
and all LEDs powered represent a lit environment. The thresholds for the transitions between the different LED states can be changed by the controller 
via telecommand and are stored in persistant memory.
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Mode4.h"

#include <Arduino.h>
#include <Utils.h>

void mode4(){
  delay(100);
  if( modeChanged() ){
    return;
  }

   int sensorValue1 = analogRead(LDR_PIN);
  
  // Map the ADC value to LED states
  if (sensorValue1 <= dim_environment_adc) {
    // Dark environment - turn off all LEDs
    digitalWrite(ledGreenPin, LOW);
    digitalWrite(ledYellowPin, LOW);
    digitalWrite(ledRedPin, LOW);
  } else if (sensorValue1 <= medium_environment_adc) {
    // Dim light environment - turn on one LED
    digitalWrite(ledGreenPin, HIGH);
    digitalWrite(ledYellowPin, LOW);
    digitalWrite(ledRedPin, LOW);
  } else if (sensorValue1 <= bright_environment_adc) {
    // Medium light environment - turn on two LEDs
    digitalWrite(ledGreenPin, HIGH);
    digitalWrite(ledYellowPin, HIGH);
    digitalWrite(ledRedPin, LOW);
  } else if (sensorValue1 > bright_environment_adc) {
    // Bright environment - turn on all LEDs
    digitalWrite(ledGreenPin, HIGH);
    digitalWrite(ledYellowPin, HIGH);
    digitalWrite(ledRedPin, HIGH);
  }

  // Delay between readings
  delay(50);
}

//End of M4:)