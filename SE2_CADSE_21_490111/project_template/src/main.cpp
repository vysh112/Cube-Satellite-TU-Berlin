/**
 * @file main.cpp
 * @author Vasanth K
 * @brief This file is the Main.cpp which is the central code which controls the major flow of the Satellite's functions.
 * @version 0.1
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//DEFAULT LIBRARIES
#include <Arduino.h>
#include <Utils.h>
#include <Mode0.h>
#include <Mode1.h>
#include <Mode2.h>
#include <Mode3.h>
#include <Mode4.h>
#include <Mode5.h>

//LIBRARIES FOR OLED SCREEN
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//LIBRARIES FOR ENVIRONMENTAL SENSOR
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//LIBRARIES FOR IMU SENSORS
#include <Adafruit_LSM6DSO32.h> 


//LIBRARIES FOR MQTT SYSTEM
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>  
#include <RadioLib.h>
#include <ArduinoJson.h>

// LIBRARIES FOR SERVO UNIT
#include <ESP32Servo.h> 

//LIBRARIES FOR MUSIC PLAY
#include <Melodies.h>

//LIBRARIES FOR PRESISTENT MEMORY 
#include <EEPROM.h>


int defaultMode = 0; // Global variable to store the default mode
int EEPROMAddress = 0;

//Function to call modes
void DisplayMode(bool mode0Triggered,bool mode1Triggered,bool mode2Triggered,bool mode3Triggered,bool mode4Triggered,bool mode5Triggered);

//function to declare (X)mode to  persistent memory.
void saveDefaultMode() {
  EEPROM.put(EEPROMAddress, defaultMode);
  EEPROM.commit();
  Serial.print("Default mode saved to EEPROM: ");
  Serial.println(defaultMode);
}

void loadDefaultMode() {
  EEPROM.get(EEPROMAddress, defaultMode);
  Serial.print("Default mode loaded from EEPROM: ");
  Serial.println(defaultMode);
}
/**
 * @brief Callback Function for received messages on "broadcast" topic
 * @param topic
 * @param payload
 * @param int
 * @return (void)
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Parse JSON payload
  StaticJsonDocument<2000> doc; // Adjust the size according to the JSON payload
  DeserializationError error = deserializeJson(doc, payload, length);
  
  // Check for parsing errors
  if (error) {
    Serial.print("Parsing failed: ");
    Serial.println(error.c_str());
    return;
  }
  
  // Extract values from JSON
  const char* type = doc["type"];
  int cadse_id = doc["cadse_id"];
  int modeset = doc["mode"];
  
  // Check if the type is telecommand
  if (strcmp(type, "telecommand") == 0) {
    // Check if the cadse_id matches and the satellite is in the correct mode
    if (cadse_id == 21) {
      // Call mode-specific functions based on mode
      if (doc["data"].containsKey("defaultmode")) {
        // Set defaultMode to the value of "defaultmode" in the JSON
        defaultMode = doc["data"]["defaultmode"];
        Serial.print("Default mode set to: ");
        Serial.println(defaultMode);
        saveDefaultMode();
      } else {
        Serial.println("No 'defaultmode' found in JSON data");
      }

      if (doc["data"].containsKey("TEMP_LOW_THRESHOLD")) {
        TEMPERATURE_THRESHOLD_LOW = doc["data"]["TEMP_LOW_THRESHOLD"];
        Serial.print("TEMP LOW THRESHOLD set to: ");
        Serial.println(TEMPERATURE_THRESHOLD_LOW);
      }
      if (doc["data"].containsKey("TEMP_HIGH_THRESHOLD")) {
        TEMPERATURE_THRESHOLD_HIGH = doc["data"]["TEMP_HIGH_THRESHOLD"];
        Serial.print("TEMP HIGH THRESHOLD set to: ");
        Serial.println(TEMPERATURE_THRESHOLD_HIGH);
      }
      if (doc["data"].containsKey("dim_environment")) {
        dim_environment_adc = doc["data"]["dim_environment"];
        Serial.print("Dim Environment ADC Value THRESHOLD set to: ");
        Serial.println(dim_environment_adc);
      }
      if (doc["data"].containsKey("medium_environment")) {
        medium_environment_adc = doc["data"]["medium_environment"];
        Serial.print("Medium Environment ADC Value THRESHOLD set to: ");
        Serial.println(medium_environment_adc);
      }
      if (doc["data"].containsKey("bright_environment")) {
        bright_environment_adc = doc["data"]["bright_environment"];
        Serial.print("Bright Environment ADC Value THRESHOLD set to: ");
        Serial.println(bright_environment_adc);
      }
      switch (modeset) {
        case 0:
          nextMode=0;
          break;
        case 1:
          nextMode=1;
          break;
        case 2:
          nextMode=2;
          break;
        case 3:
          nextMode=3;
          break;
        case 4:
          nextMode=4;
          break;
        case 5:
          nextMode=5;
          break;
        default:
          Serial.println("Invalid mode");
      }
    } else {
      Serial.println("Invalid cadse_id or mode");
    }
  } else {
    Serial.println("Unsupported type");
  }
}

/**
 * @brief Setup function for initializing pins and sensor, motor initialization
 * @return (void)
 */
