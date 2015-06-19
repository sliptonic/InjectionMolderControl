/* Serial 7-Segment Display Example Code
    Serial Mode Stopwatch
   by: Jim Lindblom
     SparkFun Electronics
   date: November 27, 2012
   license: This code is public domain.

   This example code shows how you could use software serial
   Arduino library to interface with a Serial 7-Segment Display.

   There are example functions for setting the display's
   brightness, decimals and clearing the display.

   The print function is used with the SoftwareSerial library
   to send display data to the S7S.

   Circuit:
   Arduino -------------- Serial 7-Segment
     5V   --------------------  VCC
     GND  --------------------  GND
      8   --------------------  RX
*/
#include <SoftwareSerial.h>

// These are the Arduino pins required to create a software seiral
//  instance. We'll actually only use the TX pin.
const int softwareTx = 8;
const int softwareRx = 7;

SoftwareSerial s7s(softwareRx, softwareTx);

unsigned int counter = 0;  // This variable will count up to 65k
char tempString[10];  // Will be used with sprintf to create strings

void setup()
{
  // Must begin s7s software serial at the correct baud rate.
  //  The default of the s7s is 9600.
  s7s.begin(9600);

  // Clear the display, and then turn on all segments and decimals
  clearDisplay();  // Clears display, resets cursor
  s7s.print("-HI-");  // Displays -HI- on all digits
  setDecimals(0b111111);  // Turn on all decimals, colon, apos

  // Flash brightness values at the beginning
  setBrightness(0);  // Lowest brightness
  delay(1500);
  setBrightness(127);  // Medium brightness
  delay(1500);
  setBrightness(255);  // High brightness
  delay(1500);

  // Clear the display before jumping into loop
  clearDisplay();  
}

void loop()
{
  // Magical sprintf creates a string for us to send to the s7s.
  //  The %4d option creates a 4-digit integer.
  sprintf(tempString, "%4d", counter);

  // This will output the tempString to the S7S
  s7s.print(tempString);
  setDecimals(0b00000100);  // Sets digit 3 decimal on

  counter++;  // Increment the counter
  delay(100);  // This will make the display update at 10Hz.
}

// Send the clear display command (0x76)
//  This will clear the display and reset the cursor
void clearDisplay()
{
  s7s.write(0x76);  // Clear display command
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightness(byte value)
{
  s7s.write(0x7A);  // Set brightness command byte
  s7s.write(value);  // brightness data byte
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimals(byte decimals)
{
  s7s.write(0x77);
  s7s.write(decimals);
}

