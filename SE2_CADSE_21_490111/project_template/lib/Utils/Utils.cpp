//DEFAULT LIBRARIES
#include <Arduino.h>
#include <Utils.h>

//LIBRARIES FOR OLED SCREEN
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//LIBRARIES FOR ENVIRONMENTAL SENSOR
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//LIBRARIES FOR IMU SENSORS
#include <Adafruit_LSM6DSO32.h> //LSM6DS"O"32 and not LSM6DS"0"32


//LIBRARIES FOR MQTT SYSTEM
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>  
#include <RadioLib.h>
#include <ArduinoJson.h>
#include <Melodies.h>
#include <arduino_secrets.h>

// LIBRARIES FOR SERVO UNIT
#include <ESP32Servo.h> //(WORKING, TEMP ON HOLD FOR FUTURE SETUP)


// Shared Variables
extern int currentMode; // Declaration of external variable for current mode
int volatile nextMode = 0; // Next mode to be set, marked as volatile for interrupts
String housekeepingData = ""; // Variable to store housekeeping data
int servoposition = -90;
float TEMPERATURE_THRESHOLD_HIGH = 45.0;
float TEMPERATURE_THRESHOLD_LOW = 15.0;
int dim_environment_adc = 376;
int medium_environment_adc = 1900;
int bright_environment_adc = 3500;

// WiFi and MQTT configuration
const char* ssid = SECRET_WIFI_SSID;
const char* password = SECRET_WIFI_PASSWORD;
const char* mqtt_server = SECRET_MQTT_BROKER;
const int mqtt_port = SECRET_MQTT_PORT;
const char* mqtt_user = SECRET_MQTT_USER;
const char* mqtt_password = SECRET_MQTT_PASSWORD;
std::string topicp = std::string(SECRET_MQTT_PREFIX) + "/" + std::to_string(SECRET_MQTT_YEAR) + "/" + std::to_string(SECRET_MQTT_BOARDID);
const char* topic = topicp.c_str();
std::string topic_subscribe = std::string(SECRET_MQTT_PREFIX) + "/" + std::to_string(SECRET_MQTT_YEAR) + "/" + "broadcast";


volatile bool receivedFlag = false; // Flag indicating if message received
unsigned long startTime = 0; // Time since program started

// Mode variables
int currentMode = 0;
bool mode0Triggered = false;
bool mode1Triggered = false;
bool mode2Triggered = false;
bool mode3Triggered = false;
bool mode4Triggered = false;
bool mode5Triggered = false;

// Sensor data variables
float accelX = 0.0;
float accelY = 0.0;
bool alarmActive = false;
float temperature = 0;
float pressure = 0;
float altitude = 0;
int setTime = 0; // Time set for alarm
const int servoMinAngle = 0;
const int servoMaxAngle = 180;
const int dotDuration = 100; // Duration for Morse code dot
const int dashDuration = 300; // Duration for Morse code dash
const int interElementSpace = 100; // Space between Morse code elements

// MQTT client and WiFi client
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Sensor event variables
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;
Adafruit_BME280 bme; // I2C
Adafruit_LSM6DSO32 dso32;
Servo myservo;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void increaseModeNumber(){
  //int maxTouch = max(touchRead(touchUpPin), touchRead(touchRightPin));
  int maxTouch = touchRead(touchRightPin);
  if( currentMode < 6 && maxTouch > touchThreshold ){
    nextMode = currentMode + 1;
  } 
}

void decreaseModeNumber(){
  //int maxTouch = max(touchRead(touchDownPin), touchRead(touchLeftPin));
  int maxTouch = touchRead(touchLeftPin);
  if( currentMode > -1 && maxTouch > touchThreshold ){
    nextMode = currentMode - 1;
  }
}

void initializeTouchbuttons(){
  touchAttachInterrupt(touchRightPin, increaseModeNumber, touchThreshold);
  touchAttachInterrupt(touchLeftPin, decreaseModeNumber, touchThreshold);
}

void initializeGPIOs(){
  // Set LED pins as output
  pinMode(ledYellowPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT); 
  // Setup buzzer using LEDC
  ledcSetup(channelBuzzer,1000,12);
  ledcAttachPin(buzzerPin,channelBuzzer);
}

bool modeChanged(){
  return (currentMode != nextMode);
}

void modeStartup(int mode){
  Serial.print("Switching to mode ");
  Serial.println(mode);     
}

void buttonTouched() {
}

//TIMER SETUP X MODE 3
void displayTimeSet() {
  // Clear display
  display.clearDisplay();
  // Set text size and color
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  // Set cursor position
  display.setCursor(0, 0);
  // Print time set
  display.print("Time Set: ");
  display.print(setTime);
  display.print(" sec");
  // Update display
  display.display();
}

