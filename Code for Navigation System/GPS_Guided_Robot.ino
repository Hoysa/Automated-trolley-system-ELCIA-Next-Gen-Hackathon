#include <AFMotor.h>              // the Adafruit Motor Shield Library ver. 1
#include "Wire.h"                 // Used by I2C and QMC5883L compass
#include <QMC5883LCompass.h>      // Library for the QMC5883L compass
#include <Servo.h>                // Servo library to control Servo arm for metal detector
#include <SoftwareSerial.h>       // Software Serial for Serial Communications - not used
#include <TinyGPS++.h>            // Tiny GPS Plus Library

//******************************************************************************************************                                                                  
// GPS Variables & Setup
int GPS_Course;                      // variable to hold the gps's determined course to destination
int Number_of_SATS;                  // variable to hold the number of satellites acquired
TinyGPSPlus gps;                     // gps = instance of TinyGPS 

//******************************************************************************************************
// Setup Drive Motors using the Adafruit Motor Controller version 1.0 Library
AF_DCMotor motor1(1, MOTOR12_64KHZ); // create motor #1, 64KHz pwm
AF_DCMotor motor2(2, MOTOR12_64KHZ); // create motor #2, 64KHz pwm
AF_DCMotor motor3(3, MOTOR12_64KHZ); // create motor #3, 64KHz pwm
AF_DCMotor motor4(4, MOTOR12_64KHZ); // create motor #4, 64KHz pwm

int turn_Speed = 175;                // motor speed when using the compass to turn left and right
int mtr_Spd = 250;                   // motor speed when moving forward and reverse

//******************************************************************************************************
// Compass Variables & Setup
QMC5883LCompass compass;             // QMC5883L compass
int desired_heading;                 // initialize variable - stores value for the new desired heading
int compass_heading;                 // initialize variable - stores value calculated from compass readings
int compass_dev = 5;                 // the amount of deviation that is allowed in the compass heading - Adjust as Needed

int Heading_A;                       // variable to store compass heading
int Heading_B;                       // variable to store compass heading in Opposite direction
int pass = 0;                        // variable to store which pass the robot is on

//******************************************************************************************************
// Servo Control
Servo myservo;                       // create servo object to control a servo 
int pos = 0;                         // variable to store the servo position

//******************************************************************************************************
// Ping Sensor for Collision Avoidance
boolean pingOn = false;              // Turn Collision detection On or Off

int trigPin = 43;                    // Trig - Orange
int echoPin = 42;                    // Echo - Yellow
long duration, inches;
int Ping_distance;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;    // Store last time Ping was updated
const long interval = 200;           // Ping the Distance every X milliseconds
 
//******************************************************************************************************
// Bluetooth Variables & Setup
String str;                          // raw string received from android to arduino
int blueToothVal;                    // stores the last value sent over via bluetooth
int bt_Pin = 34;                     // Pin 34 of the Aruino Mega used to test the Bluetooth connection status - Not Used

//*****************************************************************************************************
// GPS Locations
unsigned long Distance_To_Home;      // variable for storing the distance to destination

int ac = 0;                          // GPS array counter
int wpCount = 0;                     // GPS waypoint counter
double Home_LATarray[50];            // variable for storing the destination Latitude - Only Programmed for 5 waypoint
double Home_LONarray[50];            // variable for storing the destination Longitude - up to 50 waypoints

int increment = 0;

void setup() {  
  Serial.begin(115200);              // Serial 0 is for communication with the computer
  Serial1.begin(9600);               // Serial 1 is for Bluetooth communication - DO NOT MODIFY
  Serial2.begin(9600);               // Serial 2 is for GPS communication at 9600 baud - DO NOT MODIFY
  myservo.attach(9);                 // attaches the servo to pin 9 (Servo 0 on the Adafruit Motor Control Board)
  
  pinMode(36, OUTPUT);               // define pin 36 as an output for an LED indicator - Not Used
  pinMode(bt_Pin, INPUT);            // This pin(34) is used to check the status of the Bluetooth connection - Not Used

  // Ping Sensor
  pinMode(trigPin, OUTPUT);          // Ping Sensor
  pinMode(echoPin, INPUT);           // Ping Sensor

  // Compass
  Wire.begin();                      // Join I2C bus used for the QMC5883L compass
  compass.init();                    // initialize the compass (QMC5883L)
  compass.setCalibrationOffsets(70.00, -853.00, -491.00);    // Set calibration offset
  compass.setCalibrationScales(0.87, 0.77, 1.78);

  void Startup();                         // Run the Startup procedure on power-up one time
}

//********************************************************************************************************
// Main Loop
void loop() { 
  bluetooth();                       // Run the Bluetooth procedure to see if there is any data being sent via BT                                                    
  void getGPS();                          // Update the GPS location
  void getCompass();                      // Update the Compass Heading
  Ping();                            // Use at your own discretion, this is not fully tested
}
