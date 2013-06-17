/*

Kelvin Mead
May 2013

Interactive LED panels

Design for 4 LEDs, 4 IR sensors and 4 IR Emitters
Addition for 4067, 16 channel input expander
Addition for TLC5940, 16 channel pwm expander
Reesign for 16 LEDs, 16 IR sensors and 16 IR Emitters


*/

/*
    Basic Pin setup:
    ------------                                  ---u----
    ARDUINO   13|-> SCLK (pin 25)           OUT1 |1     28| OUT channel 0
              12|                           OUT2 |2     27|-> GND (VPRG)
              11|-> SIN (pin 26)            OUT3 |3     26|-> SIN (pin 11)
              10|-> BLANK (pin 23)          OUT4 |4     25|-> SCLK (pin 13)
               9|-> XLAT (pin 24)             .  |5     24|-> XLAT (pin 9)
               8|                             .  |6     23|-> BLANK (pin 10)
               7|                             .  |7     22|-> GND
               6|                             .  |8     21|-> VCC (+5V)
               5|                             .  |9     20|-> 2K Resistor -> GND
               4|                             .  |10    19|-> +5V (DCPRG)
               3|-> GSCLK (pin 18)            .  |11    18|-> GSCLK (pin 3)
               2|                             .  |12    17|-> SOUT
               1|                             .  |13    16|-> XERR
               0|                           OUT14|14    15| OUT channel 15
    ------------                                  --------

    -  Put the longer leg (anode) of the LEDs in the +5V and the shorter leg
         (cathode) in OUT(0-15).
    -  +5V from Arduino -> TLC pin 21 and 19     (VCC and DCPRG)
    -  GND from Arduino -> TLC pin 22 and 27     (GND and VPRG)
    -  digital 3        -> TLC pin 18            (GSCLK)
    -  digital 9        -> TLC pin 24            (XLAT)
    -  digital 10       -> TLC pin 23            (BLANK)
    -  digital 11       -> TLC pin 26            (SIN)
    -  digital 13       -> TLC pin 25            (SCLK)
    -  The 2K resistor between TLC pin 20 and GND will let ~20mA through each
       LED.  To be precise, it's I = 39.06 / R (in ohms).  This doesn't depend
       on the LED driving voltage.
    - (Optional): put a pull-up resistor (~10k) between +5V and BLANK so that
                  all the LEDs will turn off when the Arduino is reset.

    If you are daisy-chaining more than one TLC, connect the SOUT of the first
    TLC to the SIN of the next.  All the other pins should just be connected
    together:
        BLANK on Arduino -> BLANK of TLC1 -> BLANK of TLC2 -> ...
        XLAT on Arduino  -> XLAT of TLC1  -> XLAT of TLC2  -> ...
    The one exception is that each TLC needs it's own resistor between pin 20
    and GND.

    This library uses the PWM output ability of digital pins 3, 9, 10, and 11.
    Do not use analogWrite(...) on these pins.

    This sketch does the Knight Rider strobe across a line of LEDs.

    Alex Leone <acleone ~AT~ gmail.com>, 2009-02-03 
*/

#include "Tlc5940.h"


// set up the leds on the pwm outputs
//  int ledpin1 = 5;
//  int ledpin2 = 6;
//  int ledpin3 = 9;
//  int ledpin4 = 10;
//  int ledpin5 = 3;
//  int ledpin6 = 11;
  
// set up the tlc pin assignments
  int led1 = 0;
  int led2 = 1;
  int led3 = 2;
  int led4 = 3;
  int led5 = 4;
  int led6 = 5;
  int led7 = 6;
  int led8 = 7;
  int led9 = 8;
  int led10 = 9;
  int led11 = 10;
  int led12 = 11;
  int led13 = 12;
  int led14 = 13;
  int led15 = 14;
  int led16 = 15;

  int lastBright[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};

//Mux control pins 4067
  int s0 = 2;
  int s1 = 4;
  int s2 = 7;
  int s3 = 8;

//Mux in "SIG" pin 4067
  int SIG_pin = 0;
  
// fade speed
// 1 is instant, 9 is slow (not slow enough though)
  int fadespeed = 99;
  int ramp = 0;
  
// this is for changing the fade time without using the delay command  
  long previousMillis = 0;
  // speed for the fades in milliseconds
  long interval = 50;
  
// lowest brightness setting (eliminates tiny flickering errors on divisable fades
  int lowestbrightness = 1;