void setup() {
  // Disable PSRAM since it's not needed for this project
  // Pins 35-37 used for LEDs are connected to the internal octal SPI (OSPI) interface, which we disable by setting the chip select pin on GPIO 26.
  // This prevents side effects with other libraries that might cause odd behavior.
  startTime = millis();
  pinMode(26, OUTPUT); // PSRAM chip select pin
  pinMode(38, OUTPUT); // LoRa module
  pinMode(10, OUTPUT); // IMU
  digitalWrite(26, HIGH); // Disable PSRAM

  // Start serial communication
  Serial.begin(115200);
  delay(3000); // Delay for stability
  Serial.print("Starting up ...");
  
  // Initialize touch buttons
  initializeTouchbuttons(); 
  
  // Initialize GPIOs
  initializeGPIOs();
  Serial.println(" Ready :)");

  // Wait until serial connection is established
  //while (!Serial); 

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize environmental sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Initialize IMU
  Serial.println("Adafruit LSM6DSO32 test!");
  //while (!Serial)
    //delay(10); // Wait until serial console opens
  if (!dso32.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
    Serial.println("Failed to find LSM6DSO32 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("LSM6DSO32 Found!");
  dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);

  // Initialize MQTT
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  espClient.setInsecure();

  // Initialize servo
  myservo.attach(7); 
  myservo.write(-90);
  EEPROM.begin(sizeof(defaultMode));
  loadDefaultMode();
}



/**
 * @brief Main loop which consists of transmitting Housekeeping data and controlling Mode Switching
 * @return (void)
 */
void loop() {
  // Read WiFi signal strength
  float rssi = WiFi.RSSI();
  Serial.print("WiFi RSSI: ");
  Serial.print(rssi, 1);
  Serial.println(" db");

  // Initialize housekeeping data string
  housekeepingData = "";

  // Turn off all LEDs
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledYellowPin, LOW);
  digitalWrite(ledRedPin, LOW);

  // Check if mode needs to be switched
  if (nextMode != currentMode) {

    // Switching to new mode
    if (nextMode > currentMode && nextMode < 6) {
      digitalWrite(ledGreenPin, HIGH);
      delay(300);
      digitalWrite(ledGreenPin, LOW);
    } else if (nextMode < currentMode && nextMode >= 0) {
      digitalWrite(ledYellowPin, HIGH);
      delay(300);
      digitalWrite(ledYellowPin, LOW);
    } else if (nextMode >= 6 || nextMode <= -1) {
      // Indicate error with red LED flashing
      for (int i = 0; i < 5; i++) {
        digitalWrite(ledRedPin, HIGH);
        delay(100);
        digitalWrite(ledRedPin, LOW);
        delay(100);
      }
    }
    currentMode = nextMode;

    // Buzzer setup for current mode
    if (currentMode <= 5) {
      for (int i = 0; i < currentMode; i++) {
        ledcSetup(channelBuzzer, 1000, 12);
        ledcAttachPin(buzzerPin, channelBuzzer);
        ledcWrite(channelBuzzer, 99);
        delay(99);
        ledcWrite(channelBuzzer, 0);
        delay(99);
      }
    }

    // Trigger mode setup
    modeStartup(currentMode);
  }

  // Add current mode and WiFi RSSI to housekeeping data
  housekeepingData += "Mode: ";
  housekeepingData += String(currentMode);
  housekeepingData += '\n';
  housekeepingData += "WiFi RSSI: ";
  housekeepingData += String(rssi);
  housekeepingData += " db\n";

  // Update mode trigger flags
  mode0Triggered = (currentMode == 0);
  mode1Triggered = (currentMode == 1);
  mode2Triggered = (currentMode == 2);
  mode3Triggered = (currentMode == 3);
  mode4Triggered = (currentMode == 4);
  mode5Triggered = (currentMode == 5);
  //ENV SENS LOOP SETUP START
    {
      delay(500);

      Serial.print("Temperature: ");
      Serial.print(bme.readTemperature(), 1);
      Serial.println(" *C");

      Serial.print("Humidity: ");
      Serial.print(bme.readHumidity(), 1);
      Serial.println(" %");

      Serial.print("Pressure: ");
      Serial.print(bme.readPressure() / 100.0F, 1);
      Serial.println(" hPa");

      Serial.print("Approx. Altitude: ");
      Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA), 1);
      Serial.println(" m");

      pressure=bme.readPressure() / 100.0F;

      altitude= bme.readAltitude(SEALEVELPRESSURE_HPA);

      housekeepingData += "T:";
      housekeepingData += String(bme.readTemperature(), 1);
      housekeepingData += " *C\n";

      housekeepingData += "H:";
      housekeepingData += String(bme.readHumidity(), 1);
      housekeepingData += " %\n";

      housekeepingData += "P:";
      housekeepingData += String(bme.readPressure() / 100.0F, 1);
      housekeepingData += " hPa\n";

      housekeepingData += "A:";
      housekeepingData += String(bme.readAltitude(SEALEVELPRESSURE_HPA), 1);
      housekeepingData += " m\n"; 


      

      Serial.println();
  }
  //ENV SENS LOOP SETUP END

  
  // LDR LOOP SETUP START
    {
     int sensorValue = analogRead(LDR_PIN);
     float voltage = sensorValue * (3.3 / 4095.0); // Convert analog reading to voltage

     Serial.println("LDR Sensor Reading:");
     Serial.print("ADC Value: ");
     Serial.print(sensorValue);
     Serial.print(", Voltage: ");
     Serial.print(voltage, 2); // Print voltage with 2 decimal places
     Serial.println(" V");

     Serial.println();

     housekeepingData += "ADC ";
     housekeepingData += String(sensorValue);
     housekeepingData += '\n';
     housekeepingData += "Voltage ";
     housekeepingData += String(voltage, 2);
     housekeepingData += " V\n"; // Include newline character for better formatting
  
     delay(500); // Delay between readings  
  }
  // LDR LOOP SETUP END


  // IMU LOOP SETUP START
    {
      digitalWrite(10, LOW); 
       
     
     dso32.getEvent(&accel, &gyro, &temp);

     accelX = accel.acceleration.y;
     accelY = accel.acceleration.z;

     Serial.print("\t\tTemperature ");
     Serial.print(temp.temperature);
     Serial.println(" deg C");

  /* Display the results (acceleration is measured in m/s^2) */
     Serial.print("\t\tAccel X: ");
     Serial.print(accel.acceleration.x, 1);
     Serial.print(" \tY: ");
     Serial.print(accel.acceleration.y, 1);
     Serial.print(" \tZ: ");
     Serial.print(accel.acceleration.z, 1);
     Serial.println(" m/s^2 ");

    housekeepingData += "Accel(m/s^2) ";
        housekeepingData += '\n';

     housekeepingData += String(accel.acceleration.x, 1);
     housekeepingData += '\n';

     housekeepingData += String(accel.acceleration.y, 1);
         housekeepingData += '\n';

     housekeepingData += String(accel.acceleration.z, 1);
  
     Serial.print("\t\tGyro X: ");
     housekeepingData += '\n';
     Serial.print(gyro.gyro.x, 1);
     Serial.print(" \tY: ");
     Serial.print(gyro.gyro.y, 1);
     Serial.print(" \tZ: ");
     Serial.print(gyro.gyro.z, 1);
     Serial.println(" radians/s ");
     Serial.println();

      housekeepingData += "Gyro(rad/s) ";
          housekeepingData += '\n';

     housekeepingData += String(gyro.gyro.x, 1);
         housekeepingData += '\n';

     housekeepingData += String(gyro.gyro.y, 1);
         housekeepingData += '\n';

     housekeepingData += String(gyro.gyro.z, 1);
         housekeepingData += '\n';

         digitalWrite(10, HIGH);

     

     int touch;

     touch = touchRead(touchUpPin);
     
     housekeepingData += "TU ";
     housekeepingData += String(touch);
     housekeepingData += '\n'; // Append newline character

     touch = touchRead(touchDownPin);
     
     housekeepingData += "TD ";
     housekeepingData += String(touch);
     housekeepingData += '\n'; // Append newline character

     touch = touchRead(touchLeftPin);
     
     housekeepingData += "TL ";
     housekeepingData += String(touch);
     housekeepingData += '\n'; // Append newline character

     touch = touchRead(touchRightPin);
     
     housekeepingData += "TR ";
     housekeepingData += String(touch);
     housekeepingData += '\n'; // Append newline character


     touch = touchRead(touchXPin);
     housekeepingData += "TX ";
     housekeepingData += String(touch);
     housekeepingData += '\n'; // Append newline character

     // Calculate the elapsed time since the start
  unsigned long elapsedTime = millis() - startTime;

  // Include the timestamp in the housekeeping data
  housekeepingData += "Timestamp: ";
  housekeepingData += String(elapsedTime / 1000); // Convert milliseconds to seconds
  housekeepingData += "s\n";

  housekeepingData += "Servo Position: ";
  housekeepingData += String(servoposition);

    delay(1000);
  }
  // IMU LOOP END

  
  // MQTT LOOP START
    
    {
      if (!client.connected()) {
     reconnect();
     }
     client.loop();

    // Send housekeeping data

// Iterate over each character in the original string
     // Publish message
     if (client.publish(topic, housekeepingData.c_str())) {
     Serial.println("Message sent successfully");
     } else {
     Serial.println("Failed to send message");
     }

    // Wait for one second before sending the next message
     delay(1000);
  }
  // MQTT LOOP END
  
   DisplayMode(mode0Triggered,mode1Triggered,mode2Triggered,mode3Triggered,mode4Triggered,mode5Triggered);
   // Print housekeeping data to serial monitor
   Serial.println(housekeepingData);

   // Keep MQTT connection active
   client.loop();
}

