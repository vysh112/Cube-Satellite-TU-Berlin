/*This requirement describes the operation in mode 5. 
PAYLOAD DEPLOYMENt, It will be explained in detail during the presentation*/
/**
 * @file Mode5.cpp
 * @author Vasanth
 * @brief Mode 5 Function This requirement describes the operation in mode 5. 
PAYLOAD DEPLOYMENt, It will be explained in detail during the presentation
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Mode5.h"

#include <Arduino.h>
#include <Utils.h>
#include <Melodies.h>

void mode5(){
  delay(100);
  if( modeChanged() ){
    return;
  }
  int sensorValue5 = analogRead(LDR_PIN);
    temperature = bme.readTemperature();
    pressure = bme.readPressure() / 100.0F;
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
            myservo.attach(7);
    bool conditionsSatisfied = (temperature >= 15) && (altitude < 250) && (pressure < 1036.25);

    if (conditionsSatisfied) {
        digitalWrite(ledGreenPin, HIGH);
        digitalWrite(ledYellowPin, HIGH);
        digitalWrite(ledRedPin, HIGH);
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("PAYLOAD DEPLOYED!!   CONDITIONS SATISFIED");
        display.display();
        for (int angle = 0; angle <= 180; angle++) {
            myservo.write(angle); // Move to the specified angle
            delay(15); // Adjust delay as needed for smoother motion
        }
        myservo.write(0);
        servoposition=0;
        delay(1000);
    } else {
        digitalWrite(ledGreenPin, LOW);
        digitalWrite(ledYellowPin, LOW);
        digitalWrite(ledRedPin, LOW);
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Conditions unsatsfied:");
        display.setCursor(0, 10);
        if (temperature < 30) {
            display.println("Temperature < 15C"); //type thresholds for display
        }
        if (altitude >= 250) {
            display.println("Altitude >= 250m");
        }
        if (pressure >= 1013.25) {
            display.println("Pressure >= 1036.25hPa");
        }
        display.display();
    }
}

//End of M5:)