// full brightness  
// 4095 for the 5490
  int maxBright = 4095;
  
  //calibration routine
  int sensorValue1 = 0;         // the sensor value
  int sensorValue2 = 0;         // the sensor value
  int sensorValue3 = 0;         // the sensor value
  int sensorValue4 = 0;         // the sensor value
  int sensorValue5 = 0;         // the sensor value
  int sensorValue6 = 0;         // the sensor value
  int sensorValue7 = 0;         // the sensor value
  int sensorValue8 = 0;         // the sensor value
  int sensorValue9 = 0;         // the sensor value
  int sensorValue10 = 0;         // the sensor value
  int sensorValue11 = 0;         // the sensor value
  int sensorValue12 = 0;         // the sensor value
  int sensorValue13 = 0;         // the sensor value
  int sensorValue14 = 0;         // the sensor value
  int sensorValue15 = 0;         // the sensor value
  int sensorValue16 = 0;         // the sensor value
  
  int sensorMin1 = 1023;        // minimum sensor value
  int sensorMin2 = 1023;        // minimum sensor value
  int sensorMin3 = 1023;        // minimum sensor value
  int sensorMin4 = 1023;        // minimum sensor value
  int sensorMin5 = 1023;        // minimum sensor value
  int sensorMin6 = 1023;        // minimum sensor value
  int sensorMin7 = 1023;        // minimum sensor value
  int sensorMin8 = 1023;        // minimum sensor value
  int sensorMin9 = 1023;        // minimum sensor value
  int sensorMin10 = 1023;        // minimum sensor value
  int sensorMin11 = 1023;        // minimum sensor value
  int sensorMin12 = 1023;        // minimum sensor value
  int sensorMin13 = 1023;        // minimum sensor value
  int sensorMin14 = 1023;        // minimum sensor value
  int sensorMin15 = 1023;        // minimum sensor value
  int sensorMin16 = 1023;        // minimum sensor value
  
  int sensorMax = 1000;         // maximum sensor value

// variables for high and low led
  int sensitivity = 25;
  int highsetting1 = 1023;


void setup() {
// TLC5940
// Call Tlc.init() to setup the tlc.
//You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
  Tlc.init();

// initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
    
// setup mux pins as outputs
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 
  pinMode(s3, OUTPUT); 

// and set to low to initiate 
  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
 
  // turn on LED to signal the start of the calibration period:
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // calibrate during the first 1 second 
  while (millis() < 1000) {
    sensorValue1 = readMux(0);
    sensorValue2 = readMux(1);
    sensorValue3 = readMux(2);
    sensorValue4 = readMux(3);
    sensorValue5 = readMux(4);
    sensorValue6 = readMux(5);
    sensorValue7 = readMux(6);
    sensorValue8 = readMux(7);
    sensorValue9 = readMux(8);
    sensorValue10 = readMux(9);
    sensorValue11 = readMux(10);
    sensorValue12 = readMux(11);
    sensorValue13 = readMux(12);
    sensorValue14 = readMux(13);
    sensorValue15 = readMux(14);
    sensorValue16 = readMux(15);
   
    // record the minimum sensor value
    if (sensorValue1 < sensorMin1) {
      sensorMin1 = sensorValue1;
    }
    if (sensorValue2 < sensorMin2) {
      sensorMin2 = sensorValue2;
    }
    if (sensorValue3 < sensorMin3) {
      sensorMin3 = sensorValue3;
    }
    if (sensorValue4 < sensorMin4) {
      sensorMin4 = sensorValue4;
    }
    if (sensorValue5 < sensorMin5) {
      sensorMin5 = sensorValue5;
    }
    if (sensorValue6 < sensorMin6) {
      sensorMin6 = sensorValue6;
    }
    if (sensorValue7 < sensorMin7) {
      sensorMin7 = sensorValue7;
    }
    if (sensorValue8 < sensorMin8) {
      sensorMin8 = sensorValue8;
    }
    if (sensorValue9 < sensorMin9) {
      sensorMin9 = sensorValue9;
    }
    if (sensorValue10 < sensorMin10) {
      sensorMin10 = sensorValue10;
    }
    if (sensorValue11 < sensorMin11) {
      sensorMin11 = sensorValue11;
    }
    if (sensorValue12 < sensorMin12) {
      sensorMin12 = sensorValue12;
    }
    if (sensorValue13 < sensorMin13) {
      sensorMin13 = sensorValue13;
    }
    if (sensorValue14 < sensorMin14) {
      sensorMin14 = sensorValue14;
    }
    if (sensorValue15 < sensorMin15) {
      sensorMin15 = sensorValue15;
    }
    if (sensorValue16 < sensorMin16) {
      sensorMin16 = sensorValue16;
    }
  }
  // signal the end of the calibration period
  digitalWrite(13, LOW);    
}

