//GENERAL LIBRARIES
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_LSM6DSO32.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>  
#include <RadioLib.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>

// External variables
extern String housekeepingData; 
extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;
extern const int mqtt_port;
extern const char* mqtt_user;
extern const char* mqtt_password;
extern const char* topic;
extern volatile bool receivedFlag;
extern unsigned long startTime;
extern int servoposition;

extern int currentMode;
extern bool mode0Triggered;
extern bool mode1Triggered;
extern bool mode2Triggered;
extern bool mode3Triggered;
extern bool mode4Triggered;
extern bool mode5Triggered;
extern float accelX;
extern float accelY;
extern bool alarmActive;
extern float temperature;
extern float pressure;
extern float altitude;
extern int setTime;
extern const int servoMinAngle;
extern const int servoMaxAngle;
extern const int dotDuration;
extern const int dashDuration;
extern const int interElementSpace;

extern int dim_environment_adc;
extern int medium_environment_adc;
extern int bright_environment_adc;


extern WiFiClientSecure espClient;
extern PubSubClient client;
extern Adafruit_SSD1306 display;
extern sensors_event_t accel;
extern sensors_event_t gyro;
extern sensors_event_t temp;
extern Adafruit_BME280 bme;
extern Adafruit_LSM6DSO32 dso32;
extern Servo myservo;

// Screen dimensions
static const int SCREEN_WIDTH = 128;
static const int SCREEN_HEIGHT = 32;
static const int OLED_RESET = -1;

// Number of modes
static const int NUM_MODES = 6;

// Temperature thresholds
extern float TEMPERATURE_THRESHOLD_HIGH; // Adjust as needed
extern float TEMPERATURE_THRESHOLD_LOW;  // Adjust as needed

// SOS signal duration
static const int SOS_DURATION = 3000; // Duration in milliseconds for SOS signal

// Environmental sensor defines
static const float SEALEVELPRESSURE_HPA = 1013.25;

// IMU defines
static const int LSM_CS = 10;    // Chip select pin for LSM6DS
static const int LSM_SCK = 12;   // SPI SCK pin
static const int LSM_MISO = 13;  // SPI MISO pin
static const int LSM_MOSI = 11;  // SPI MOSI pin

// Servo and LDR pins
static const int servoPin = 7;
static const int channelServo = 0;
static const int ldrPin = 6;

// Touch pad pins
static const int touchUpPin = 4;
static const int touchDownPin = 1;
static const int touchLeftPin = 5;
static const int touchRightPin = 2;
static const int touchXPin = 3;
const int touchThreshold = 40000;

// Buzzer and rotary encoder pins
static const int channelBuzzer = 1; // 0 is used by servo
static const int buzzerPin = 14;

// Switches and LEDs pins
static const int sw1Pin = 0;
static const int ledGreenPin = 35;
static const int ledYellowPin = 36;
static const int ledRedPin = 37;  

// Global variable for managing modes and mode changes
extern volatile int nextMode;

// Function prototypes
void increaseModeNumber();
void decreaseModeNumber();
void initializeTouchbuttons();
void initializeGPIOs();
bool modeChanged();
void modeStartup(int mode);
bool currentModeIsValid();
void prepareDisplay();
void printModeInfo();
void activateAlarm();
void deactivateAlarm();
void moveServo(int durationInSeconds);
void displayTimeSet();
void buttonTouched();
void setup_wifi();
void reconnect();

// LDR GPIO declaration
const int LDR_PIN = 6;


//End of Utils.h :)