//FUNCTIONAL SETUP(servo, buzzer, diaplay) X MODE 3
/**
 * @brief Function responsible for servo movement in Mode 2
 * @param durationInSeconds
 * @return (void)
 */
void moveServo(int durationInSeconds) {
  myservo.write(-90);
  int totalTime = durationInSeconds * 1000;
  
  // Calculate the time interval per angle step
  float timePerStep = (float)totalTime / (servoMaxAngle - servoMinAngle);
  
  // Record the start time
  unsigned long startTime = millis();

  // Main loop for servo movement
  for (int angle = servoMinAngle; angle <= servoMaxAngle; angle++) {
    // Calculate the elapsed time
    servoposition=angle;
    unsigned long elapsedTime = millis() - startTime;

    // Calculate the remaining time
    int remainingTime = max(0, static_cast<int>(durationInSeconds - (elapsedTime / 1000)));

    // Display the remaining time on the OLED display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Remaining Time:");
    display.setCursor(0, 10);
    display.print(remainingTime);
    display.display();

    // Calculate the pulse width corresponding to the current angle
    int pulseWidth = map(angle, -90, 90, 1000, 2000); // assuming servo operates in 0-180 degree range
    
    // Write the pulse width to the servo
    myservo.writeMicroseconds(pulseWidth);
    
    // Wait for the specified time interval
    delay(timePerStep);
  }
  
  // Play melody after servo movement
  play_melody(mario, sizeof(mario), channelBuzzer);
   myservo.write(-90);
   servoposition=-90;
}

//ALARM FUNCTION X MODE2
/**
 * @brief Function responsible for Alarm Activation
 * @return (void)
 */
void activateAlarm() {
  alarmActive = true;
  
  // Display SOS signal on OLED
  for (int i = 0; i < 3; i++) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 0);
    display.println("...---...");
    display.display();
    delay(500); // Display "SOS" for 500 milliseconds

    display.clearDisplay();
    display.display(); // Turn off display
    delay(1000); // Wait for 500 milliseconds before repeating
  }


//SOS SIGNAL IN BUZZER
 // Dot loop (...)
  for (int i = 0; i < 3; i++) {
    // Dot
    tone(buzzerPin, 1000); // Start tone
    delay(dotDuration); // Dot duration
    noTone(buzzerPin); // Stop tone
    delay(interElementSpace); // Inter-element space
  }

// Dash loop (---)
  for (int i = 0; i < 3; i++) {
    // Dash
    tone(buzzerPin, 1000); // Start tone
    delay(dashDuration); // Dash duration
    noTone(buzzerPin); // Stop tone
    delay(interElementSpace); // Inter-element space
  }

// Dot loop (...)
  for (int i = 0; i < 3; i++) {
    // Dot
    tone(buzzerPin, 1000); // Start tone
    delay(dotDuration); // Dot duration
    noTone(buzzerPin); // Stop tone
    delay(interElementSpace); // Inter-element space
  }
  delay(5000); // Wait before rechecking temperature
}
//ALARM FUNCTION X MODE2
void deactivateAlarm() {
  alarmActive = false;
  noTone(buzzerPin); // Stop buzzer
}

// MQTT MISC VOID SETUP
void setup_wifi() {
  // Initial delay for stability
  delay(10);
  // Connect to WiFi network
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  // Wait until connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Connection successful, print IP address
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


/**
 * @brief MQTT COnnection
 * @return (void)
 */
void reconnect() {
    // Attempt MQTT connection until successful
    while (!client.connected()) {
        Serial.println("Attempting MQTT connection...");
        // Attempt to connect to MQTT broker
        if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
            // Connection successful
            Serial.println("connected");
            // Subscribe to MQTT topic
            client.subscribe(topic_subscribe.c_str());
        } else {
            // Connection failed, print error code and retry after delay
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}


bool currentModeIsValid() {
    return (currentMode >= 0 && currentMode <= 5);
}


//OLED DISPLAY FUNCTIONS
/**
 * @brief Display Function
 * @return (void)
 */
void prepareDisplay() {
    // Clear the display
    display.clearDisplay();
    // Set cursor position
    display.setCursor(25, 5);
    // Set text size
    display.setTextSize(2);
    // Set text color
    display.setTextColor(WHITE);
    // Rotate display (if needed)
    display.setRotation(2);
}

void printModeInfo() {
    display.print("Mode ");
    display.print(currentMode);
  
}

// End of Utils.cpp :) 