void loop() {

// TLC5940
// Tlc.clear() sets all the grayscale values to zero, but does not send
// them to the TLCs.  To actually send the data, call Tlc.update() */
  Tlc.clear();

//Tlc.set(channel (0-15), value (0-4095)) sets the grayscale value for one channel (15 is OUT15 on the first TLC, if multiple TLCs are daisy-
//chained, then channel = 16 would be OUT0 of the second TLC, etc.). value goes from off (0) to always on (4095).
//Like Tlc.clear(), this function only sets up the data, Tlc.update() will send the data.
// set variable channel for the led number (already used channel earlier in code)
//    Tlc.set(led1, 4095);
    
// Tlc.update() sends the data to the TLCs.  This is when the LEDs will actually change. */
//    Tlc.update();
    
  // read sensor values
  sensorValue1 = readMux(0);
  sensorValue2 = readMux(1);
  sensorValue3 = readMux(2);
  sensorValue4 = readMux(3);
  sensorValue5 = readMux(4);
  sensorValue6 = readMux(5);
  sensorValue7 = readMux(6);
  sensorValue8 = readMux(7);
  sensorValue9 = readMux(8);
  sensorValue10 = readMux(9);
  sensorValue11 = readMux(10);
  sensorValue12 = readMux(11);
  sensorValue13 = readMux(12);
  sensorValue14 = readMux(13);
  sensorValue15 = readMux(14);
  sensorValue16 = readMux(15);
  
  // apply the calibration to the sensor reading
  // max is set to 4095 for the 5940
  int mappedValue1 = map(sensorValue1, sensorMin1, sensorMax, 0, 255);
  int mappedValue2 = map(sensorValue2, sensorMin2, sensorMax, 0, 255);
  int mappedValue3 = map(sensorValue3, sensorMin3, sensorMax, 0, 255);
  int mappedValue4 = map(sensorValue4, sensorMin4, sensorMax, 0, 255);
  int mappedValue5 = map(sensorValue5, sensorMin5, sensorMax, 0, 255);
  int mappedValue6 = map(sensorValue6, sensorMin6, sensorMax, 0, 255);
  int mappedValue7 = map(sensorValue7, sensorMin7, sensorMax, 0, 255);
  int mappedValue8 = map(sensorValue8, sensorMin8, sensorMax, 0, 255);
  int mappedValue9 = map(sensorValue9, sensorMin9, sensorMax, 0, 255);
  int mappedValue10 = map(sensorValue10, sensorMin10, sensorMax, 0, 255);
  int mappedValue11 = map(sensorValue11, sensorMin11, sensorMax, 0, 255);
  int mappedValue12 = map(sensorValue12, sensorMin12, sensorMax, 0, 255);
  int mappedValue13 = map(sensorValue13, sensorMin13, sensorMax, 0, 255);
  int mappedValue14 = map(sensorValue14, sensorMin14, sensorMax, 0, 255);
  int mappedValue15 = map(sensorValue15, sensorMin15, sensorMax, 0, 255);
  int mappedValue16 = map(sensorValue16, sensorMin16, sensorMax, 0, 255);


//  if (Serial.available() > 0) {
//    int inByte = Serial.read();
//    // case examples for different fades
//    switch (inByte) {
//    case 'a':    
//      // do something here
//      // no fade & full brightness
//      fadespeed = 1;
//      maxBright = 255;
//      break;
//    case 'b':    
//        maxBright + 1; 
////        }
//      break;
//    case 'c':    
//      // do something here
//      fadespeed = 9;
//      delay(30);
//      break;
//    case 'd':    
//      // do something here
//      break;
//    case 'e':    
//      // do something here
//      break;
//    default:
//      // do something standard here
//      // full brightness, longest fade
//      maxBright = 255;
//      fadespeed = 9;
//    } 
 
  for (int i = 1; i < 16; i++) {
static int lastBright[i] = 0;                                                    // set the lastbright variable
if (mappedValue[i]<sensitivity) {                                                // check to see if the ir is detecting above the base level 
      unsigned long currentMillis = millis();
      if (lastBright[i] > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright[i] = lastBright[i]/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis; 
}  else {
  lastBright[i] = maxBright;                                                     //  Set the brightness to full
}
    Tlc.set(led[i], lastBright[i]);                                                  // set the brightness of the led1
    Tlc.update();                                                                // send the data to the tlc
  }  
  
/*


  static int lastBright2 = 0;                                                    // set the lastbright variable
if (mappedValue2<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led2, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright2 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright2 = lastBright2/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright2 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright1 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright1 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led2, lastBright2);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright3 = 0;                                                    // set the lastbright variable
if (mappedValue3<sensitivity) {                                                // check to see if the ir is detecting above the base level 
  Tlc.set(led3, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
      unsigned long currentMillis = millis();
      if (lastBright3 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright3 = lastBright3/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright3 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright3 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright3 = 0;                                                           // then turn the led off
  }
  Tlc.set(led3, lastBright3);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright4 = 0;                                                    // set the lastbright variable
if (mappedValue4<sensitivity) {                                                // check to see if the ir is detecting above the base level 
  Tlc.set(led4, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
      unsigned long currentMillis = millis();
      if (lastBright4 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright4 = lastBright4/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright4 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright4 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright4 = 0;                                                           // then turn the led off
  }
  Tlc.set(led4, lastBright4);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright5 = 0;                                                    // set the lastbright variable
if (mappedValue5<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led5, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright5 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright5 = lastBright5/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright5 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright5 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright5 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led5, lastBright5);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright6 = 0;                                                    // set the lastbright variable
if (mappedValue6<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led6, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright6 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright6 = lastBright6/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright6 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright6 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright6 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led6, lastBright6);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright7 = 0;                                                    // set the lastbright variable
if (mappedValue7<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led7, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright7 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright7 = lastBright7/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright7 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright7 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright7 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led7, lastBright7);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright8 = 0;                                                    // set the lastbright variable
if (mappedValue8<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led8, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright8 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright8 = lastBright8/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright8 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright8 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright8 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led8, lastBright8);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright9 = 0;                                                    // set the lastbright variable
if (mappedValue9<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led9, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright9 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright9 = lastBright9/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright9 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright9 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright9 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led9, lastBright9);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright10 = 0;                                                    // set the lastbright variable
if (mappedValue10<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led10, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright10 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright10 = lastBright10/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright10 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright10 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright10 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led10, lastBright10);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright11 = 0;                                                    // set the lastbright variable
if (mappedValue11<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led11, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright11 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright11 = lastBright11/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright11 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright11 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright11 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led11, lastBright11);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright12 = 0;                                                    // set the lastbright variable
if (mappedValue12<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led12, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright12 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright12 = lastBright12/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright12 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright12 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright12 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led12, lastBright12);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright13 = 0;                                                    // set the lastbright variable
if (mappedValue13<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led13, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright13 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright13 = lastBright13/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright13 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright13 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright13 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led13, lastBright13);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright14 = 0;                                                    // set the lastbright variable
if (mappedValue14<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led14, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright14 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright14 = lastBright14/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright14 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright14 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright14 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led14, lastBright14);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright15 = 0;                                                    // set the lastbright variable
if (mappedValue15<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led15, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright15 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright15 = lastBright15/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright15 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright15 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright15 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led15, lastBright15);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
  
  static int lastBright16 = 0;                                                    // set the lastbright variable
if (mappedValue16<sensitivity) {                                                // check to see if the ir is detecting above the base level 
//  Tlc.clear();                                                               // clear all pins   // probably not needed
  Tlc.set(led16, 0);                                                            // set tlc of led1 to 0
  Tlc.update();                                                                // send the data to the tlc chip
//  analogWrite(ledpin1, LOW);                                                 //old code             // turn the led off
      unsigned long currentMillis = millis();
      if (lastBright16 > 0 && (currentMillis - previousMillis > interval))      // check to see if the last brightness is more than 0 and the interval time has passed
      lastBright16 = lastBright16/100*fadespeed;                                 // Dim the light til it reaches 0
      previousMillis = currentMillis;                            
}  else {
  lastBright16 = maxBright;                                                     //  Set the brightness to full
}
  if (lastBright16 < lowestbrightness) {                                        // check to see if the led has got to a low level
    lastBright16 = 0;                                                           // then turn the led off
  }
//  analogWrite(ledpin1, lastBright1);                                         // old code            // write the brightness to the led
  Tlc.set(led16, lastBright16);                                                  // set the brightness of the led1
  Tlc.update();                                                                // send the data to the tlc
 
 
 
*/ 
 
  
// print out the value you read: WARNING... activating this slows the entire program down
  Serial.print(" 1:Mux "),Serial.print(readMux(0)), Serial.print(" lastBright1:"),Serial.print(lastBright1), Serial.print(" mappedValue1:"), Serial.print(mappedValue1), Serial.print(" sensorMin1:"), Serial.print(sensorMin1), Serial.print(" sensitivity:"), Serial.print(sensitivity); 
  Serial.println(" "); 
  
// delay for the crack of it
//  delay(500);

}

// single definition code for the reading of the 16 channels from the 4067
// for some reason this bit of code is chucked at the end of the program
  int readMux (int channel) {
  digitalWrite(s3,channel & 8);
  digitalWrite(s2,channel & 4);
  digitalWrite(s1,channel & 2);
  digitalWrite(s0,channel & 1);
  return analogRead(SIG_pin);
  }
