// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Servo.h>
#include <LiquidCrystal.h>

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high
//MPU6050 accelgyro(0x68, &Wire1); // <-- use for AD0 low, but 2nd Wire (TWI/I2C) object

int16_t ax, ay, az;
int16_t gx, gy, gz;

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO


#define LED_PIN 13
Servo DOOR_SERVO;
#define LIFT_PIN 9
#define PIR_PIN 8

#define NOTE_C4  262
#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_B3  247
#define NOTE_C4  262

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};


bool blinkState = false;

// help check for shakiness
double prev_speeds[20] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float seconds = 0.0;
int degreesTurned = 0;
bool stoplift = false;
bool liftOpen = true;
int degreesTurnedDoor = 0;
bool is_shaky;
bool is_played = false;
bool is_played_second = false;


void setup() {
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
      
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // Wire.setWireTimeout(300, true); // if the gyro "freezes" after a while then it will "unfreeze itself"

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values
    /*
    Serial.println("Updating internal sensor offsets...");
    // -76	-2359	1688	0	0	0
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    */

    // [-2889,-2888] --> [-5,10]	[1003,1004] --> [-8,11]	[813,814] --> [16377,16401]	[-24,-23] --> [0,4]	[17,18] --> [0,3]	[-51,-50] --> [0,2]
    accelgyro.setXAccelOffset(-2889);
    accelgyro.setYAccelOffset(1003);
    accelgyro.setZAccelOffset(813);
    accelgyro.setXGyroOffset(-24);
    accelgyro.setYGyroOffset(17);
    accelgyro.setZGyroOffset(-51);
    /*
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    */

    // configure Arduino LED pin for output
    pinMode(LED_PIN, OUTPUT);
    DOOR_SERVO.attach(10);
    DOOR_SERVO.write(0);
    // LIFT_SERVO.attach(9);
    pinMode(LIFT_PIN, OUTPUT);
    pinMode(PIR_PIN, INPUT);
    
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // conversion
    // value of g on Earth is 9.8 m/s^2, 

    // converted to m/s^2
    double ax_converted = (double)ax / 16384 * 9.8;
    double ay_converted = (double)ay / 16384 * 9.8;
    double az_converted = (double)az / 16384 * 9.8;

    // converted to degrees per second
    double gx_converted = (double)gx / 131;
    double gy_converted = (double)gy / 131;
    double gz_converted = (double)gz / 131;


    #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        Serial.print("a/g:\t");
        Serial.print("Acceleration-x:"); Serial.print("\t"); Serial.print(ax_converted); Serial.print("\t");
        Serial.print("Acceleration-y:"); Serial.print("\t"); Serial.print(ay_converted); Serial.print("\t");
        Serial.print("Acceleration-z:"); Serial.print("\t"); Serial.print(az_converted); Serial.print("\t");
        Serial.print("Gyroscope-x"); Serial.print("\t"); Serial.print(gx_converted); Serial.print("\t");
        Serial.print("Gyroscope-y"); Serial.print("\t"); Serial.print(gy_converted); Serial.print("\t");
        Serial.print("Gyroscope-z"); Serial.print("\t"); Serial.println(gz_converted);

    //     Serial.print("a/g:\t");
    //     Serial.print("Acceleration-x:"); Serial.print("\t"); Serial.print(ax); Serial.print("\t");
    //     Serial.print("Acceleration-y:"); Serial.print("\t"); Serial.print(ay); Serial.print("\t");
    //     Serial.print("Acceleration-z:"); Serial.print("\t"); Serial.print(az); Serial.print("\t");
    //     Serial.print("Gyroscope-x"); Serial.print("\t"); Serial.print(gx); Serial.print("\t");
    //     Serial.print("Gyroscope-y"); Serial.print("\t"); Serial.print(gy); Serial.print("\t");
    //     Serial.print("Gyroscope-z"); Serial.print("\t"); Serial.println(gz);
    #endif

    // #ifdef OUTPUT_BINARY_ACCELGYRO
    //     Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
    //     Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
    //     Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
    //     Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
    //     Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
    //     Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
    // #endif

    int val = digitalRead(PIR_PIN);
    Serial.print("PIR pin: ");
    Serial.print(val);

    // threshold
    int THRESHOLD = 2; // assign later
    for (int i = 1; i < 20; i++) {
      prev_speeds[i-1] = prev_speeds[i];
    }
    // prev_speeds[std::end(prev_speeds) - std::begin(prev_speeds) -1] = i;

    // we will do for x-accel first bc idk (can be a vector which might be better)
    prev_speeds[20 -1] = ax_converted; 
    
    // to find max and min in this array
    double lowest = 20.0; // 2g is 2 * 9.8 = 19.6, which is the highest possible value as of now
    double highest = - 20.0; // same as above but negative

    for (int i = 0; i < 20; i++) {
      if (prev_speeds[i] < lowest ) {
        lowest = prev_speeds[i];
      } else if (prev_speeds[i] > highest) {
        highest = prev_speeds[i];
      }
    }

    if (highest - lowest > THRESHOLD) {
      is_shaky = true;
      Serial.print("ALERT");
    }

    if (is_shaky) {
      Serial.print(" seconds should be added");
      seconds += 0.1;
      // degreesTurnedDoor = 0;
    }
    Serial.print(seconds);

    if (seconds >= 2.0 || stoplift) {
      digitalWrite(LIFT_PIN, LOW);
    } else {
      Serial.print("lift pin is high");
      digitalWrite(LIFT_PIN, HIGH);
    }
    if (!is_played && seconds >= 2) {
      for (int thisNote = 0; thisNote < 8; thisNote++) {

        // to calculate the note duration, take one second
        // divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(11, melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(8);
      }
      is_played = true;
    }

    if (seconds >= 2 && seconds < 8) {
      
      lcd.print("Emergency:");
      lcd.setCursor(0, 1);
      lcd.print("Do not use lift");
      lcd.setCursor(0, 2);
      

      stoplift = true;

      // is_shaky = false;
      liftOpen = true;
      



      // if (val > 0.0 && degreesTurnedDoor < 180) {
      if (degreesTurnedDoor < 180) {
        degreesTurnedDoor += 10;
    
        Serial.println(degreesTurnedDoor);
        DOOR_SERVO.write(degreesTurnedDoor);
      }

    }
    if (seconds >= 9 && liftOpen) {
      if (!is_played_second) {
        for (int thisNote = 0; thisNote < 8; thisNote++) {

          // to calculate the note duration, take one second
          // divided by the note type.
          //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
          int noteDuration = 1000 / noteDurations[thisNote];
          tone(11, melody[thisNote], noteDuration);

          // to distinguish the notes, set a minimum time between them.
          // the note's duration + 30% seems to work well:
          int pauseBetweenNotes = noteDuration * 1.30;
          delay(pauseBetweenNotes);
          // stop the tone playing:
          noTone(8);
        }
        is_played_second = true;
      }
      
    
      is_shaky = false;
      degreesTurnedDoor -= 10;
      if (degreesTurnedDoor == 0) {
        seconds = 0;
      }
      
      Serial.println(degreesTurnedDoor);
      DOOR_SERVO.write(degreesTurnedDoor);
      

    }
    // idk whether this is fine
    // else {
    //   is_shaky = false
    // }

    // for printing to serial monitor
    // for (int i = 0; i < 20; i++) {
    //   cout << prev_speeds[i] << " ";
    // }
    // cout << "\n";



    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);

    // if (degreesTurned == 360) {
    //   degreesTurned = 0;
    // }

    // for the lift servo
    // if (!stoplift) {
    //   degreesTurned += 60;
    // }

    
    // LIFT_SERVO.write(degreesTurned);
    // total delay is 100

    // LIFT_SERVO.write(180);

    delay(100);
}