/**
 * @brief Function Responsible for calling respective mode functions on their trigger.
 * @param mode0Triggered
 * @param mode1Triggered
 * @param mode2Triggered
 * @param mode3Triggered
 * @param mode4Triggered
 * @param mode5Triggered
 * @return (void)
 */
void DisplayMode(bool mode0Triggered, bool mode1Triggered, bool mode2Triggered, bool mode3Triggered, bool mode4Triggered, bool mode5Triggered) {
  // Check if current mode is valid
  if (currentModeIsValid()) {
    // Prepare display
    prepareDisplay();
    // Print mode information
    printModeInfo();
    // Update display
    display.display();
  }

  // Handle mode triggers
  if (mode0Triggered) {
    // Handle mode 0 trigger based on default mode
    switch(defaultMode) {
      case 1:
        nextMode = 1;
        break;
      case 2:
        nextMode = 2;
        break;
      case 3:
        nextMode = 3;
        break;
      case 4:
        nextMode = 4;
        break;
      case 5:
        nextMode = 5;
        break;
      default:
        // Handle invalid default mode
        break;
    }
  }

  // Handle mode 1 trigger
  if (mode1Triggered) {
    mode1();
  }

  // Handle mode 2 trigger
  if (mode2Triggered) {
    mode2();
  }

  // Handle mode 3 trigger
  if (mode3Triggered) {
    mode3();
  }

  // Handle mode 4 trigger
  if (mode4Triggered) {
    mode4();
  }

  // Handle mode 5 trigger
  if (mode5Triggered) {
    mode5();
  }
}


//End of main.cpp :)